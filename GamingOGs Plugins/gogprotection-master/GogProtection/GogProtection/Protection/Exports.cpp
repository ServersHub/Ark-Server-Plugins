#include "Exports.h"

#include "Application.h"
#include "../Storage/Storage.h"
#include "../Modules/ProtManager.h"

namespace Protection
{
	__declspec(dllexport) bool IsTeamProtected(int team_id)
	{
		auto team = App->GetStorage()->Teams()->Get(team_id);
		if (!team)
		{
			return false;
		}

		return App->GetProtManager()->IsProtectionActive(team, ProtectionType::AP);
	}
}