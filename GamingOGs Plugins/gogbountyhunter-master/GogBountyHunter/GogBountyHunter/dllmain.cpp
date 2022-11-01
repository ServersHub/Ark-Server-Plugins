#include <Logger/Logger.h>
#include <json.hpp>

#include "BountyHunter.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
   switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        Log::Get().Init("BountyHunter");
        BountyHunter::Load();
        return TRUE;
    }
    case DLL_PROCESS_DETACH:
        BountyHunter::Unload();
        break;
    }
    return TRUE;
}
