#pragma once

#include <json.hpp>

#include "Database.h"
#include "Cache.h"
#include "DBAdapters.h"
#include "PlayerProps.h"
#include "TeamProps.h"
#include "../utils/Config.h"

namespace AutoDecay
{
	class Storage
	{
	private:
		Database db_;
		TCache<Player, unsigned long long, PlayerDBAdapter> players_cache_;
		TCache<Team, int, TeamDBAdapter> teams_cache_;

	public:
		Storage() = default;
		~Storage() = default;

		TCache<Player, unsigned long long, PlayerDBAdapter>* Players()
		{
			return &players_cache_;
		}

		TCache<Team, int, TeamDBAdapter>* Teams()
		{
			return &teams_cache_;
		}

		bool Initialize(const nlohmann::basic_json<>& conf)
		{
			const std::string mysql_host = conf["Mysql"]["MysqlHost"];
			const std::string mysql_user = conf["Mysql"]["MysqlUser"];
			const std::string mysql_pass = conf["Mysql"]["MysqlPass"];
			const std::string mysql_db = conf["Mysql"]["MysqlDB"];

			if (!db_.Connect(mysql_host, mysql_user, mysql_pass, mysql_db))
			{
				return false;
			}

			if (!db_.CheckStructure())
			{
				return false;
			}

			players_cache_.LoadData(&db_);
			teams_cache_.LoadData(&db_);

			return true;
		}

		void Shutdown()
		{
			SaveData();
		}

		void SaveData()
		{
			players_cache_.SaveData(&db_);
			teams_cache_.SaveData(&db_);
		}
	};
}