#include "Helpers.h"

#include "Config.h"
#include <Permissions/ArkPermissions.h>

namespace Protection
{
    namespace Helpers
    {
		FString ToHumanReadableTime(time_t seconds)
		{
			if (seconds < 60)
				return "0 minutes";

			seconds = seconds / 60;

			const uint64 MINUTE = 1;
			const uint64 HOUR = MINUTE * 60;
			const uint64 DAY = HOUR * 24;

			uint64 r_days = 0;
			uint64 r_hours = 0;
			uint64 r_minutes = 0;

			if (seconds >= DAY) {
				r_days = seconds / DAY;
				seconds -= r_days * DAY;
			}
			if (seconds >= HOUR) {
				r_hours = seconds / HOUR;
				seconds -= r_hours * HOUR;
			}
			if (seconds >= MINUTE) {
				r_minutes = seconds;
			}

			FString reply;
			if (r_days > 0)
				reply += FString::Format("{} days ", r_days);
			if (r_hours > 0)
				reply += FString::Format("{} hours ", r_hours);
			if (r_minutes > 0)
				reply += FString::Format("{} minutes", r_minutes);


			reply.TrimEndInline();
			return reply;
		}

		uint64 ValidateSteamID(const FString& cmd, int param_num)
		{
			TArray<FString> parsed;
			cmd.ParseIntoArray(parsed, L" ");
			if (parsed.IsValidIndex(param_num))
				return std::strtoull(parsed[1].ToString().c_str(), nullptr, 10);
			return 0ULL;
		}

		int ValidateTeamID(const FString& cmd, int param_num)
		{
			TArray<FString> parsed;
			cmd.ParseIntoArray(parsed, L" ");
			if (parsed.IsValidIndex(param_num))
				return std::strtol(parsed[1].ToString().c_str(), nullptr, 10);
			return 0;
		}

		std::string ExtractZoneID(const FString& cmd, int param_num)
		{
			std::string out = "";
			TArray<FString> parsed;

			cmd.ParseIntoArray(parsed, L" ");
			if (parsed.IsValidIndex(param_num))
				out = parsed[1].ToString();
			return out;
		}

		int GetAimedActorTeam(APlayerController* player_controller)
		{
			int team_id = 0;

			AShooterPlayerController* shooter_controller = static_cast<AShooterPlayerController*>(player_controller);
			APrimalCharacter* character = static_cast<APrimalCharacter*>(shooter_controller->CharacterField());
			if (character) {
				AActor* aimed_actor = character->GetAimedActor(ECollisionChannel::ECC_GameTraceChannel2, 0i64, 0.0, 0.0, 0i64, 0i64, 0, 0, 0);
				if (aimed_actor)
					team_id = aimed_actor->TargetingTeamField();
			}

			return team_id;
		}

		FString GetBlueprint(UObjectBase* object)
		{
			if (object != nullptr && object->ClassField() != nullptr)
			{
				FString path_name;
				object->ClassField()->GetDefaultObject(true)->GetFullName(&path_name, nullptr);

				if (int find_index = 0; path_name.FindChar(' ', find_index))
				{
					path_name = "Blueprint'" + path_name.Mid(find_index + 1,
						path_name.Len() - (find_index + (path_name.EndsWith(
							"_C", ESearchCase::
							CaseSensitive)
							? 3
							: 1))) + "'";
					return path_name.Replace(L"Default__", L"", ESearchCase::CaseSensitive);
				}
			}

			return FString("");
		}
    }
}