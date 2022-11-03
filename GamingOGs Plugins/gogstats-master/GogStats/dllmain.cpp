#include <fstream>
#include <Logger/Logger.h>
#include <codecvt>
#include <tchar.h>

#include "Hooks.h"
#include "Store.h"
#include "Config.h"

#pragma comment(lib, "ArkApi.lib")
#pragma comment(lib, "mysqlclient.lib")

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		Log::Get().Init("Statistics");
		Stats::LoadConfig();
		Stats::Store::Get().Initialize(Stats::GetDBConfig());
		Stats::SetHooks();
		break;
	}
	case DLL_PROCESS_DETACH:
		Stats::UnsetHooks();
		break;
	}
	return TRUE;
}

