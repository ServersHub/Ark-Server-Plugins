#ifndef _GOG_PROT_HARVEST_MANAGER_H_
#define _GOG_PROT_HARVEST_MANAGER_H_

#include "../Storage/Storage.h"

namespace Protection
{
	class HarvestManager
	{
	public:
		HarvestManager(float modifier);
		~HarvestManager() = default;

		bool TeamHasBonus(TeamItemType& team);
		void SetBonus(TeamItemType& team);
		void UnsetBonus(TeamItemType& team);
		int CalculateAmount(int initial_amount);

	private:
		float modifier_;
	};
}

#endif // !_GOG_PROT_HARVEST_MANAGER_H_
