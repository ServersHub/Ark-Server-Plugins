#include "BaseTimesManager.h"

namespace AutoDecay
{
	BaseTimesManager::BaseTimesManager(const nlohmann::basic_json<>& j)
	{
		for (const auto& item : j)
		{
			TArray<FString> groups{};
			int structures, dinos;

			for (const auto& group : item["RankGroups"])
			{
				groups.Push(FString(group.get<std::string>()));
			}
			structures = item["StructuresDecayTime"].get<int>() * 60;
			dinos = item["DinosDecayTime"].get<int>() * 60;

			settings_.push_back(std::make_shared<TimesSettings>(structures, dinos, groups));
		}
	}

	RefPtr<TimesSettings> BaseTimesManager::GetSettings(const std::string& group_id)
	{
		RefPtr<TimesSettings> defaults = nullptr;

		for (const auto& setting : settings_)
		{
			if (setting->groups.Contains(L"Default"))
			{
				defaults = setting;
			}

			if (setting->groups.Contains(FString(group_id)))
			{
				return setting;
			}
		}
		return defaults;
	}
}