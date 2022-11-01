#include <Windows.h>

#include <Logger/Logger.h>
#include <json.hpp>

#include "Trivia.h"
#include "Platform.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        Log::Get().Init("Trivia");
        if (!GogTrivia::Load())
        {
            throw std::runtime_error("Can't initialize plugin");
        }
        break;
    }
    case DLL_PROCESS_DETACH:
        GogTrivia::Unload();
        break;
    }
    return TRUE;
}

