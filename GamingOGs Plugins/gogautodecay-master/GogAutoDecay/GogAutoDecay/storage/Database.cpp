#include "Database.h"
#include "DatabaseImpl.h"

namespace AutoDecay
{
	Database::Database()
		: impl_(std::make_unique<DatabaseImpl>())
	{
	}

	Database::~Database()
	{
	}

	bool Database::Connect(std::string host, std::string user, std::string pass, std::string db)
	{
		return impl_->Connect(std::move(host), std::move(user), std::move(pass), std::move(db));
	}

	bool Database::CheckStructure()
	{
		return impl_->CheckStructure();
	}

	void Database::LoadPlayers(std::vector<Player>& out)
	{
		impl_->LoadPlayers(out);
	}

	void Database::LoadTeams(std::vector<Team>& out)
	{
		impl_->LoadTeams(out);
	}

	void Database::SavePlayers(const std::vector<Player>& data)
	{
		impl_->SavePlayers(data);
	}

	void Database::SaveTeams(const std::vector<Team>& data)
	{
		impl_->SaveTeams(data);
	}
}