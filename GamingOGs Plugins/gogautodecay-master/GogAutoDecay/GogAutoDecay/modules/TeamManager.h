#pragma once

#include "../storage/PlayerProps.h"
#include "../storage/TeamProps.h"

namespace AutoDecay
{
	class Storage;

	class TeamManager
	{
	public:
		TeamManager(Storage* storage);
		
		void RegisterPlayer(Player& player);
		void RegisterTeam(Team& team);
		void UnregisterTeam(Team& team);
		void MergeTeams(Team& old_team, Team& new_team);
		void RemovePlayerFromTeam(Team& team, Player& player);
		void AddPlayerToTeam(Team& team, Player& player);
		bool IsTeamEmpty(Team& team);

	private:
		Storage* storage_;
	};
}