#ifndef _GOG_PROT_STORAGE_H_
#define _GOG_PROT_STORAGE_H_

#include <memory>
#include <vector>

#include "DataTypes.h"
#include "TCache.h"
#include "DBAdapters.h"
#include "Database.h"
#include "../Common/Config.h"

namespace Protection
{
	class Storage
	{
	private:
		DB::Database db_;
		TCache<PlayerData, unsigned long long, PlayerDBAdapter> players_cache_;
		TCache<TeamData, int, TeamDBAdapter> teams_cache_;

	public:
		Storage() = default;
		~Storage() = default;

		TCache<PlayerData, unsigned long long, PlayerDBAdapter>* Players()
		{
			return &players_cache_;
		}

		TCache<TeamData, int, TeamDBAdapter>* Teams()
		{
			return &teams_cache_;
		}

		bool Initialize()
		{
			const std::string mysql_host = GetConfig()["Mysql"]["MysqlHost"];
			const std::string mysql_user = GetConfig()["Mysql"]["MysqlUser"];
			const std::string mysql_pass = GetConfig()["Mysql"]["MysqlPass"];
			const std::string mysql_db = GetConfig()["Mysql"]["MysqlDB"];

			if (!db_.Connect(mysql_host, mysql_user, mysql_pass, mysql_db))
				return false;

			if (!db_.CheckStructure())
				return false;

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

	using PlayerItemType = std::shared_ptr<PlayerData>;
	using TeamItemType = std::shared_ptr<TeamData>;
}

#endif // !_GOG_PROT_STORAGE_H_