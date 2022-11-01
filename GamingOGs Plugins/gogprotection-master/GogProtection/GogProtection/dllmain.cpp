#include <API/ARK/Ark.h>
#include <json.hpp>

#include "Protection/Application.h"
#include "Protection/Commands.h"
#include "Protection/Hooks.h"

#pragma comment(lib, "ArkApi.lib")
#pragma comment(lib, "Permissions.lib")
#pragma comment(lib, "DiscordMetrics.lib")
#pragma comment(lib, "mysqlclient.lib")

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		{
			Log::Get().Init("PlayerProtection");
			Protection::App = std::make_unique<Protection::Application>();

			if (!Protection::App->Initialize())
				throw std::runtime_error("Error initialization Application");

			Protection::Commands::RegisterCommands();
			Protection::Hooks::RegisterHooks();
			return TRUE;
		}
	case DLL_PROCESS_DETACH:
		Protection::Commands::UnregisterCommands();
		Protection::Hooks::UnregisterHooks();

		if (Protection::App)
			Protection::App->Shutdown();
		return TRUE;
	}
}