#include "PermissionsInterface.h"

#ifdef ARK
	#pragma comment(lib, "ArkPermissions.lib")
	#include <Permissions/ArkPermissions.h>
#else
	#pragma comment(lib, "AtlasPermissions.lib")
	#include <Permissions/AtlasPermissions.h>
#endif

namespace AutoDecay::Perms
{
	TArray<FString> GetPlayerGroups(uint64 steam_id)
	{
		return Permissions::GetPlayerGroups(steam_id);
	}
	bool IsPlayerInGroup(uint64 steam_id, const FString& group_name)
	{
		return Permissions::IsPlayerInGroup(steam_id, group_name);
	}
}