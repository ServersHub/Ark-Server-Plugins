#define CPPHTTPLIB_OPENSSL_SUPPORT
#define WIN32_LEAN_AND_MEAN
#include <Logger/Logger.h>
#include <json.hpp>

#include "ApiProxy.h"
#include "Commands.h"
#include "Hooks.h"
#include "AdvertManager.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        Log::Get().Init("Adverts");
        GogAdvert::AdvertManager::Get().Initialize();
		GogAdvert::Commands::Initialize();
		GogAdvert::Hooks::Initialize();
        break;
    }
    case DLL_PROCESS_DETACH:
		GogAdvert::Commands::Shutdown();
		GogAdvert::Hooks::Shutdown();
		break;
    }
    return TRUE;
}

