#ifndef _GOG_PROT_TEAM_MANAGER_H_
#define _GOG_PROT_TEAM_MANAGER_H_

#include "../Storage/Storage.h"

namespace Protection
{
	class TeamManager
	{
	public:
		TeamManager(Storage* storage);
		~TeamManager();

		void RegisterPlayer(PlayerItemType& player);
		void RegisterTeam(TeamItemType& team);
		void UnregisterTeam(TeamItemType& team);
		void MergeTeams(TeamItemType& old_team, TeamItemType& new_team);
		void AllyTeams(TeamItemType& first_team, TeamItemType& second_team);
		void RemovePlayerFromTeam(TeamItemType& team, PlayerItemType& player);
		void AddPlayerToTeam(TeamItemType& team, PlayerItemType& player);
		bool IsTeamEmpty(TeamItemType& team);

	private:
		unsigned long long DetermineLowestAPActivationTime(const TeamItemType& first_team, const TeamItemType& second_team) const;
		unsigned long long DetermineLowestAPDeactivationTime(const TeamItemType& first_team, const TeamItemType& second_team) const;
		unsigned long long DetermineHighestDamageTakenTime(const TeamItemType& first_team, const TeamItemType& second_team) const;
		bool DetermineBlackListStatus(const TeamItemType& first_team, const TeamItemType& second_team) const;
		bool DetermineHarvestBonusStatus(const TeamItemType& first_team, const TeamItemType& second_team) const;
		void MergeTeamsData(TeamItemType& first_team, TeamItemType& second_team);

		Storage* storage_;
	};
}

#endif // !_GOG_PROT_TEAM_MANAGER_H_
