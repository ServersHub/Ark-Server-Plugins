#include "GogLootBox.h"
#include "App.h"

namespace GogLootBox
{

	bool IsEnabled()
	{
		auto& app = LootBox::App::Get();
		return app.LootBoxExternal_IsEnabled();
	}

	std::string GiveBoxes(unsigned long long steamId, const std::string& boxName, int amount, const std::string& message, const std::string& sender)
	{
		auto& app = LootBox::App::Get();
		return app.LootBoxExternal_GiveBoxes(steamId, boxName, amount, message, sender);
	}

	std::vector<LootBoxExternal_PlayerData> GetPlayerBoxes(unsigned long long steamId)
	{
		auto& app = LootBox::App::Get();
		const auto boxes = app.LootBoxExternal_GetPlayerBoxes(steamId);
		std::vector<LootBoxExternal_PlayerData> out{};

		for (const auto& item : boxes.items())
		{
			out.push_back(LootBoxExternal_PlayerData{ item.key(), item.value().get<int>() });
		}

		return out;
	}

	std::string RemoveBoxes(unsigned long long steamId, const std::string& boxName, int amount)
	{
		auto& app = LootBox::App::Get();
		return app.LootBoxExternal_RemoveBoxes(steamId, boxName, amount);
	}

}