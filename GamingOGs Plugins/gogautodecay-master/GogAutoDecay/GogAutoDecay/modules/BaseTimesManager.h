#pragma once

#include <API/UE/Containers/FString.h>
#include <API/UE/Containers/TArray.h>
#include <json.hpp>

#include "../utils/Types.h"

namespace AutoDecay
{
	class TimesSettings
	{
	public:
		TimesSettings()
			: structures(0), dinos(0)
		{
		}

		TimesSettings(int structures, int dinos, const TArray<FString>& groups)
			: structures(structures), dinos(dinos), groups(groups)
		{
		}

		TArray<FString> groups;
		int structures;
		int dinos;
	};

	class BaseTimesManager
	{
	public:
		BaseTimesManager() = delete;
		~BaseTimesManager() = default;

		BaseTimesManager(const nlohmann::basic_json<>& j);

		RefPtr<TimesSettings> GetSettings(const std::string& group_id);

	private:
		std::vector<RefPtr<TimesSettings>> settings_;
	};
}