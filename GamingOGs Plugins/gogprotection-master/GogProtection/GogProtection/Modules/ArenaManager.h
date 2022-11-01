#ifndef _GOG_PROT_ARENA_MANAGER_H_
#define _GOG_PROT_ARENA_MANAGER_H_

#include <json.hpp>
#include <API/UE/Math/Vector.h>

namespace Protection
{
	struct ArenaPivotPoint
	{
		ArenaPivotPoint()
			: radius(0.0f), x(0.0f), y(0.0f), z(0.0f)
		{
		}

		ArenaPivotPoint(float radius, float x, float y, float z) :
			radius(radius), x(x), y(y), z(z)
		{
		}

		float radius;
		float x, y, z;
	};

	class ArenaManager
	{
	public:
		ArenaManager(const nlohmann::json& arenas_json);

		bool IsInArena(FVector* position);


	private:
		bool IsInParticularArena(FVector* position, const ArenaPivotPoint& arena);

		std::vector<ArenaPivotPoint> arenas_;
	};

}

#endif