#include "Database.h"
#include <Logger/Logger.h>
#include "Player.h"

namespace LootBox::Database
{

	void Database::Initialize(const std::string& host, const std::string& user, const std::string& pass, const std::string& db)
	{
		daotk::mysql::connect_options options{};
		options.server = host;
		options.username = user;
		options.password = pass;
		options.dbname = db;
		options.timeout = 30;
		options.autoreconnect = true;

		if (!m_conn.open(options))
		{
			throw std::runtime_error("Can't connect to database");
		}

		bool result = m_conn.query(
			"CREATE TABLE IF NOT EXISTS `lootbox` ("
			"`steam_id` BIGINT(11) NOT NULL,"
			"`trader_id` BIGINT(11) NOT NULL,"
			"`character_name` VARCHAR(256) NOT NULL,"
			"`steam_name` VARCHAR(128) NOT NULL,"
			"`lootboxes` VARCHAR(512) NOT NULL DEFAULT '{}',"
			"`cooldowns` VARCHAR(512) NOT NULL DEFAULT '{}',"
			"UNIQUE INDEX `steam_id_UNIQUE` (`steam_id` ASC));"
		);

		if (!result)
		{
			throw std::runtime_error("Can't create MySQL table");
		}
	}

	bool Database::IsPlayerExist(unsigned long long steamId)
	{
		try
		{
			const auto result = m_conn.query("SELECT count(1) FROM lootbox WHERE steam_id = %I64u;", steamId).get_value<int>();
			return result > 0;
		}
		catch (const std::exception& exception)
		{
			Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
			return false;
		}
	}

	unsigned long long Database::LoadLastTraderId()
	{
		try
		{
			if (m_conn.query("SELECT count(1) FROM lootbox;").get_value<int>() == 0)
			{
				return 0;
			}
			else
			{
				return m_conn.query("SELECT MAX(trader_id) FROM lootbox;").get_value<int>();
			}
		}
		catch (const std::exception& exception)
		{
			Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
			return 0;
		}
	}

	std::shared_ptr<Player> Database::LoadPlayer(unsigned long long steamId)
	{
		try
		{
			const std::string query = fmt::format("SELECT trader_id, character_name, steam_name, lootboxes, cooldowns FROM lootbox WHERE steam_id = {};", steamId);
			auto res = m_conn.query(query);
			if (res.count() > 0)
			{
				unsigned long long traderId;
				std::string characterName;
				std::string steamName;
				std::string lootboxes;
				std::string cooldowns;
				
				res.fetch(traderId, characterName, steamName, lootboxes, cooldowns);
				return std::make_shared<Player>(
					this,
					steamId,
					traderId,
					characterName,
					steamName,
					Json::parse(lootboxes),
					Json::parse(cooldowns)
					);
			}
			return nullptr;
		}
		catch (const std::exception& exception)
		{
			Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
			return nullptr;
		}
	}

	std::shared_ptr<Player> Database::LoadPlayerByTraderId(unsigned long long traderId)
	{
		try
		{
			const std::string query = fmt::format("SELECT steam_id, character_name, steam_name, lootboxes, cooldowns FROM lootbox WHERE trader_id = {};", traderId);
			auto res = m_conn.query(query);
			if (res)
			{
				unsigned long long steamId;
				std::string characterName;
				std::string steamName;
				std::string lootboxes;
				std::string cooldowns;

				res.fetch(steamId, characterName, steamName, lootboxes, cooldowns);
				return std::make_shared<Player>(
					this,
					steamId,
					traderId,
					characterName,
					steamName,
					Json::parse(lootboxes),
					Json::parse(cooldowns)
					);
			}
			return nullptr;
		}
		catch (const std::exception& exception)
		{
			Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
			return nullptr;
		}
	}

	std::shared_ptr<Player> Database::AddPlayer(unsigned long long steamId, unsigned long long traderId, const std::string& characterName, const std::string& steamName)
	{
		try
		{
			m_conn.query(fmt::format("INSERT INTO lootbox (steam_id, trader_id, character_name, steam_name) VALUES ({}, {}, '{}', '{}');",
				steamId, traderId, characterName, steamName));
			auto newPlayer = std::make_shared<Player>(
				this,
				steamId,
				traderId,
				characterName,
				steamName,
				Json::object(),
				Json::object()
				);
			return newPlayer;
		}
		catch (const std::exception& exception)
		{
			Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
			return nullptr;
		}
	}

	void Database::UpdateCharacterName(unsigned long long steamId, std::string name)
	{
		try
		{
			m_conn.query(fmt::format("UPDATE lootbox SET character_name = '{}' WHERE steam_id = {};", std::move(name), steamId));
		}
		catch (const std::exception& exception)
		{
			Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
		}
	}

	void Database::UpdateLootBoxes(unsigned long long steamId, const Json& lootboxes)
	{
		try
		{
			m_conn.query(fmt::format("UPDATE lootbox SET lootboxes = '{}' WHERE steam_id = {};", lootboxes.dump(), steamId));
		}
		catch (const std::exception& exception)
		{
			Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
		}
	}

	void Database::UpdateCooldowns(unsigned long long steamId, const Json& cooldowns)
	{
		try
		{
			m_conn.query(fmt::format("UPDATE lootbox SET cooldowns = '{}' WHERE steam_id = {};", cooldowns.dump(), steamId));
		}
		catch (const std::exception& exception)
		{
			Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
		}
	}

}