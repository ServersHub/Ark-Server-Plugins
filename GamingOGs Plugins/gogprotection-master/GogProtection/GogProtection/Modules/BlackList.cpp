#include "BlackList.h"

#include "../Storage/Storage.h"

namespace Protection
{
	BlackList::BlackList(Storage* storage)
		: storage_(storage)
	{
	}

	BlackList::~BlackList()
	{
	}

	bool BlackList::IsTeamBlackListed(TeamItemType& team)
	{
		if (!team)
			return false;

		if (IsTeamHasBlackListedPlayers(team))
			return true;
		else
			return team->GetBlackListFlag();
	}

	void BlackList::ForceAddTeam(TeamItemType& team)
	{
		if (team)
			team->SetBlackListFlag(true);
	}

	bool BlackList::ForceRemoveTeam(TeamItemType& team)
	{
		if (!team)
			return false;

		team->SetBlackListFlag(false);

		if (!IsTeamHasBlackListedPlayers(team))
			return true;
		else
			return false;
	}

	bool BlackList::IsTeamHasBlackListedPlayers(const TeamItemType& team)
	{
		auto compare_func = [&team](const PlayerItemType& player)
		{
			return team->GetTeamId() == player->GetTeamId() && player->GetBlackListFlag();
		};

		auto begin_iter = storage_->Players()->Begin();
		auto end_iter = storage_->Players()->End();

		const auto it = std::find_if(begin_iter, end_iter, compare_func);

		return it != end_iter;
	}
}