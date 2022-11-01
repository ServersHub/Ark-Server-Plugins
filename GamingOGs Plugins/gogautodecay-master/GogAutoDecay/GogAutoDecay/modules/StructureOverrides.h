#pragma once

#include <vector>

#include <API/UE/Containers/FString.h>
#include <json.hpp>

#include "../utils/Types.h"

namespace AutoDecay
{
	struct Override
	{
		Override(const FString& bp, int time_sec)
			: blueprint(bp), decay_time(time_sec)
		{
		}

		virtual ~Override() = default;

		FString blueprint;
		int decay_time;
	};

	struct DuoOverride
	{
		DuoOverride(const FString& bp, const FString& foundation_bp, int time_sec)
			: blueprint(bp), foundation_blueprint(foundation_bp), decay_time(time_sec)
		{
		}

		FString blueprint;
		FString foundation_blueprint;
		int decay_time;
	};

	class SoloOverrides
	{
	public:
		SoloOverrides() = delete;
		~SoloOverrides() = default;

		SoloOverrides(const nlohmann::basic_json<>& j)
		{
			for (const auto& item : j)
			{
				overrides_.push_back(Override{ FString(item["BlueprintPath"].get<std::string>()),
											   item["DecayTime"].get<int>() * 60 });
			}
		}

		int GetOverride(const FString& bp)
		{
			const auto it = std::find_if(overrides_.begin(), overrides_.end(),
				[&bp](const Override& item)
				{
					return item.blueprint.Equals(bp);
				});

			if (it != overrides_.end())
			{
				return it->decay_time;
			}
			return 0;
		}

	private:
		std::vector<Override> overrides_;
	};

	class DuoOverrides
	{
	public:
		DuoOverrides() = delete;
		~DuoOverrides() = default;

		DuoOverrides(const nlohmann::basic_json<>& j)
		{
			for (const auto& item : j)
			{
				duo_overrides_.push_back(DuoOverride{ FString(item["BlueprintPath"].get<std::string>()),
													  FString(item["PlacedOn"].get<std::string>()),
													  item["DecayTime"].get<int>() * 60 });
			}
		}

		int GetOverride(const FString& bp, const FString& foundation_bp)
		{
			const auto it = std::find_if(duo_overrides_.begin(), duo_overrides_.end(),
				[&bp, &foundation_bp](const DuoOverride& item)
				{
					return item.blueprint.Equals(bp) && item.foundation_blueprint.Equals(foundation_bp);
				});

			if (it != duo_overrides_.end())
			{
				return it->decay_time;
			}
			return 0;
		}

	private:
		std::vector<DuoOverride> duo_overrides_;
	};
}