#include "LootBoxInterface.h"

#include <LootBox/GogLootBox.h>

#pragma comment(lib, "Lootboxes.lib")

namespace AutoDecay::Boxes
{
	TArray<Boxes::BoxData> GetPlayerBoxes(uint64 steam_id)
	{
		TArray<BoxData> out{};

		const auto boxes = GogLootBox::GetPlayerBoxes(steam_id);

		if (!boxes.empty())
		{
			for (const auto& box : boxes)
			{
				out.Push(
					BoxData{
						FString(box.boxName),
						box.amount
					}
				);
			}
		}

		return out;
	}

	bool RemoveBoxes(uint64 steam_id, const FString& box_name, int amount)
	{
		return GogLootBox::RemoveBoxes(steam_id, box_name.ToString(), amount) == "";
	}
}