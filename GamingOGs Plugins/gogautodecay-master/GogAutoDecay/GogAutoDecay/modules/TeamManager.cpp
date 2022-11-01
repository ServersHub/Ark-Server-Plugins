#include "TeamManager.h"

#include "../storage/Storage.h"

namespace AutoDecay
{
	TeamManager::TeamManager(Storage* storage)
		: storage_(storage)
	{
	}

	void TeamManager::RegisterPlayer(Player& player)
	{
		if (!storage_->Players()->Get(player->GetSteamId()))
		{
			storage_->Players()->Insert(player);
		}
	}

	void TeamManager::RegisterTeam(Team& team)
	{
		if (!storage_->Teams()->Get(team->GetTeamId()))
		{
			storage_->Teams()->Insert(team);
		}
	}

	void TeamManager::UnregisterTeam(Team& team)
	{
		storage_->Teams()->Remove(team->GetTeamId());
	}

	void TeamManager::MergeTeams(Team& old_team, Team& new_team)
	{
		auto replacer_func = [&](const Player& player)
		{
			if (player->GetTeamId() == old_team->GetTeamId())
				player->SetTeamId(new_team->GetTeamId());
		};

		std::for_each(storage_->Players()->Begin(), storage_->Players()->End(), replacer_func);
	}

	void TeamManager::RemovePlayerFromTeam(Team& team, Player& player)
	{
		player->SetTeamId(0);
		if (IsTeamEmpty(team))
		{
			team->SetLastSeenTime(time(0));
		}
	}

	void TeamManager::AddPlayerToTeam(Team& team, Player& player)
	{
		player->SetTeamId(team->GetTeamId());
		team->SetLastSeenTime(0);
	}

	bool TeamManager::IsTeamEmpty(Team& team)
	{
		auto comp_func = [&team](const Player& item)
		{
			return item->GetTeamId() == team->GetTeamId();
		};

		const auto count = std::count_if(storage_->Players()->Begin(), storage_->Players()->End(), comp_func);

		return count <= 0;
	}
}