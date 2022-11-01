#pragma once

#include <vector>
#include <memory>

#include "PlayerProps.h"
#include "TeamProps.h"

namespace AutoDecay
{
	class DatabaseImpl;

	class Database
	{
	public:
		Database();
		~Database();

		bool Connect(std::string host, std::string user, std::string pass, std::string db);
		bool CheckStructure();

		void LoadPlayers(std::vector<Player>& out);
		void LoadTeams(std::vector<Team>& out);

		void SavePlayers(const std::vector<Player>& data);
		void SaveTeams(const std::vector<Team>& data);

	private:
		std::unique_ptr<DatabaseImpl> impl_;
	};
}