#pragma once

#include <API/UE/BasicTypes.h>

namespace AutoDecay::Shop
{
	int GetPoints(uint64 steam_id);
	bool RemovePoints(uint64 steam_id, int amount);
}