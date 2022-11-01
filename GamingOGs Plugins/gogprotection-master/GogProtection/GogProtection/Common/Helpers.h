#ifndef _GOG_PROT_HELPERS_H_
#define _GOG_PROT_HELPERS_H_

#include <ctime>
#include <API/ARK/Ark.h>

namespace Protection
{
    namespace Helpers
    {
		FString ToHumanReadableTime(time_t seconds);

		uint64 ValidateSteamID(const FString& cmd, int param_num);

		int ValidateTeamID(const FString& cmd, int param_num);

		std::string ExtractZoneID(const FString& cmd, int param_num);

		int GetAimedActorTeam(APlayerController* player_controller);

		FString GetBlueprint(UObjectBase* object);
    }
}

#endif // !_GOG_PROT_HELPERS_H_