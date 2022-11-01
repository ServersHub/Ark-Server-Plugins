#include "OnlineManager.h"

#include "../storage/Storage.h"

namespace AutoDecay
{
	OnlineManager::OnlineManager(Storage* storage)
		: storage_(storage)
	{
	}

	bool OnlineManager::IsPlayerOnline(Player& player)
	{
		return player->GetOnlineFlag();
	}

	bool OnlineManager::IsTeamOnline(Team& team)
	{
		const int team_id = team->GetTeamId();

		const auto cbk = [team_id](const Player& player)
		{
			return player->GetTeamId() == team_id && player->GetOnlineFlag();
		};

		const std::size_t count = std::count_if(storage_->Players()->Begin(), storage_->Players()->End(), cbk);

		return count > 0;
	}

	void OnlineManager::PlayerOnline(Player& player)
	{
		player->SetOnlineFlag(true);

		Team team = storage_->Teams()->Get(player->GetTeamId());

		if (team)
		{
			team->SetLastSeenTime(0);
		}
	}

	void OnlineManager::PlayerOffline(Player& player)
	{
		player->SetOnlineFlag(false);

		Team team = storage_->Teams()->Get(player->GetTeamId());

		if (team && !IsTeamOnline(team))
		{
			team->SetLastSeenTime(time(0));
		}
	}
}