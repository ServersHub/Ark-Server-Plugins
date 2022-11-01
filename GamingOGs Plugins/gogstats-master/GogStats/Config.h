#ifndef _GOG_STATS_CONFIG_H_
#define _GOG_STATS_CONFIG_H_

#include <string>
#include <fstream>
#include <vector>
#include <json.hpp>
#include <Tools.h>

namespace Stats
{
	inline nlohmann::json config_;

	struct DBConfig
	{
		std::string host;
		std::string user;
		std::string password;
		std::string database;
		std::vector<std::string> customFields;
	};

	inline bool IsRareDino(const std::string& blueprint)
	{
		for (const auto& item : config_["SpecialDinos"].items())
		{
			if (blueprint == item.value().get<std::string>())
			{
				return true;
			}
		}
		return false;
	}

	inline std::string GetCustomFieldName(const std::string& blueprint)
	{
		for (const auto& item : config_["SpecialDinos"].items())
		{
			if (blueprint == item.value().get<std::string>())
			{
				return item.key();
			}
		}
	}

	inline DBConfig GetDBConfig()
	{
		const auto mysqlNode = config_["MySQL"];
		std::vector<std::string> customFields{};
		for (const auto& item : config_["SpecialDinos"].items())
		{
			customFields.push_back(item.key());
		}
		return DBConfig{
			mysqlNode["Host"],
			mysqlNode["User"],
			mysqlNode["Password"],
			mysqlNode["Database"],
			std::move(customFields)
		};
	}

	inline std::string GetServerId()
	{
		return config_["ServerId"].get<std::string>();
	}

	inline std::string GetLicenseKey()
	{
		const std::string configPath = ArkApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/Statistics/config.json";
		std::ifstream file{ configPath };

		if (!file.is_open())
		{
			throw std::runtime_error("Can't open config.json");
		}

		try
		{
			file >> config_;
		}
		catch (const std::exception& e)
		{
			throw std::runtime_error(std::string{ "Parse error: " } + e.what());
		}
		file.close();

		if (!config_.contains("/LicenseKey"_json_pointer))
		{
			throw std::runtime_error("Can't load license key");
		}

		return config_["/LicenseKey"_json_pointer].get<std::string>();
	}

}

#endif // !_GOG_STATS_CONFIG_H_
