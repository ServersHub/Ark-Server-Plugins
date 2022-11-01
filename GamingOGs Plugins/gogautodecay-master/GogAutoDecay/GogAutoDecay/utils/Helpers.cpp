#include "Helpers.h"

namespace AutoDecay
{
	namespace Helpers
	{
		FString Seconds2String(time_t seconds)
		{
			if (seconds < 60)
			{
				return "0 minutes";
			}

			seconds = seconds / 60;

			const uint64 MINUTE = 1;
			const uint64 HOUR = MINUTE * 60;
			const uint64 DAY = HOUR * 24;

			uint64 r_days = 0;
			uint64 r_hours = 0;
			uint64 r_minutes = 0;

			if (seconds >= DAY)
			{
				r_days = seconds / DAY;
				seconds -= r_days * DAY;
			}
			if (seconds >= HOUR)
			{
				r_hours = seconds / HOUR;
				seconds -= r_hours * HOUR;
			}
			if (seconds >= MINUTE)
			{
				r_minutes = seconds;
			}

			FString reply;
			if (r_days > 0)
			{
				reply += FString::Format("{} days ", r_days);
			}
			if (r_hours > 0)
			{
				reply += FString::Format("{} hours ", r_hours);
			}
			if (r_minutes > 0)
			{
				reply += FString::Format("{} minutes", r_minutes);
			}

			reply.TrimEndInline();
			return reply;
		}

		AActor* GetAimedActor(APlayerController* player_controller)
		{
			int team_id = 0;

			AShooterPlayerController* shooter_controller = reinterpret_cast<AShooterPlayerController*>(player_controller);
			APrimalCharacter* character = reinterpret_cast<APrimalCharacter*>(shooter_controller->CharacterField());

			if (character)
			{
#ifdef ARK
				AActor* aimed_actor = character->GetAimedActor(ECollisionChannel::ECC_GameTraceChannel2, 0i64, 0.0, 0.0, 0i64, 0i64, 0, 0, 0);
#else
				AActor* aimed_actor = character->GetAimedActor(ECollisionChannel::ECC_GameTraceChannel2, 0i64, 0.0, 0.0, 0i64, 0i64, 0, 0, 0);
#endif
				return aimed_actor;
			}

			return nullptr;
		}

		FString GetStructureBlueprint(UObjectBase* object)
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