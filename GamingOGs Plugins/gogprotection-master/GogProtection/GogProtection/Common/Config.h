#ifndef _GOG_PROT_CONFIG_H_
#define _GOG_PROT_CONFIG_H_

#include <API/UE/Containers/FString.h>
#include <API/UE/Math/Color.h>
#include <string>
#include <fstream>
#include <json.hpp>

namespace Protection
{
	struct DiscordChannelData
	{
		std::string tag;
		std::string bot_name;
	};

	nlohmann::json& GetConfig();

	bool ParseConfig();

	FString GetText(const std::string& label);

	TArray<FString> GetBlackListGroups();

	DiscordChannelData GetDiscordChannelData();

	bool IsMetricsEnabled();

	inline nlohmann::json config_;
}

#endif // !_GOG_PROT_CONFIG_H_