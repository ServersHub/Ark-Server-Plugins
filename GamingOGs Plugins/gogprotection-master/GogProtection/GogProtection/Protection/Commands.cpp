#include "Commands.h"

#include "../Common/Config.h"
#include "../Common/Helpers.h"
#include "Application.h"
#include "CommandDelegates.h"

#include "../Storage/Storage.h"
#include "../Modules/ProtManager.h"

#include <API/ARK/Ark.h>

namespace Protection
{
    namespace Commands
    {
		inline const float NOTIFICATION_SIZE = 1.5f;
		inline const float NOTIFICATION_TIME = 10.0f;
		inline const FColor SUCCESS_NOTIFICATION_COLOR = FColorList::Green;
		inline const FColor FAIL_NOTIFICATION_COLOR = FColorList::Red;

		inline void SendChatResponse(AShooterPlayerController* shooter_controller, CommandExecResultPtr&& result)
		{
			if (result->GetResult())
			{
				ArkApi::GetApiUtils().SendNotification(shooter_controller, SUCCESS_NOTIFICATION_COLOR,
					NOTIFICATION_SIZE, NOTIFICATION_TIME, nullptr, *(result->GetOutput()));
			}
			else
			{
				ArkApi::GetApiUtils().SendNotification(shooter_controller, FAIL_NOTIFICATION_COLOR,
					NOTIFICATION_SIZE, NOTIFICATION_TIME, nullptr, *(result->GetOutput()));
			}
		}

		inline void SendAdminResponse(APlayerController* player_controller, CommandExecResultPtr&& result)
		{
			if (result->GetResult())
			{
				ArkApi::GetApiUtils().SendServerMessage(static_cast<AShooterPlayerController*>(player_controller),
					SUCCESS_NOTIFICATION_COLOR, *(result->GetOutput()));
			}
			else
			{
				ArkApi::GetApiUtils().SendServerMessage(static_cast<AShooterPlayerController*>(player_controller),
					FAIL_NOTIFICATION_COLOR, *(result->GetOutput()));
			}
		}

		inline void SendRCONResponse(RCONClientConnection* conn, RCONPacket* packet, CommandExecResultPtr&& result)
		{
			FString res = result->GetOutput() + "\n";
			conn->SendMessageW(packet->Id, 0, &res);
		}

		void CliInfoCmd(AShooterPlayerController* shooter_controller, FString* message, int mode)
		{
			CliInfoCommandDelegate executor(App.get());
			CommandExecResultPtr result = executor.Exec();

			SendChatResponse(shooter_controller, std::move(result));
		}

		void CliStatusCmd(AShooterPlayerController* shooter_controller, FString* message, int mode)
		{
			const int team_id = shooter_controller->TargetingTeamField();

			CliStatusCommandDelegate executor(App.get(), team_id);
			CommandExecResultPtr result = executor.Exec();

			SendChatResponse(shooter_controller, std::move(result));
		}

		void CliDisableCmd(AShooterPlayerController* shooter_controller, FString* message, int mode)
		{
			const int team_id = shooter_controller->TargetingTeamField();
			bool is_team_admin = true;

			FTribeData* tribe_data = static_cast<AShooterPlayerState*>(shooter_controller->PlayerStateField())->MyTribeDataField();
			if (tribe_data && tribe_data->TribeIDField() > 0) {
				if(!(static_cast<AShooterPlayerState*>(shooter_controller->PlayerStateField())->IsTribeAdmin()))
					is_team_admin = false;
			}

			CliDisableCommandDelegate executor(App.get(), team_id, is_team_admin);
			CommandExecResultPtr result = executor.Exec();

			SendChatResponse(shooter_controller, std::move(result));
		}

		void CliSetZoneCmd(AShooterPlayerController* shooter_controller, FString* message, int mode)
		{
			const std::string zone_id = Helpers::ExtractZoneID(*message, 1);
			const int team_id = shooter_controller->TargetingTeamField();
			const FVector position = shooter_controller->DefaultActorLocationField();

			CliSetZoneCommandDelegate executor(App.get(), team_id, zone_id, position);
			CommandExecResultPtr result = executor.Exec();

			SendChatResponse(shooter_controller, std::move(result));
		}

		void CliUnsetZoneCmd(AShooterPlayerController* shooter_controller, FString* message, int mode)
		{
			const std::string zone_id = Helpers::ExtractZoneID(*message, 1);
			const int team_id = shooter_controller->TargetingTeamField();

			CliUnsetZoneCommandDelegate executor(App.get(), team_id, zone_id);
			CommandExecResultPtr result = executor.Exec();

			SendChatResponse(shooter_controller, std::move(result));
		}

		void CliDisplayZonesCmd(AShooterPlayerController* shooter_controller, FString* message, int mode)
		{
			const int team_id = shooter_controller->TargetingTeamField();
			FVector player_position = shooter_controller->DefaultActorLocationField();

			CliDisplayZonesCommandDelegate executor(App.get(), team_id, player_position);
			CommandExecResultPtr result = executor.Exec();

			SendChatResponse(shooter_controller, std::move(result));
		}

		void AdmDisableCmd(APlayerController* player_controller, FString* message, bool write_to_log)
		{
			const auto steam_id = Helpers::ValidateSteamID(*message, 1);

			DisableCommandDelegate executor(App.get(), steam_id);
			CommandExecResultPtr result = executor.Exec();

			SendAdminResponse(player_controller, std::move(result));
		}

		void AdmRestoreCmd(APlayerController* player_controller, FString* message, bool write_to_log)
		{
			const auto steam_id = Helpers::ValidateSteamID(*message, 1);

			RestoreCommandDelegate executor(App.get(), steam_id);
			CommandExecResultPtr result = executor.Exec();

			SendAdminResponse(player_controller, std::move(result));
		}

		void AdmAddAPCdm(APlayerController* player_controller, FString* message, bool writeLog)
		{
			uint64 steam_id = 0;
			int add_time_secs = 0;
			TArray<FString> parsed;

			message->ParseIntoArray(parsed, L" ");

			if (parsed.IsValidIndex(1))
				steam_id = std::strtoull(parsed[1].ToString().c_str(), nullptr, 10);

			if (parsed.IsValidIndex(2))
				add_time_secs = std::strtol(parsed[2].ToString().c_str(), nullptr, 10) * 60 * 60;

			AddProtectionCommandDelegate executor(App.get(), steam_id, add_time_secs);
			CommandExecResultPtr result = executor.Exec();

			if (!result->GetResult())
				Log::GetLog()->error("AddProtection command execution failed: {}", result->GetOutput().ToString());
		}

		void AdmSetAPCdm(APlayerController* player_controller, FString* message, bool writeLog)
		{
			uint64 steam_id = Helpers::ValidateSteamID(*message, 1);

			TArray<FString> parsed;
			message->ParseIntoArray(parsed, L" ");

			SetProtectionCommandDelegate executor(App.get(), steam_id, message->ToString());
			CommandExecResultPtr result = executor.Exec();

			SendAdminResponse(player_controller, std::move(result));
		}

		void AdmBlackListAddCmd(APlayerController* player_controller, FString* message, bool write_log)
		{
			int team_id = Helpers::ValidateTeamID(*message, 1);

			if (team_id == 0)
				team_id = Helpers::GetAimedActorTeam(player_controller);

			BlackListAddCommandDelegate executor(App.get(), team_id);
			CommandExecResultPtr result = executor.Exec();

			SendAdminResponse(player_controller, std::move(result));
		}

		void AdmBlackListRemoveCmd(APlayerController* player_controller, FString* message, bool write_log)
		{
			int team_id = Helpers::ValidateTeamID(*message, 1);

			if (team_id == 0)
				team_id = Helpers::GetAimedActorTeam(player_controller);

			BlackListRemoveCommandDelegate executor(App.get(), team_id);
			CommandExecResultPtr result = executor.Exec();

			SendAdminResponse(player_controller, std::move(result));
		}

		void AdmEnableORPCmd(APlayerController* player_controller, FString* message, bool write_log)
		{
			EnableORPCommandDelegate executor(App.get());
			CommandExecResultPtr result = executor.Exec();

			SendAdminResponse(player_controller, std::move(result));
		}

		void AdmDisableORPCmd(APlayerController* player_controller, FString* message, bool write_log)
		{
			DisableORPCommandDelegate executor(App.get(), message->ToString());
			CommandExecResultPtr result = executor.Exec();

			SendAdminResponse(player_controller, std::move(result));
		}

		void AdmReloadCmd(APlayerController* player_controller, FString* message, bool write_to_log)
		{
			ReloadCommandDelegate executor(App.get());
			CommandExecResultPtr result = executor.Exec();

			SendAdminResponse(player_controller, std::move(result));
		}

		void RCONDisableCmd(RCONClientConnection* rcon_connection, RCONPacket* rcon_packet, UWorld*)
		{
			const auto steam_id = Helpers::ValidateSteamID(rcon_packet->Body, 1);

			DisableCommandDelegate executor(App.get(), steam_id);
			CommandExecResultPtr result = executor.Exec();

			SendRCONResponse(rcon_connection, rcon_packet, std::move(result));
		}

		void RCONRepairCmd(RCONClientConnection* rcon_connection, RCONPacket* rcon_packet, UWorld*)
		{
			const auto steam_id = Helpers::ValidateSteamID(rcon_packet->Body, 1);

			RestoreCommandDelegate executor(App.get(), steam_id);
			CommandExecResultPtr result = executor.Exec();

			SendRCONResponse(rcon_connection, rcon_packet, std::move(result));
		}

		void RCONSetProtectionCmd(RCONClientConnection* rcon_connection, RCONPacket* rcon_packet, UWorld*)
		{
			const auto steam_id = Helpers::ValidateSteamID(rcon_packet->Body, 1);

			SetProtectionCommandDelegate executor(App.get(), steam_id, rcon_packet->Body.ToString());
			CommandExecResultPtr result = executor.Exec();

			SendRCONResponse(rcon_connection, rcon_packet, std::move(result));
		}

		void RCONBlackListAddCmd(RCONClientConnection* rcon_connection, RCONPacket* rcon_packet, UWorld*)
		{
			int team_id = Helpers::ValidateTeamID(rcon_packet->Body, 1);

			BlackListAddCommandDelegate executor(App.get(), team_id);
			CommandExecResultPtr result = executor.Exec();

			SendRCONResponse(rcon_connection, rcon_packet, std::move(result));
		}

		void RCONBlackListRemoveCmd(RCONClientConnection* rcon_connection, RCONPacket* rcon_packet, UWorld*)
		{
			int team_id = Helpers::ValidateTeamID(rcon_packet->Body, 1);

			BlackListRemoveCommandDelegate executor(App.get(), team_id);
			CommandExecResultPtr result = executor.Exec();

			SendRCONResponse(rcon_connection, rcon_packet, std::move(result));
		}

		void RCONEnableORPCmd(RCONClientConnection* rcon_connection, RCONPacket* rcon_packet, UWorld*)
		{
			EnableORPCommandDelegate executor(App.get());
			CommandExecResultPtr result = executor.Exec();

			SendRCONResponse(rcon_connection, rcon_packet, std::move(result));
		}

		void RCONDisableORPCmd(RCONClientConnection* rcon_connection, RCONPacket* rcon_packet, UWorld*)
		{
			DisableORPCommandDelegate executor(App.get(), rcon_packet->Body.ToString());
			CommandExecResultPtr result = executor.Exec();

			SendRCONResponse(rcon_connection, rcon_packet, std::move(result));
		}

		void RCONReloadCmd(RCONClientConnection* rcon_connection, RCONPacket* rcon_packet, UWorld*)
		{
			ReloadCommandDelegate executor(App.get());
			CommandExecResultPtr result = executor.Exec();

			SendRCONResponse(rcon_connection, rcon_packet, std::move(result));
		}

		void RegisterCommands()
		{
			auto& commands = ArkApi::GetCommands();

			commands.AddChatCommand(FString(std::string(GetConfig()["AP"]["Commands"]["ChatInfo"])), &CliInfoCmd);
			commands.AddChatCommand(FString(std::string(GetConfig()["AP"]["Commands"]["ChatStatus"])), &CliStatusCmd);
			commands.AddChatCommand(FString(std::string(GetConfig()["AP"]["Commands"]["ChatDisable"])), &CliDisableCmd);
			commands.AddChatCommand(FString(std::string(GetConfig()["AP"]["Commands"]["ChatSetZone"])), &CliSetZoneCmd);
			commands.AddChatCommand(FString(std::string(GetConfig()["AP"]["Commands"]["ChatUnsetZone"])), &CliUnsetZoneCmd);
			commands.AddChatCommand(FString(std::string(GetConfig()["AP"]["Commands"]["ChatDisplayZones"])), &CliDisplayZonesCmd);

			commands.AddConsoleCommand(FString(std::string(GetConfig()["AP"]["Commands"]["RemoveProtection"])), &AdmDisableCmd);
			commands.AddConsoleCommand(FString(std::string(GetConfig()["AP"]["Commands"]["ResetProtection"])), &AdmRestoreCmd);
			commands.AddConsoleCommand(FString(std::string(GetConfig()["AP"]["Commands"]["AddProtection"])), &AdmAddAPCdm);
			commands.AddConsoleCommand(FString(std::string(GetConfig()["AP"]["Commands"]["SetProtection"])), &AdmSetAPCdm);
			commands.AddConsoleCommand(FString(std::string(GetConfig()["AP"]["Commands"]["AddToBlacklist"])), &AdmBlackListAddCmd);
			commands.AddConsoleCommand(FString(std::string(GetConfig()["AP"]["Commands"]["RemoveFromBlacklist"])), &AdmBlackListRemoveCmd);
			commands.AddConsoleCommand(FString(std::string(GetConfig()["AP"]["Commands"]["EnableORP"])), &AdmEnableORPCmd);
			commands.AddConsoleCommand(FString(std::string(GetConfig()["AP"]["Commands"]["DisableORP"])), &AdmDisableORPCmd);
			commands.AddConsoleCommand(FString(std::string(GetConfig()["AP"]["Commands"]["Reload"])), &AdmReloadCmd);

			commands.AddRconCommand(FString(std::string(GetConfig()["AP"]["Commands"]["RemoveProtection"])), &RCONDisableCmd);
			commands.AddRconCommand(FString(std::string(GetConfig()["AP"]["Commands"]["ResetProtection"])), &RCONRepairCmd);
			commands.AddRconCommand(FString(std::string(GetConfig()["AP"]["Commands"]["SetProtection"])), &RCONSetProtectionCmd);
			commands.AddRconCommand(FString(std::string(GetConfig()["AP"]["Commands"]["AddToBlacklist"])), &RCONBlackListAddCmd);
			commands.AddRconCommand(FString(std::string(GetConfig()["AP"]["Commands"]["RemoveFromBlacklist"])), &RCONBlackListRemoveCmd);
			commands.AddRconCommand(FString(std::string(GetConfig()["AP"]["Commands"]["EnableORP"])), &RCONEnableORPCmd);
			commands.AddRconCommand(FString(std::string(GetConfig()["AP"]["Commands"]["DisableORP"])), &RCONDisableORPCmd);
			commands.AddRconCommand(FString(std::string(GetConfig()["AP"]["Commands"]["Reload"])), &RCONReloadCmd);
		}

		void UnregisterCommands()
		{
			auto& commands = ArkApi::GetCommands();

			commands.RemoveChatCommand(FString(std::string(GetConfig()["AP"]["Commands"]["ChatInfo"])));
			commands.RemoveChatCommand(FString(std::string(GetConfig()["AP"]["Commands"]["ChatStatus"])));
			commands.RemoveChatCommand(FString(std::string(GetConfig()["AP"]["Commands"]["ChatDisable"])));
			commands.RemoveChatCommand(FString(std::string(GetConfig()["AP"]["Commands"]["ChatSetZone"])));
			commands.RemoveChatCommand(FString(std::string(GetConfig()["AP"]["Commands"]["ChatUnsetZone"])));
			commands.RemoveChatCommand(FString(std::string(GetConfig()["AP"]["Commands"]["ChatDisplayZones"])));

			commands.RemoveConsoleCommand(FString(std::string(GetConfig()["AP"]["Commands"]["RemoveProtection"])));
			commands.RemoveConsoleCommand(FString(std::string(GetConfig()["AP"]["Commands"]["ResetProtection"])));
			commands.RemoveConsoleCommand(FString(std::string(GetConfig()["AP"]["Commands"]["AddProtection"])));
			commands.RemoveConsoleCommand(FString(std::string(GetConfig()["AP"]["Commands"]["SetProtection"])));
			commands.RemoveConsoleCommand(FString(std::string(GetConfig()["AP"]["Commands"]["AddToBlacklist"])));
			commands.RemoveConsoleCommand(FString(std::string(GetConfig()["AP"]["Commands"]["RemoveFromBlacklist"])));
			commands.RemoveConsoleCommand(FString(std::string(GetConfig()["AP"]["Commands"]["EnableORP"])));
			commands.RemoveConsoleCommand(FString(std::string(GetConfig()["AP"]["Commands"]["DisableORP"])));
			commands.RemoveConsoleCommand(FString(std::string(GetConfig()["AP"]["Commands"]["Reload"])));

			commands.RemoveRconCommand(FString(std::string(GetConfig()["AP"]["Commands"]["RemoveProtection"])));
			commands.RemoveRconCommand(FString(std::string(GetConfig()["AP"]["Commands"]["ResetProtection"])));
			commands.RemoveRconCommand(FString(std::string(GetConfig()["AP"]["Commands"]["SetProtection"])));
			commands.RemoveRconCommand(FString(std::string(GetConfig()["AP"]["Commands"]["AddToBlacklist"])));
			commands.RemoveRconCommand(FString(std::string(GetConfig()["AP"]["Commands"]["RemoveFromBlacklist"])));
			commands.RemoveRconCommand(FString(std::string(GetConfig()["AP"]["Commands"]["EnableORP"])));
			commands.RemoveRconCommand(FString(std::string(GetConfig()["AP"]["Commands"]["DisableORP"])));
			commands.RemoveRconCommand(FString(std::string(GetConfig()["AP"]["Commands"]["Reload"])));
		}
    }
}