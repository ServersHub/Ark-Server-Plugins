#pragma once

#include <string>
#include <vector>

#include <API/UE/Containers/FString.h>
#include <API/UE/Containers/TArray.h>
#include <json.hpp>

#include "../storage/PlayerProps.h"
#include "../storage/TeamProps.h"
#include "../utils/Types.h"
#include "../external/PermissionsInterface.h"

namespace AutoDecay
{
	class Storage;

	class RankGroup
	{
	public:
		RankGroup() = delete;
		~RankGroup() = default;
		RankGroup(const nlohmann::basic_json<>& j);

		bool CheckPlayer(Player& player) const;

		std::string GetIdentifier() const
		{
			return identifier_;
		}

		bool operator<(const RankGroup& other) const
		{
			return this->weight_ < other.weight_;
		}

		bool operator>(const RankGroup& other) const
		{
			return this->weight_ > other.weight_;
		}

	private:
		std::string identifier_;
		int weight_;
		TArray<FString> permissions_;
	};

	class RankGroupsManager
	{
	public:
		RankGroupsManager() = delete;
		~RankGroupsManager() = default;
		RankGroupsManager(const nlohmann::basic_json<>& j, Storage* storage);

		RankGroup GetDefaultGroup() const;
		RankGroup GetGroup(Player& player) const;
		RankGroup GetGroup(Team& team) const;

	private:
		std::vector<RankGroup> groups_;
		Storage* storage_;
	};
}