#include "ShopInterface.h"

#ifdef ARK
	#pragma comment(lib, "ArkShop.lib")
	#include <ArkShop/Points.h>
#else
	#pragma comment(lib, "AtlasShop.lib")
	#include <AtlasShop/Points.h>
#endif

namespace AutoDecay::Shop
{
#ifdef ARK
	int GetPoints(uint64 steam_id)
	{
		return ArkShop::Points::GetPoints(steam_id);
	}

	bool RemovePoints(uint64 steam_id, int amount)
	{
		return ArkShop::Points::SpendPoints(amount, steam_id);
	}
#else
	int GetPoints(uint64 steam_id)
	{
		return AtlasShop::Points::GetPoints(steam_id);
	}

	bool RemovePoints(uint64 steam_id, int amount)
	{
		return AtlasShop::Points::SpendPoints(amount, steam_id);
	}
#endif
}