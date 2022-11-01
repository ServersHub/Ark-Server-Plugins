#pragma once

#ifdef ARK
	#include <API/ARK/Ark.h>
	#include <ArkShop/Points.h>
	#include <GogProtection/Exports.h>
	#pragma comment(lib, "ArkShop.lib")
	#pragma comment(lib, "ArkApi.lib")
	#pragma comment(lib, "Lootboxes.lib")
	#pragma comment(lib, "PlayerProtection.lib")
#else
	#include <AtlasShop/Points.h>
	#pragma comment(lib, "AtlasShop.lib")
	#include <API/Atlas/Atlas.h>
	#pragma comment(lib, "AtlasApi.lib")
	#pragma comment(lib, "GogLootboxAtlas.lib")
#endif

#include <string>
#include <random>

#include <LootBox/GogLootBox.h>

namespace BountyHunter::Platform
{
#ifdef ARK
	inline std::string GetConfigPath()
	{
		return ArkApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/BountyHunter/config.json";
	}
#else
	inline std::string GetConfigPath()
	{
		return ArkApi::Tools::GetCurrentDir() + "/AtlasApi/Plugins/BountyHunter/config.json";
	}
#endif

	inline bool IsTeamProtected(int id)
	{
#ifdef ARK
		if (ArkApi::Tools::IsPluginLoaded("GogProtection"))
		{
			return Protection::IsTeamProtected(id);
		}
		else
		{
			return false;
		}
#else
		return false;
#endif
	}

#ifdef ARK
	inline void GivePoints(int amount, uint64 steam_id)
	{
		ArkShop::Points::AddPoints(amount, steam_id);
	}
#else
	inline void GivePoints(int amount, uint64 steam_id)
	{
		AtlasShop::Points::AddPoints(amount, steam_id);
	}
#endif

	inline int RandNumber(int min, int max)
	{
		std::default_random_engine generator(std::random_device{}());
		const std::uniform_int_distribution<int> distribution(min, max);

		return distribution(generator);
	}

	inline bool GiveItem(AShooterPlayerController* player_controller, const std::string& blueprint, int amount, float quality,
		bool force_blueprint, unsigned short stat_value)
	{
		FString fblueprint(blueprint);

#ifdef ARK
		TArray<UPrimalItem*> out;
		const bool result = player_controller->GiveItem(&out, &fblueprint, amount, quality, force_blueprint, false, 0);
#else
		const bool result = player_controller->GiveItem(&fblueprint, amount, quality, force_blueprint);
#endif

		return result;
	}
}