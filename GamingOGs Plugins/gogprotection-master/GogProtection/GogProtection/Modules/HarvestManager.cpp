#include "HarvestManager.h"

namespace Protection
{
	HarvestManager::HarvestManager(float modifier)
		: modifier_(modifier)
	{
	}

	bool HarvestManager::TeamHasBonus(TeamItemType& team)
	{
		if (team)
			return team->GetHarvestFlag();
		else
			return false;
	}

	void HarvestManager::SetBonus(TeamItemType& team)
	{
		if (team)
			team->SetHarvestFlag(true);
	}

	void HarvestManager::UnsetBonus(TeamItemType& team)
	{
		if (team)
			team->SetHarvestFlag(false);
	}

	int HarvestManager::CalculateAmount(int initial_amount)
	{
		const float amount_tmp = static_cast<float>(initial_amount);
		return static_cast<int>(amount_tmp * modifier_);
	}
}