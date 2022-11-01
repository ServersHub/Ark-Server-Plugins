#include "EntityFactory.h"

#include <Permissions/ArkPermissions.h>
#include <API/UE/Containers/FString.h>
#include <API/UE/Containers/TArray.h>

#include "../Common/Config.h"
#include "../Common/Helpers.h"

namespace Protection
{
	EntityFactory::EntityFactory(APTimingRules* ap_timing_rules)
		: ap_timing_rules_(ap_timing_rules)
	{
	}

	EntityFactory::~EntityFactory()
	{
	}

	PlayerItemType EntityFactory::MakePlayer(unsigned long long steam_id)
	{
		PlayerItemType new_player = std::make_shared<PlayerData>();

		TArray<FString> blacklist_groups = GetBlackListGroups();
		TArray<FString> player_groups = Permissions::GetPlayerGroups(steam_id);
		bool is_blacklisted = false;

		for (const auto& group : blacklist_groups) {
			if (player_groups.Find(group) != INDEX_NONE) {
				is_blacklisted = true;
				break;
			}
		}

		new_player->SetSteamId(steam_id);
		new_player->SetTeamId(0);
		new_player->SetBlackListFlag(is_blacklisted);
		new_player->SetOnlineFlag(false);

		return new_player;
	}

	TeamItemType EntityFactory::MakeTeam(int team_id)
	{
		TeamItemType new_team = std::make_shared<TeamData>();

		unsigned long long now = time(0);

		new_team->SetTeamId(team_id);
		new_team->SetAPActivationTime(now);
		new_team->SetAPDeactivationTime(now + ap_timing_rules_->GetAPDefaultDurationSec());
		new_team->SetORPActivationTime(0ULL);
		new_team->SetORPDeactivationTime(0ULL);
		new_team->SetLastCombatTime(0ULL);
		new_team->SetBlackListFlag(false);
		new_team->SetHarvestFlag(false);

		return new_team;
	}

	TeamItemType EntityFactory::MakeTeamBasedOn(int new_team_id, TeamItemType& source_team)
	{
		if (!source_team)
			return MakeTeam(new_team_id);

		TeamItemType new_team = std::make_shared<TeamData>();

		new_team->SetTeamId(new_team_id);
		new_team->SetAPActivationTime(source_team->GetAPActivationTime());
		new_team->SetAPDeactivationTime(source_team->GetAPDeactivationTime());
		new_team->SetORPActivationTime(source_team->GetORPActivationTime());
		new_team->SetORPDeactivationTime(source_team->GetORPDeactivationTime());
		new_team->SetLastCombatTime(source_team->GetLastCombatTime());
		new_team->SetBlackListFlag(source_team->GetBlackListFlag());
		new_team->SetHarvestFlag(source_team->GetHarvestFlag());

		return new_team;
	}
}