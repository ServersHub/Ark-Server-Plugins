#include <Logger/Logger.h>
#include <json.hpp>

#include "ChannelManager.h"
#include "Config.h"
#include "Hooks.h"

#pragma comment(lib, "DiscordMetrics.lib")
#pragma comment(lib, "Permissions.lib")

#ifdef ARK
	#pragma comment(lib, "ArkApi.lib")
#endif
#ifdef ATLAS
	#pragma comment(lib, "AtlasApi.lib")
#endif

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
	{
		Log::Get().Init("CommandLogger");
		if (!CommandLogger::ParseConfig())
		{
			throw std::runtime_error("Can't parse configuration");
		}
		if (!CommandLogger::Initialize())
		{
			throw std::runtime_error("Can't initialize manager");
		}
		CommandLogger::SetHooks();
		return TRUE;
	}
    case DLL_PROCESS_DETACH:
		CommandLogger::RemoveHooks();
        break;
    }
    return TRUE;
}
