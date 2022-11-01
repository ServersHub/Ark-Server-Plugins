#include "Commands.h"

#ifdef ARK
	#include <API/ARK/Ark.h>
#endif
#ifdef ATLAS
	#include <API/Atlas/Atlas.h>
#endif

#include <fstream>
#include "Config.h"
#include "ChannelManager.h"


namespace CommandLogger
{
	FString ReloadImpl()
	{
#ifdef ARK
		const std::string config_path = ArkApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/GogCommandLogger/config.json";
#else
		const std::string config_path = ArkApi::Tools::GetCurrentDir() + "/AtlasApi/Plugins/GogCommandLogger/config.json";
#endif

		std::fstream file(config_path);

		if (!file.is_open())
		{
			return FString::Format("Can't open file '{}'", config_path);
		}

		nlohmann::json config_tmp{};

		try
		{
			file >> config_tmp;
		}
		catch (const std::exception& e)
		{
			file.close();
			return FString::Format("JSON parsing error '{}'", e.what());
		}

		file.close();

		config_ = std::move(config_tmp);
		if (!Initialize())
		{
			return L"Can't initialize Logger";
		}

		return L"Success";
	}

	void ReloadAdmin(APlayerController* controller, FString* cmd, bool /*unused*/)
	{
		const FString reply = ReloadImpl();
		ArkApi::GetApiUtils().SendChatMessage(static_cast<AShooterPlayerController*>(controller), "CommandLogger", *reply);
	}

	void ReloadRCON(RCONClientConnection* rcon_connection, RCONPacket* rcon_packet, UWorld* /*unused*/)
	{
		FString reply = ReloadImpl();
		rcon_connection->SendMessageW(rcon_packet->Id, 0, &reply);
	}


	void BindCommands()
	{
		ArkApi::GetCommands().AddConsoleCommand(L"cl.reload", &ReloadAdmin);
		ArkApi::GetCommands().AddRconCommand(L"cl.reload", &ReloadRCON);
	}

	void UnbindCommands()
	{
		ArkApi::GetCommands().RemoveConsoleCommand(L"cl.reload");
		ArkApi::GetCommands().RemoveRconCommand(L"cl.reload");
	}

}