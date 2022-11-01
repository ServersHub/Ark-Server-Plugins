#pragma once

#include "../storage/PlayerProps.h"
#include "../storage/TeamProps.h"
#include "../utils/Types.h"

namespace AutoDecay
{
	class EntityFactory
	{
	public:
		Player CreatePlayer(unsigned long long steam_id, int team_id) const;

		Team CreateTeam(int team_id) const;
	};
}