#pragma once

#include "ApiProxy.h"

namespace AutoDecay
{
	namespace Helpers
	{
		FString Seconds2String(time_t seconds);

		AActor* GetAimedActor(APlayerController* player_controller);

		FString GetStructureBlueprint(UObjectBase* actor);
	}
}