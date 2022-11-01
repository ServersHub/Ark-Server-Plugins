#include "Commands.h"

#include "ApiProxy.h"
#include "AdvertManager.h"

namespace GogAdvert::Commands
{
	void AdmReload(APlayerController* player_controller, FString* message, bool write_to_log)
	{
		const auto result = AdvertManager::Get().Reload();

		if (result)
		{
			ArkApi::GetApiUtils().SendServerMessage(reinterpret_cast<AShooterPlayerController*>(player_controller),
				FColorList::Green, L"GogAdvert was reloaded");
		}
		else
		{
			ArkApi::GetApiUtils().SendServerMessage(reinterpret_cast<AShooterPlayerController*>(player_controller),
				FColorList::Red, L"GogAdvert wasn't reloaded");
		}
	}

	void RconReload(RCONClientConnection* rcon_connection, RCONPacket* rcon_packet, UWorld*)
	{
		const auto result = AdvertManager::Get().Reload();

		FString reply{};
		if (result)
		{
			reply = "GogAdvert was reloaded\n";
		}
		else
		{
			reply = "GogAdvert wasn't reloaded\n";
		}

		rcon_connection->SendMessageW(rcon_packet->Id, 0, &reply);
	}

	void Initialize()
	{
		auto& commands = ArkApi::GetCommands();

		commands.AddConsoleCommand(L"advertreload", &AdmReload);
		commands.AddRconCommand(L"advertreload", &RconReload);
	}

	void Shutdown()
	{
		auto& commands = ArkApi::GetCommands();

		commands.RemoveConsoleCommand(L"advertreload");
		commands.RemoveRconCommand(L"advertreload");
	}
}