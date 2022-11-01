#include "OnlineMonitor.h"

namespace Protection
{
	OnlineMonitor::OnlineMonitor(Storage* storage)
		: storage_(storage)
	{
	}

	OnlineMonitor::~OnlineMonitor()
	{
	}

	bool OnlineMonitor::IsPlayerOnline(const PlayerItemType& player)
	{
		if (player)
			return player->GetOnlineFlag();

		return false;
	}

	bool OnlineMonitor::IsTeamOnline(const TeamItemType& team)
	{
		bool result = false;

		if (team) {
			const auto begin_iter = storage_->Players()->Begin();
			const auto end_iter = storage_->Players()->End();

			auto pred = [&team](const std::shared_ptr<PlayerData>& item)
			{
				return item->GetTeamId() == team->GetTeamId() && item->GetOnlineFlag();
			};

			size_t count = std::count_if(begin_iter, end_iter, pred);

			result = (count > 0);
		}

		return result;
	}

	void OnlineMonitor::PlayerOnline(const PlayerItemType& player)
	{
		if (player)
			player->SetOnlineFlag(true);
	}

	void OnlineMonitor::PlayerOffline(const PlayerItemType& player)
	{
		if (player)
			player->SetOnlineFlag(false);
	}
}