#pragma once

#include "Platform.h"

#include <json.hpp>
#include <map>

namespace BountyHunter
{
	inline nlohmann::json json_conf;

	struct BountyTarget
	{
		unsigned long long steam_id_ = 0;
		time_t marked_time_ = 0;
		time_t logout_time_ = 0;
		std::string target_name_ = "";
	};

	inline std::map<uint64, time_t> penalized;

	inline BountyTarget target;

	std::string GetLicenseKey();

	FString Msg(const std::string& title);

	FString Cmd(const std::string& title);

	void RewardPlayer(AShooterPlayerController* controller);

	void PickTarget(uint64 steam_id = 0);

	void NotifyOnNewTarget();

	void NotifyOnClaimTarget(AShooterPlayerController* controller);

	void Load();

	void Unload();
}