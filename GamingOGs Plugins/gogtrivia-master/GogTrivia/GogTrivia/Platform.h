#pragma once

#ifdef ARK
	#include <API/ARK/Ark.h>
	#include <ArkShop/Points.h>
	#pragma comment(lib, "ArkShop.lib")
	#pragma comment(lib, "ArkApi.lib")
	#pragma comment(lib, "Lootboxes.lib")
#else
	#include <AtlasShop/Points.h>
	#include <API/Atlas/Atlas.h>
	#pragma comment(lib, "AtlasShop.lib")
	#pragma comment(lib, "AtlasApi.lib")
	#pragma comment(lib, "GogLootboxAtlas.lib")
#endif

#include <string>
#include <random>
#include <fstream>

#include <json.hpp>
#include <LootBox/GogLootBox.h>

namespace GogTrivia::Platform
{
    inline nlohmann::json conf;

#ifdef ARK
	inline std::string GetConfigPath()
	{
		return ArkApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/Trivia/config.json";
	}
#else
	inline std::string GetConfigPath()
	{
		return ArkApi::Tools::GetCurrentDir() + "/AtlasApi/Plugins/Trivia/config.json";
	}
#endif

	inline void LoadConfig()
	{
		const std::string config_path = Platform::GetConfigPath();
		std::ifstream file{ config_path };

		if (!file.is_open())
		{
			throw std::runtime_error("Can't open config.json");
		}

		file >> conf;
		file.close();
	}

    inline nlohmann::json& Conf()
    {
		if (conf.empty())
			LoadConfig();
        return conf;
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

	inline FString Msg(const std::string& title)
	{
		return FString(ArkApi::Tools::Utf8Decode(conf["Messages"][title].get<std::string>().c_str()));
	}

	inline FString Cmd(const std::string& title)
	{
		return FString(ArkApi::Tools::Utf8Decode(conf["Commands"][title].get<std::string>().c_str()));
	}

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