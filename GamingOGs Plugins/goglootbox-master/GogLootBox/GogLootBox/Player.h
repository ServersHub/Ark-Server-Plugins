#ifndef _LOOTBOX_PLAYER_H_
#define _LOOTBOX_PLAYER_H_

#include <API/UE/BasicTypes.h>
#include <string>
#include <vector>
#include <algorithm>
#include "Types.h"

namespace LootBox
{
	namespace Database
	{
		class Database;
	}

	class Player
	{
	public:
		Player(Database::Database* db, uint64 steamId, uint64 traderId, std::string characterName, std::string steamName, Json lootboxes, Json cooldowns)
			: m_pDB(db), m_steamId(steamId), m_traderId(traderId), m_characterName(std::move(characterName)), m_steamName(std::move(steamName)), m_lootboxes(std::move(lootboxes)), m_cooldowns(std::move(cooldowns))
		{
		}

		[[nodiscard]] uint64 SteamId() const noexcept { return m_steamId; }
		[[nodiscard]] uint64 TraderId() const noexcept { return m_traderId; }
		[[nodiscard]] const std::string& CharacterName() const noexcept { return m_characterName; }
		[[nodiscard]] const std::string& SteamName() const noexcept { return m_steamName; }
		[[nodiscard]] const Json& InventoryBoxes() const noexcept { return m_lootboxes; }
		[[nodiscard]] const Json& BoxesCooldowns() const noexcept { return m_cooldowns; }

		void Rename(std::string newName);
		[[nodiscard]] bool HasBoxes(const std::string& boxName, int amount) const;
		void AddBoxes(std::string boxName, int amount);
		void SetBoxes(const std::string& boxName, int amount);
		void RemoveBoxes(const std::string& boxName, int amount);
		[[nodiscard]] bool HasCooldown(const std::string& boxName) const;
		void SetCooldown(const std::string& boxName, int hitsAmount);
		void DecrementCooldowns();

	private:
		Database::Database* m_pDB;
		uint64 m_steamId;
		uint64 m_traderId;
		std::string m_characterName;
		std::string m_steamName;
		Json m_lootboxes;
		Json m_cooldowns;

	};

}

#endif // !_LOOTBOX_PLAYER_H_
