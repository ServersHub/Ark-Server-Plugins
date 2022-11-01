#include <Logger/Logger.h>
#include <json.hpp>

#include "utils/Config.h"
#include "auto_decay/Commands.h"
#include "auto_decay/Hooks.h"
#include "auto_decay/Application.h"

#pragma comment(lib, "mysqlclient.lib")
#pragma comment(lib, "DiscordMetrics.lib")

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
	{
		Log::Get().Init("AutoDecay");
		AutoDecay::Application::Initialize();
		AutoDecay::Commands::Initialize();
		AutoDecay::Hooks::Initialize();
		return TRUE;
	}
    case DLL_PROCESS_DETACH:
		AutoDecay::Commands::Deinitialize();
		AutoDecay::Hooks::Deinitialize();
		AutoDecay::Application::Shutdown();
        break;
    }

    return TRUE;
}
