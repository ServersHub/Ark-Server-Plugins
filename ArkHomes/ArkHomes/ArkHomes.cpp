#include "ArkHomes.h"

#include "HomeSystem.h"
#include "Teleport.h"

#include <fstream>

#pragma comment(lib, "ArkApi.lib")
#pragma comment(lib, "Permissions.lib")
#pragma comment(lib, "ArkShop.lib")

nlohmann::json ArkHome::config;

SQLite::Database& ArkHome::GetDB()
{
	static SQLite::Database db(ArkApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/ArkHomes/ArkHomes.db",
	                           SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
	return db;
}

FString ArkHome::GetText(const std::string& str)
{
	return FString(ArkApi::Tools::Utf8Decode(config["Messages"].value(str, "No message")).c_str());
}

void ReadConfig()
{
	const std::string config_path = ArkApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/ArkHomes/config.json";
	std::ifstream file{config_path};
	if (!file.is_open())
		throw std::runtime_error("Can't open config.json");

	file >> ArkHome::config;

	file.close();
}

void ReloadConfig(APlayerController* player_controller, FString*, bool)
{
	AShooterPlayerController* shooter_controller = static_cast<AShooterPlayerController*>(player_controller);

	try
	{
		ReadConfig();
	}
	catch (const std::exception& error)
	{
		ArkApi::GetApiUtils().SendServerMessage(shooter_controller, FColorList::Red, "Failed to reload config");

		Log::GetLog()->error(error.what());
		return;
	}

	ArkApi::GetApiUtils().SendServerMessage(shooter_controller, FColorList::Green, "Reloaded config");
}

void ReloadConfigRcon(RCONClientConnection* rcon_connection, RCONPacket* rcon_packet, UWorld*)
{
	FString reply;

	try
	{
		ReadConfig();
	}
	catch (const std::exception& error)
	{
		Log::GetLog()->error(error.what());

		reply = error.what();
		rcon_connection->SendMessageW(rcon_packet->Id, 0, &reply);
		return;
	}

	reply = "Reloaded config";
	rcon_connection->SendMessageW(rcon_packet->Id, 0, &reply);
}

void Load()
{
	Log::Get().Init("ArkHomes");

	try
	{
		ReadConfig();
	}
	catch (const std::exception& error)
	{
		Log::GetLog()->error(error.what());
		throw;
	}

	auto& db = ArkHome::GetDB();

	db.exec("create table if not exists Players ("
		"Id integer primary key autoincrement not null,"
		"SteamId integer default 0,"
		"Homes text default '{}',"
		"TpHomeCooldown integer default 0,"
		"AddHomeCooldown integer default 0,"
		"TeleportCooldown integer default 0"
		");");

	HomeSystem::Init();
	Teleport::Init();

	ArkApi::GetCommands().AddConsoleCommand("ArkHome.Reload", &ReloadConfig);
	ArkApi::GetCommands().AddRconCommand("ArkHome.Reload", &ReloadConfigRcon);
}


void Unload()
{
	HomeSystem::RemoveHooks();

	ArkApi::GetCommands().RemoveConsoleCommand("ArkHome.Reload");
	ArkApi::GetCommands().RemoveRconCommand("ArkHome.Reload");

}

BOOL APIENTRY DllMain(HMODULE /*hModule*/, DWORD ul_reason_for_call, LPVOID /*lpReserved*/)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		Load();
		break;
	case DLL_PROCESS_DETACH:
		Unload();
		break;
	}
	return TRUE;
}