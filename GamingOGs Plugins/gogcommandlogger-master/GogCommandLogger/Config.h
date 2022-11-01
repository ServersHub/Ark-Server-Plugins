#ifndef _GOG_COMMAND_LOGGER_CONFIG_H_
#define _GOG_COMMAND_LOGGER_CONFIG_H_

#include <json.hpp>
#include <API/UE/Containers/FString.h>
#include <API/UE/Containers/TArray.h>

namespace CommandLogger
{
	bool ParseConfig();

	nlohmann::json& GetConfig();

	std::string GetLicenseKey();

	inline nlohmann::json config_;
}

#endif // !_GOG_COMMAND_LOGGER_CONFIG_H_
