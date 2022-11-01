#include "Database.h"
#include "DatabaseImpl.h"
#include "../Common/Config.h"

namespace Protection
{
	namespace DB
	{
		Database::Database()
			: impl_(std::make_unique<DatabaseImpl>())
		{
		}

		Database::~Database()
		{
		}

		bool Database::Connect(const std::string& host, const std::string& user, const std::string& pass, const std::string& db)
		{
			return impl_->Connect(host, user, pass, db);
		}

		bool Database::CheckStructure()
		{
			return impl_->CheckStructure();
		}

		void Database::LoadPlayers(std::vector<std::shared_ptr<PlayerData>>& out)
		{
			impl_->LoadPlayers(out);
		}

		void Database::LoadTeams(std::vector<std::shared_ptr<TeamData>>& out)
		{
			impl_->LoadTeams(out);
		}

		void Database::SavePlayers(const std::vector<std::shared_ptr<PlayerData>>& data)
		{
			impl_->SavePlayers(data);
		}

		void Database::SaveTeams(const std::vector<std::shared_ptr<TeamData>>& data)
		{
			impl_->SaveTeams(data);
		}

		void Database::CleanPlayers()
		{
			impl_->CleanPlayers();
		}

		void Database::CleanTeams()
		{
			impl_->CleanTeams();
		}
	}
}