#include "ArenaManager.h"

namespace Protection
{

	ArenaManager::ArenaManager(const nlohmann::json& arenas_json)
	{
		for (const auto& arena : arenas_json)
		{
			arenas_.push_back(ArenaPivotPoint{
				arena["radius"].get<float>(),
				arena["x"].get<float>(),
				arena["y"].get<float>(),
				arena["z"].get<float>()
			});
		}
	}

	bool ArenaManager::IsInArena(FVector* position)
	{
		for (const auto& arena : arenas_)
		{
			if (IsInParticularArena(position, arena))
			{
				return true;
			}
		}
		return false;
	}

	bool ArenaManager::IsInParticularArena(FVector* position, const ArenaPivotPoint& arena)
	{
		const float distance = FVector::Distance(*position, FVector{ arena.x, arena.y, arena.z});
		return distance <= arena.radius;
	}

}