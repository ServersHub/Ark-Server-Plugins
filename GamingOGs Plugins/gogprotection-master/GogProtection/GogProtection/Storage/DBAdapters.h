#ifndef _GOG_PROT_DB_ADAPTERS_H_
#define _GOG_PROT_DB_ADAPTERS_H_

#include <vector>
#include <memory>
#include "DataTypes.h"
#include "Database.h"

namespace Protection
{
	class PlayerDBAdapter
	{
	private:
		std::vector<std::shared_ptr<PlayerData>>* data_;
		DB::Database* db_;

	public:
		PlayerDBAdapter(std::vector<std::shared_ptr<PlayerData>>* data, DB::Database* db)
			: data_(data), db_(db)
		{
		}

		~PlayerDBAdapter() = default;

		void Load() { db_->LoadPlayers(*data_); }

		void Save() { db_->SavePlayers(*data_); }

		void Clean() { db_->CleanPlayers(); }
	};

	class TeamDBAdapter
	{
	private:
		std::vector<std::shared_ptr<TeamData>>* data_;
		DB::Database* db_;

	public:
		TeamDBAdapter(std::vector<std::shared_ptr<TeamData>>* data, DB::Database* db)
			: data_(data), db_(db)
		{
		}

		~TeamDBAdapter() = default;

		void Load() { db_->LoadTeams(*data_); }

		void Save() { db_->SaveTeams(*data_); }

		void Clean() { db_->CleanTeams(); }
	};
}

#endif // !_GOG_PROT_DB_ADAPTERS_H_
