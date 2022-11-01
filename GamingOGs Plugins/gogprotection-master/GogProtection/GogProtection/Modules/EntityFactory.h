#ifndef _GOG_PROT_ENTITY_FACTORY_H_
#define _GOG_PROT_ENTITY_FACTORY_H_

#include "../Storage/Storage.h"
#include <memory>

namespace Protection
{
	class EntityFactory
	{
	public:
		EntityFactory(APTimingRules* ap_timing_rules);
		~EntityFactory();

		PlayerItemType MakePlayer(unsigned long long steam_id);
		TeamItemType MakeTeam(int team_id);
		TeamItemType MakeTeamBasedOn(int new_team_id, TeamItemType& source_team);

	private:
		APTimingRules* ap_timing_rules_;
	};
}

#endif // !_GOG_PROT_ENTITY_FACTORY_H_
