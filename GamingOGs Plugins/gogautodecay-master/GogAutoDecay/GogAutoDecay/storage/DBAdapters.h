#pragma once

#include "PlayerProps.h"
#include "TeamProps.h"
#include "Database.h"

namespace AutoDecay
{
	class PlayerDBAdapter
	{
	private:
		std::vector<Player>* data_;
		Database* db_;

	public:
		PlayerDBAdapter(std::vector<Player>* data, Database* db)
			: data_(data), db_(db)
		{
		}

		~PlayerDBAdapter() = default;

		void Load() { db_->LoadPlayers(*data_); }

		void Save() { db_->SavePlayers(*data_); }
	};

	class TeamDBAdapter
	{
	private:
		std::vector<Team>* data_;
		Database* db_;

	public:
		TeamDBAdapter(std::vector<Team>* data, Database* db)
			: data_(data), db_(db)
		{
		}

		~TeamDBAdapter() = default;

		void Load() { db_->LoadTeams(*data_); }

		void Save() { db_->SaveTeams(*data_); }
	};
}