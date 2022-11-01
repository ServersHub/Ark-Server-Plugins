#pragma once

#include <API/UE/BasicTypes.h>
#include <API/UE/Containers/FString.h>
#include <API/UE/Containers/TArray.h>

namespace AutoDecay::Perms
{
	TArray<FString> GetPlayerGroups(uint64 steam_id);
	bool IsPlayerInGroup(uint64 steam_id, const FString& group_name);
}