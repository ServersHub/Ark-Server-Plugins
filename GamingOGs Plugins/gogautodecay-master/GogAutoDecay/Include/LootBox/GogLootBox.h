#ifndef _LOOTBOX_INTERFACE_H_
#define _LOOTBOX_INTERFACE_H_

#include <string>
#include <vector>

namespace GogLootBox
{
	struct LootBoxExternal_PlayerData
	{
		std::string boxName;
		int amount;
	};

	__declspec(dllexport) bool IsEnabled();
	__declspec(dllexport) std::string GiveBoxes(unsigned long long steamId, const std::string& boxName, int amount, const std::string& message, const std::string& sender = "");
	__declspec(dllexport) std::vector<LootBoxExternal_PlayerData> GetPlayerBoxes(unsigned long long steamId);
	__declspec(dllexport) std::string RemoveBoxes(unsigned long long steamId, const std::string& boxName, int amount);
}

#endif // !_LOOTBOX_INTERFACE_H_
