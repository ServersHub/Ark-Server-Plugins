#pragma once

#ifdef ARK
	#include <API/ARK/Ark.h>
	#pragma comment(lib, "ArkApi.lib")
#else
	#include <API/Atlas/Atlas.h>
	#pragma comment(lib, "AtlasApi.lib")
#endif