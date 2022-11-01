#ifndef _LOOTBOX_DATABASE_H_
#define _LOOTBOX_DATABASE_H_

#pragma warning(push, 0)
	#include <mysql++11/mysql++11.h>
#pragma warning(pop)
#include <string>
#include "Types.h"

namespace LootBox
{
	class Player;
}

namespace LootBox::Database
{
	class Database
	{
	public:
		Database() = default;
		~Database() = default;

		void Initialize(const std::string& host, const std::string& user, const std::string& pass, const std::string& db);
		bool IsPlayerExist(unsigned long long steamId);
		unsigned long long LoadLastTraderId();
		std::shared_ptr<Player> LoadPlayer(unsigned long long steamId);
		std::shared_ptr<Player> LoadPlayerByTraderId(unsigned long long traderId);
		std::shared_ptr<Player> AddPlayer(unsigned long long steamId, unsigned long long traderId, const std::string& characterName, const std::string& steamName);
		void UpdateCharacterName(unsigned long long steamId, std::string name);
		void UpdateLootBoxes(unsigned long long steamId, const Json& lootboxes);
		void UpdateCooldowns(unsigned long long steamId, const Json& cooldowns);

	private:
		daotk::mysql::connection m_conn;
	};
}

#endif // !_LOOTBOX_DATABASE_H_
