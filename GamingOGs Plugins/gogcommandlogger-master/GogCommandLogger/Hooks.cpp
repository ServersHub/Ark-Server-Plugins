#include "Hooks.h"

#ifdef ARK
	#include <API/ARK/Ark.h>
	#include <Permissions/ArkPermissions.h>
#endif
#ifdef ATLAS
	#include <API/Atlas/Atlas.h>
	#include <Permissions/AtlasPermissions.h>
#endif

#include <DiscordMetrics/GogDiscordMetrics.h>
#include <string>

#include "ChannelManager.h"

namespace CommandLogger
{
	inline std::string chat_cmd_templ =
		"```Chat Command Received\\n"
		"Date / Time: {}\\n"
		"Ranks: {}\\n"
		"Steam Name: {}\\n"
		"SteamID: {}\\n"
		"Command: {}```";
	inline std::string console_cmd_templ =
		"```Console Command Received\\n"
		"Date / Time: {}\\n"
		"Ranks: {}\\n"
		"Staff Member Steam Name: {}\\n"
		"SteamID: {}\\n"
		"Command: {}```";
	inline std::string rcon_cmd_templ =
		"```RCON Command Received\\n"
		"Date / Time: {}\\n"
		"Command: {}```";

	inline bool IsStaff(const TArray<FString>& groups)
	{
		for (const auto& group : GetConfig()["StaffGroups"]) {
			if (groups.Contains(FString(static_cast<std::string>(group))))
				return true;
		}

		return false;
	}

	DECLARE_HOOK(AShooterPlayerController_ServerSendChatMessage_Impl, void, AShooterPlayerController*, FString*, EChatSendMode::Type);
	DECLARE_HOOK(APlayerController_ConsoleCommand, FString*, APlayerController*, FString*, FString*, bool);
	DECLARE_HOOK(RCONClientConnection_ProcessRCONPacket, void, RCONClientConnection*, RCONPacket*, UWorld*);

	void Hook_AShooterPlayerController_ServerSendChatMessage_Impl( AShooterPlayerController* player_controller, FString* message, EChatSendMode::Type mode)
	{
		bool should_log = (message->StartsWith(L"/") && !IgnoreList->IsCommandIgnored(*message, CommandType::Chat));
		should_log = !IgnoreList->IsCommandBlacklisted(*message, CommandType::Chat);

		if (should_log) {
			std::shared_ptr<DiscordChannelData> discord_chan_data = ChanManager->FindChannelByCommand(*message, CommandType::Chat);

			if (!discord_chan_data)
				discord_chan_data = ChanManager->FindChannelByIdentifier("Default", CommandType::Chat);

			if (discord_chan_data) {
				const std::string current_date_time = DiscordMetrics::MakeTimeString();
				const uint64 steam_id = ArkApi::GetApiUtils().GetSteamIdFromController(reinterpret_cast<AController*>(player_controller));
				const FString steam_name = ArkApi::GetApiUtils().GetSteamName(reinterpret_cast<AController*>(player_controller));

				FString ranks{};
				TArray<FString> player_ranks = Permissions::GetPlayerGroups(steam_id);

				for (const auto& rank : player_ranks)
					ranks += rank + L", ";

				if (!ranks.IsEmpty())
					ranks.RemoveAt(ranks.Len() - 2, 2);
				
				std::string command_as_std_string = message->ToString();
				DiscordMetrics::EscapeString(command_as_std_string);

				const std::string final_output = fmt::format(chat_cmd_templ, current_date_time, ranks.ToString(),
					steam_name.ToString(), steam_id, command_as_std_string);

				DiscordMetrics::DiscordPacket packet(discord_chan_data->GetTag(), discord_chan_data->GetBotName(), final_output);
				DiscordMetrics::QueuePacket(packet);
			}
		}

		AShooterPlayerController_ServerSendChatMessage_Impl_original(player_controller, message, mode);
	}

	FString* Hook_APlayerController_ConsoleCommand(APlayerController* a_player_controller, FString* result, FString* cmd, bool write_to_log)
	{
		bool should_log = !IgnoreList->IsCommandIgnored(*cmd, CommandType::Console);
		if (a_player_controller->bIsAdmin()() && config_["LogEverything"].get<bool>())
		{
			should_log = true;
		}
		should_log = !IgnoreList->IsCommandBlacklisted(*cmd, CommandType::Console);

		if (should_log)
		{
			std::shared_ptr<DiscordChannelData> discord_chan_data = ChanManager->FindChannelByCommand(*cmd, CommandType::Console);

			if (!discord_chan_data)
				discord_chan_data = ChanManager->FindChannelByIdentifier("Default", CommandType::Console);

			if (discord_chan_data) {
				const std::string current_date_time = DiscordMetrics::MakeTimeString();
				const uint64 steam_id = ArkApi::GetApiUtils().GetSteamIdFromController(reinterpret_cast<AController*>(a_player_controller));
				FString steam_name = ArkApi::GetApiUtils().GetSteamName(reinterpret_cast<AController*>(a_player_controller));

				FString ranks{};
				TArray<FString> player_ranks = Permissions::GetPlayerGroups(steam_id);

				for (const auto& rank : player_ranks)
					ranks += rank + L", ";

				if (!ranks.IsEmpty())
					ranks.RemoveAt(ranks.Len() - 2, 2);

				if (IsStaff(player_ranks))
					steam_name = L"Not a staff member";

				std::string command_as_std_string = cmd->ToString();
				DiscordMetrics::EscapeString(command_as_std_string);

				const std::string final_output = fmt::format(console_cmd_templ, current_date_time, ranks.ToString(),
					steam_name.ToString(), steam_id, command_as_std_string);

				DiscordMetrics::DiscordPacket packet(discord_chan_data->GetTag(), discord_chan_data->GetBotName(), final_output);
				DiscordMetrics::QueuePacket(packet);
			}
		}

		return APlayerController_ConsoleCommand_original(a_player_controller, result, cmd, write_to_log);
	}

	void Hook_RCONClientConnection_ProcessRCONPacket(RCONClientConnection* _this, RCONPacket* packet, UWorld* in_world)
	{
		bool should_log = (!IgnoreList->IsCommandIgnored(packet->Body, CommandType::Rcon) || config_["LogEverything"].get<bool>());
		should_log = !IgnoreList->IsCommandBlacklisted(packet->Body, CommandType::Rcon);

		if (should_log) {
			std::shared_ptr<DiscordChannelData> discord_chan_data = ChanManager->FindChannelByCommand(packet->Body, CommandType::Rcon);

			if (!discord_chan_data)
				discord_chan_data = ChanManager->FindChannelByIdentifier("Default", CommandType::Rcon);

			if (discord_chan_data) {
				const std::string current_date_time = DiscordMetrics::MakeTimeString();

				std::string command_as_std_string = packet->Body.ToString();
				DiscordMetrics::EscapeString(command_as_std_string);

				const std::string final_output = fmt::format(rcon_cmd_templ, current_date_time, command_as_std_string);

				DiscordMetrics::DiscordPacket packet(discord_chan_data->GetTag(), discord_chan_data->GetBotName(), final_output);
				DiscordMetrics::QueuePacket(packet);
			}
		}

		RCONClientConnection_ProcessRCONPacket_original(_this, packet, in_world);
	}

	void SetHooks()
	{
		auto& hooks = ArkApi::GetHooks();

		hooks.SetHook("AShooterPlayerController.ServerSendChatMessage_Implementation", &Hook_AShooterPlayerController_ServerSendChatMessage_Impl,
			&AShooterPlayerController_ServerSendChatMessage_Impl_original);
		hooks.SetHook("APlayerController.ConsoleCommand", &Hook_APlayerController_ConsoleCommand,
			&APlayerController_ConsoleCommand_original);
		hooks.SetHook("RCONClientConnection.ProcessRCONPacket", &Hook_RCONClientConnection_ProcessRCONPacket,
			&RCONClientConnection_ProcessRCONPacket_original);
	}

	void RemoveHooks()
	{
		auto& hooks = ArkApi::GetHooks();

		hooks.DisableHook("AShooterPlayerController.ServerSendChatMessage_Implementation", Hook_AShooterPlayerController_ServerSendChatMessage_Impl);
		hooks.DisableHook("APlayerController.ConsoleCommand", &Hook_APlayerController_ConsoleCommand);
		hooks.DisableHook("RCONClientConnection.ProcessRCONPacket", Hook_RCONClientConnection_ProcessRCONPacket);
	}
}