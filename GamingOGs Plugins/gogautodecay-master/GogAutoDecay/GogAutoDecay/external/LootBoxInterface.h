#pragma once

#include <API/UE/BasicTypes.h>
#include <API/UE/Containers/TArray.h>
#include <API/UE/Containers/FString.h>

namespace AutoDecay::Boxes
{
	struct BoxData
	{
		FString box_name;
		int amount;
	};

	TArray<BoxData> GetPlayerBoxes(uint64 steam_id);
	bool RemoveBoxes(uint64 steam_id, const FString& box_name, int amount);
}