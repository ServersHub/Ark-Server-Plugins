#include "TeamManager.h"

namespace Protection
{
	TeamManager::TeamManager(Storage* storage)
		: storage_(storage)
	{
	}

	TeamManager::~TeamManager()
	{
	}

	void TeamManager::RegisterPlayer(PlayerItemType& player)
	{
		if (player && !storage_->Players()->Get(player->GetSteamId()))
			storage_->Players()->Insert(player);
	}

	void TeamManager::RegisterTeam(TeamItemType& team)
	{
		if (team && !storage_->Teams()->Get(team->GetTeamId()))
			storage_->Teams()->Insert(team);
	}

	void TeamManager::UnregisterTeam(TeamItemType& team)
	{
		if (team)
			storage_->Teams()->Remove(team->GetTeamId());
	}

	void TeamManager::MergeTeams(TeamItemType& old_team, TeamItemType& new_team)
	{
		if (!old_team || !new_team)
			return;

		MergeTeamsData(old_team, new_team);

		auto replacer_func = [&](const PlayerItemType& player)
		{
			if (player->GetTeamId() == old_team->GetTeamId())
				player->SetTeamId(new_team->GetTeamId());
		};

		std::for_each(storage_->Players()->Begin(), storage_->Players()->End(), replacer_func);
	}

	void TeamManager::AllyTeams(TeamItemType& first_team, TeamItemType& second_team)
	{
		if (first_team && second_team)
			MergeTeamsData(first_team, second_team);
	}

	void TeamManager::RemovePlayerFromTeam(TeamItemType& team, PlayerItemType& player)
	{
		if (team && player)
			player->SetTeamId(0);
	}

	void TeamManager::AddPlayerToTeam(TeamItemType& team, PlayerItemType& player)
	{
		if (team && player)
			player->SetTeamId(team->GetTeamId());
	}

	bool TeamManager::IsTeamEmpty(TeamItemType& team)
	{
		if (!team)
			return true;

		auto comp_func = [&team](const PlayerItemType& item)
		{
			return item->GetTeamId() == team->GetTeamId();
		};

		const auto count = std::count_if(storage_->Players()->Begin(), storage_->Players()->End(), comp_func);

		return count <= 0;
	}

	unsigned long long TeamManager::DetermineLowestAPActivationTime(const TeamItemType& first_team, const TeamItemType& second_team) const
	{
		if (!second_team && first_team)
			return first_team->GetAPActivationTime();

		if (!first_team && second_team)
			return second_team->GetAPActivationTime();

		if (!first_team && !second_team)
			return 0ULL;

		auto first_team_value = first_team->GetAPActivationTime();
		auto second_team_value = second_team->GetAPActivationTime();

		return (first_team_value <= second_team_value) ? first_team_value : second_team_value;
	}

	unsigned long long TeamManager::DetermineLowestAPDeactivationTime(const TeamItemType& first_team, const TeamItemType& second_team) const
	{
		if (!second_team && first_team)
			return first_team->GetAPDeactivationTime();

		if (!first_team && second_team)
			return second_team->GetAPDeactivationTime();

		if (!first_team && !second_team)
			return 0ULL;

		auto first_team_value = first_team->GetAPDeactivationTime();
		auto second_team_value = second_team->GetAPDeactivationTime();

		return (first_team_value <= second_team_value) ? first_team_value : second_team_value;
	}

	unsigned long long TeamManager::DetermineHighestDamageTakenTime(const TeamItemType& first_team, const TeamItemType& second_team) const
	{
		if (!second_team && first_team)
			return first_team->GetLastCombatTime();

		if (!first_team && second_team)
			return second_team->GetLastCombatTime();

		if (!first_team && !second_team)
			return 0ULL;

		auto first_team_value = first_team->GetLastCombatTime();
		auto second_team_value = second_team->GetLastCombatTime();

		return (first_team_value >= second_team_value) ? first_team_value : second_team_value;
	}

	bool TeamManager::DetermineBlackListStatus(const TeamItemType& first_team, const TeamItemType& second_team) const
	{
		if (!second_team && first_team)
			return first_team->GetBlackListFlag();

		if (!first_team && second_team)
			return second_team->GetBlackListFlag();

		if (!first_team && !second_team)
			return false;

		if (first_team->GetBlackListFlag() || second_team->GetBlackListFlag())
			return true;
		else
			return false;
	}

	bool TeamManager::DetermineHarvestBonusStatus(const TeamItemType& first_team, const TeamItemType& second_team) const
	{
		if (!second_team && first_team)
			return first_team->GetHarvestFlag();

		if (!first_team && second_team)
			return second_team->GetHarvestFlag();

		if (!first_team && !second_team)
			return false;

		if (first_team->GetHarvestFlag() || second_team->GetHarvestFlag())
			return true;
		else
			return false;
	}
	void TeamManager::MergeTeamsData(TeamItemType& first_team, TeamItemType& second_team)
	{
		unsigned long long lowest_ap_activation_time = DetermineLowestAPActivationTime(first_team, second_team);
		unsigned long long lowest_ap_deactivation_time = DetermineLowestAPDeactivationTime(first_team, second_team);
		unsigned long long highest_damage_taken_time = DetermineHighestDamageTakenTime(first_team, second_team);
		bool black_list_status = DetermineBlackListStatus(first_team, second_team);
		bool harvest_bonus_status = DetermineHarvestBonusStatus(first_team, second_team);

		first_team->SetAPActivationTime(lowest_ap_activation_time);
		first_team->SetAPDeactivationTime(lowest_ap_deactivation_time);
		first_team->SetLastCombatTime(highest_damage_taken_time);
		first_team->SetBlackListFlag(black_list_status);
		first_team->SetHarvestFlag(harvest_bonus_status);

		second_team->SetAPActivationTime(lowest_ap_activation_time);
		second_team->SetAPDeactivationTime(lowest_ap_deactivation_time);
		second_team->SetLastCombatTime(highest_damage_taken_time);
		second_team->SetBlackListFlag(black_list_status);
		second_team->SetHarvestFlag(harvest_bonus_status);
	}
}