#pragma once

#ifdef ARK
	#include <API/ARK/Ark.h>
	#define CONFIG_PATH ArkApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/Adverts/config.json"
	#pragma comment(lib, "ArkApi.lib")
#else
	#include <API/Atlas/Atlas.h>
	#define CONFIG_PATH ArkApi::Tools::GetCurrentDir() + "/AtlasApi/Plugins/Adverts/config.json"
	#pragma comment(lib, "AtlasApi.lib")
#endif

#include <Tools.h>