#include "EntityFactory.h"

namespace AutoDecay
{
	Player EntityFactory::CreatePlayer(unsigned long long steam_id, int team_id) const
	{
		Player player = std::make_shared<PlayerProps>();
		player->SetSteamId(steam_id);
		player->SetTeamId(team_id);
		return player;
	}

	Team EntityFactory::CreateTeam(int team_id) const
	{
		Team team = std::make_shared<TeamProps>();
		team->SetTeamId(team_id);
		return team;
	}
}