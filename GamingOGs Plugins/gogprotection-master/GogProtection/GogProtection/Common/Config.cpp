#include "Config.h"

#include <fstream>
#include <string>
#include <Tools.h>

namespace Protection
{
	nlohmann::json& GetConfig()
	{
		return config_;
	}

	bool ParseConfig()
	{
		const std::string config_path = ArkApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/PlayerProtection/config.json";

		std::ifstream f(config_path);

		if (!f.is_open()) {
			return false;
		}

		try {
			f >> config_;
		}
		catch (const std::exception&) {
			return false;
		}
		f.close();

		return true;
	}

	FString GetText(const std::string& label)
	{
		return FString(ArkApi::Tools::Utf8Decode(config_["AP"]["Messages"][label]).c_str());
	}

	TArray<FString> GetBlackListGroups()
	{
		TArray<FString> out;

		for (auto& item : GetConfig()["BlackListGroups"])
			out.Push(FString(static_cast<std::string>(item)));

		return out;
	}

	DiscordChannelData GetDiscordChannelData()
	{
		return DiscordChannelData{
			static_cast<std::string>(GetConfig()["DiscordChannel"]["Tag"]),
			static_cast<std::string>(GetConfig()["DiscordChannel"]["BotName"])
		};
	}

	bool IsMetricsEnabled()
	{
		return GetConfig()["EnableMetrics"].get<bool>();
	}
}