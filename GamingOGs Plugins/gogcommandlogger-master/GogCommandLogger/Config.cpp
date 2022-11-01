#include "Config.h"

#include <fstream>
#include <Tools.h>

namespace CommandLogger
{
	bool ParseConfig()
	{
#ifdef ARK
		const std::string config_path = ArkApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/CommandLogger/config.json";
#else
		const std::string config_path = ArkApi::Tools::GetCurrentDir() + "/AtlasApi/Plugins/CommandLogger/config.json";
#endif
		
		std::fstream file(config_path);

		if (!file.is_open())
			return false;

		try {
			file >> config_;
		}
		catch (const std::exception&) {
			file.close();
			return false;
		}

		file.close();
		return true;
	}

	nlohmann::json& GetConfig()
	{
		return config_;
	}

	std::string GetLicenseKey()
	{
		const std::string configPath = ArkApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/CommandLogger/config.json";
		std::ifstream file{ configPath };

		if (!file.is_open())
		{
			throw std::runtime_error("Can't open config.json");
		}

		nlohmann::json j{};
		file >> j;
		file.close();

		if (!j.contains("/LicenseKey"_json_pointer))
		{
			throw std::runtime_error("Can't load license key");
		}

		return j["/LicenseKey"_json_pointer].get<std::string>();
	}
}