#include "DragManager.h"

#include "../Common/Helpers.h"

namespace Protection
{
	DragManager::DragManager(const nlohmann::json& flyers, const nlohmann::json& grounds, bool carry_flag, bool drag_flag)
		: can_carry_flag_(carry_flag), can_drag_flag_(drag_flag)
	{
		for (const auto& bp : flyers)
		{
			flyers_exceptions_.Push(bp.get<std::string>().c_str());
		}

		for (const auto& bp : grounds)
		{
			grounds_exceptions_.Push(bp.get<std::string>().c_str());
		}
	}

	bool DragManager::CanCarry(APrimalDinoCharacter* dino)
	{
		if (!can_carry_flag_)
		{
			return false;
		}
		else
		{
			FString bp = Helpers::GetBlueprint(dino);
			return !flyers_exceptions_.Contains(bp);
		}
	}

	bool DragManager::CanDrag(APrimalDinoCharacter* dino)
	{
		if (!can_drag_flag_)
		{
			return false;
		}
		else
		{
			FString bp = Helpers::GetBlueprint(dino);
			return !grounds_exceptions_.Contains(bp);
		}
	}
}