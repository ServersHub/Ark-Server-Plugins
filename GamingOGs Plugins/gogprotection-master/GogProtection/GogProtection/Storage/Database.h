#ifndef _GOG_PROT_DATABASE_H_
#define _GOG_PROT_DATABASE_H_

#include <memory>
#include <string>
#include <vector>

#include "../Storage/DataTypes.h"

namespace Protection
{
    namespace DB
    {
		class DatabaseImpl;

        class Database
        {
        public:
            Database();
            ~Database();

			bool Connect(const std::string& host, const std::string& user, const std::string& pass, const std::string& db);
			bool CheckStructure();

			void LoadPlayers(std::vector<std::shared_ptr<PlayerData>> & out);
			void LoadTeams(std::vector<std::shared_ptr<TeamData>>& out);

			void SavePlayers(const std::vector<std::shared_ptr<PlayerData>>& data);
			void SaveTeams(const std::vector<std::shared_ptr<TeamData>>& data);

			void CleanPlayers();
			void CleanTeams();

		private:
			std::unique_ptr<DatabaseImpl> impl_;
        };
    }
}

#endif // !_GOG_PROT_DATABASE_H_