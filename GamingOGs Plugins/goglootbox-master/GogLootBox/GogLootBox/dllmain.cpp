#include <Logger/Logger.h>
#include <json.hpp>

#include "App.h"
#include "Hooks.h"

#pragma comment(lib, "ArkApi.lib")
#pragma comment(lib, "ArkShop.lib")
#pragma comment(lib, "Permissions.lib")
#pragma comment(lib, "DiscordMetrics.lib")
#pragma comment(lib, "mysqlclient.lib")

BOOL APIENTRY DllMain(HMODULE /*hModule*/, DWORD ulReasonForCall, LPVOID /*lpReserved*/)
{
	switch (ulReasonForCall)
	{
	case DLL_PROCESS_ATTACH:
	{
		Log::Get().Init("Lootboxes");
		LootBox::App::Get().Initialize();
		LootBox::Hooks::SetHooks();
		return TRUE;
	}
	case DLL_PROCESS_DETACH:
		LootBox::Hooks::UnsetHooks();
		LootBox::App::Get().Shutdown();
		return TRUE;
	}
}
