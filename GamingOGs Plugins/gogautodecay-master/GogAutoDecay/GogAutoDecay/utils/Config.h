#pragma once

#include <Tools.h>
#include <Logger/Logger.h>
#include <API/UE/Containers/FString.h>
#include <Tools.h>
#include <json.hpp>
#include <fstream>

namespace AutoDecay
{
	inline nlohmann::json config_root_;

	inline bool ParseConfiguration()
	{
#ifdef ARK
		const auto conf_path = ArkApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/AutoDecay/config.json";
#else
		const auto conf_path = ArkApi::Tools::GetCurrentDir() + "/AtlasApi/Plugins/AutoDecay/config.json";
#endif

		std::ifstream f(conf_path);

		if (!f.is_open())
		{
			Log::GetLog()->error("Can't open {}", conf_path);
			return false;
		}

		try {
			f >> config_root_;
		}
		catch (const std::exception & e) {
			Log::GetLog()->error("Can't parse config, error: {}", e.what());
			f.close();
			return false;
		}

		f.close();

		return true;
	}

	inline nlohmann::json& GetConfiguration()
	{
		return config_root_;
	}

	inline FString GetMsg(const std::string& label)
	{
		return FString(ArkApi::Tools::Utf8Decode(GetConfiguration()["Messages"][label]).c_str());
	}

	inline FString GetCmd(const std::string& label)
	{
		return FString(ArkApi::Tools::Utf8Decode(GetConfiguration()["Commands"][label]).c_str());
	}
}
