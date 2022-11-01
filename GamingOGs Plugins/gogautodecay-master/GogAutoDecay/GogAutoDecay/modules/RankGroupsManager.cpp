#include "RankGroupsManager.h"

#include "../storage/Storage.h"
#include "../external/PermissionsInterface.h"

namespace AutoDecay
{
	RankGroup::RankGroup(const nlohmann::basic_json<>& j)
		: identifier_(""), weight_(0)
	{
		identifier_ = j["Identifier"];
		weight_ = j["Weight"];

		for (const auto& permission : j["Permissions"])
		{
			permissions_.Push(FString(permission.get<std::string>()));
		}
	}

	bool RankGroup::CheckPlayer(Player& player) const
	{
		for (const auto& permission : permissions_)
		{
			TArray<FString> player_permissions = Perms::GetPlayerGroups(player->GetSteamId());
			if (player_permissions.Contains(permission))
			{
				return true;
			}
		}

		return false;
	}

	RankGroupsManager::RankGroupsManager(const nlohmann::basic_json<>& j, Storage* storage)
		: storage_(storage)
	{
		for (const auto& rank_group : j)
		{
			groups_.push_back(RankGroup{ rank_group });
		}

		std::sort(groups_.begin(), groups_.end(),
			[](const RankGroup& lhs, const RankGroup& rhs)
			{
				return lhs < rhs;
			});
	}

	RankGroup RankGroupsManager::GetDefaultGroup() const
	{
		auto it = std::find_if(groups_.begin(), groups_.end(),
			[](const RankGroup& group)
			{
				return group.GetIdentifier() == "Default";
			});

		return *it;
	}

	RankGroup RankGroupsManager::GetGroup(Player& player) const
	{
		RankGroup max_group = GetDefaultGroup();

		if (!player)
		{
			return max_group;
		}

		for (auto it = groups_.rbegin(); it != groups_.rend(); it++)
		{
			if (it->CheckPlayer(player))
			{
				max_group = *it;
				break;
			}
		}

		return max_group;
	}

	RankGroup RankGroupsManager::GetGroup(Team& team) const
	{
		RankGroup max_group = GetDefaultGroup();

		if (!team)
		{
			return max_group;
		}

		std::vector<Player> players{};
		std::for_each(storage_->Players()->Begin(), storage_->Players()->End(),
			[&players, team](const Player& player)
			{
				if (player->GetTeamId() == team->GetTeamId())
				{
					players.push_back(player);
				}
			});

		if (players.empty())
		{
			return max_group;
		}

		for (auto player : players)
		{
			RankGroup player_group = GetGroup(player);

			if (player_group > max_group)
			{
				max_group = player_group;
			}
		}

		return max_group;
	}
}