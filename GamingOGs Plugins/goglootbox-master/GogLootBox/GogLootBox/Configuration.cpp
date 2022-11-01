#include "Configuration.h"
#include "Box.h"

#include <fstream>
#include <Tools.h>

namespace LootBox
{

	void Configuration::Load()
	{
		m_mysqlHost = m_ParseString("/General/Mysql/MysqlHost"_json_pointer, "/General/Mysql/MysqlHost"_json_pointer, container);
		m_mysqlUser = m_ParseString("/General/Mysql/MysqlUser"_json_pointer, "/General/Mysql/MysqlUser"_json_pointer, container);
		m_mysqlPass = m_ParseString("/General/Mysql/MysqlPass"_json_pointer, "/General/Mysql/MysqlPass"_json_pointer, container);
		m_mysqlDB = m_ParseString("/General/Mysql/MysqlDB"_json_pointer, "/General/Mysql/MysqlDB"_json_pointer, container);

		m_enableMetrics = m_ParseBool("/General/Metrics/EnableMetrics"_json_pointer, "/General/Metrics/EnableMetrics"_json_pointer, container);

		m_ParseMetricsParams(container);
		m_ParseRandomBoxesParams(container);
		m_ParseDinoExclusionsParams(container);

		m_messages["Sender"] = m_ParseFString("/General/Messages/Sender"_json_pointer, "/General/Messages/Sender"_json_pointer, container);
		m_messages["WonItem"] = m_ParseFString("/General/Messages/WonItem"_json_pointer, "/General/Messages/WonItem"_json_pointer, container);
		m_messages["WonBox"] = m_ParseFString("/General/Messages/WonBox"_json_pointer, "/General/Messages/WonBox"_json_pointer, container);
		m_messages["NoBoxesLeft"] = m_ParseFString("/General/Messages/NoBoxesLeft"_json_pointer, "/General/Messages/NoBoxesLeft"_json_pointer, container);
		m_messages["BoxNotExist"] = m_ParseFString("/General/Messages/BoxNotExist"_json_pointer, "/General/Messages/BoxNotExist"_json_pointer, container);
		m_messages["TraderId"] = m_ParseFString("/General/Messages/TraderId"_json_pointer, "/General/Messages/TraderId"_json_pointer, container);
		m_messages["WrongTraderId"] = m_ParseFString("/General/Messages/WrongTraderId"_json_pointer, "/General/Messages/WrongTraderId"_json_pointer, container);
		m_messages["ShowBoxes"] = m_ParseFString("/General/Messages/ShowBoxes"_json_pointer, "/General/Messages/ShowBoxes"_json_pointer, container);
		m_messages["ShowBoxesEmpty"] = m_ParseFString("/General/Messages/ShowBoxesEmpty"_json_pointer, "/General/Messages/ShowBoxesEmpty"_json_pointer, container);
		m_messages["Unlucky"] = m_ParseFString("/General/Messages/Unlucky"_json_pointer, "/General/Messages/Unlucky"_json_pointer, container);
		m_messages["CantGamble"] = m_ParseFString("/General/Messages/CantGamble"_json_pointer, "/General/Messages/CantGamble"_json_pointer, container);
		m_messages["NoPermission"] = m_ParseFString("/General/Messages/NoPermission"_json_pointer, "/General/Messages/NoPermission"_json_pointer, container);
		m_messages["NoPoints"] = m_ParseFString("/General/Messages/NoPoints"_json_pointer, "/General/Messages/NoPoints"_json_pointer, container);
		m_messages["TradedBoxes"] = m_ParseFString("/General/Messages/TradedBoxes"_json_pointer, "/General/Messages/TradedBoxes"_json_pointer, container);
		m_messages["TradedBoxesSuccess"] = m_ParseFString("/General/Messages/TradedBoxesSuccess"_json_pointer, "/General/Messages/TradedBoxesSuccess"_json_pointer, container);
		m_messages["TradeBoxesBroadcast"] = m_ParseFString("/General/Messages/TradeBoxesBroadcast"_json_pointer, "/General/Messages/TradeBoxesBroadcast"_json_pointer, container);
		m_messages["IncompatibleMap"] = m_ParseFString("/General/Messages/IncompatibleMap"_json_pointer, "/General/Messages/IncompatibleMap"_json_pointer, container);
		m_messages["CannotGiveDino"] = m_ParseFString("/General/Messages/CannotGiveDino"_json_pointer, "/General/Messages/CannotGiveDino"_json_pointer, container);
		m_messages["CannotGiveItem"] = m_ParseFString("/General/Messages/CannotGiveItem"_json_pointer, "/General/Messages/CannotGiveItem"_json_pointer, container);
		m_messages["BoxesDisabled"] = m_ParseFString("/General/Messages/BoxesDisabled"_json_pointer, "/General/Messages/BoxesDisabled"_json_pointer, container);
		m_messages["ContactAdmins"] = m_ParseFString("/General/Messages/ContactAdmins"_json_pointer, "/General/Messages/ContactAdmins"_json_pointer, container);

		m_chatCommands["OpenBox"] = m_ParseFString("/General/Commands/Chat/OpenBox"_json_pointer, "/General/Commands/Chat/OpenBox"_json_pointer, container);
		m_chatCommands["ShowBoxes"] = m_ParseFString("/General/Commands/Chat/ShowBoxes"_json_pointer, "/General/Commands/Chat/ShowBoxes"_json_pointer, container);
		m_chatCommands["Gamble"] = m_ParseFString("/General/Commands/Chat/Gamble"_json_pointer, "/General/Commands/Chat/Gamble"_json_pointer, container);
		m_chatCommands["TradeBox"] = m_ParseFString("/General/Commands/Chat/TradeBox"_json_pointer, "/General/Commands/Chat/TradeBox"_json_pointer, container);
		m_chatCommands["TraderId"] = m_ParseFString("/General/Commands/Chat/TraderId"_json_pointer, "/General/Commands/Chat/TraderId"_json_pointer, container);

		m_consoleCommands["GiveBox"] = m_ParseFString("/General/Commands/Console/GiveBox"_json_pointer, "/General/Commands/Console/GiveBox"_json_pointer, container);
		m_consoleCommands["SetBox"] = m_ParseFString("/General/Commands/Console/SetBox"_json_pointer, "/General/Commands/Console/SetBox"_json_pointer, container);
		m_consoleCommands["RemoveBox"] = m_ParseFString("/General/Commands/Console/RemoveBox"_json_pointer, "/General/Commands/Console/RemoveBox"_json_pointer, container);
		m_consoleCommands["RandBox"] = m_ParseFString("/General/Commands/Console/RandBox"_json_pointer, "/General/Commands/Console/RandBox"_json_pointer, container);
		m_consoleCommands["GiveBoxAll"] = m_ParseFString("/General/Commands/Console/GiveBoxAll"_json_pointer, "/General/Commands/Console/GiveBoxAll"_json_pointer, container);
		m_consoleCommands["SetBoxAll"] = m_ParseFString("/General/Commands/Console/SetBoxAll"_json_pointer, "/General/Commands/Console/SetBoxAll"_json_pointer, container);
		m_consoleCommands["RemoveBoxAll"] = m_ParseFString("/General/Commands/Console/RemoveBoxAll"_json_pointer, "/General/Commands/Console/RemoveBoxAll"_json_pointer, container);
		m_consoleCommands["RandBoxAll"] = m_ParseFString("/General/Commands/Console/RandBoxAll"_json_pointer, "/General/Commands/Console/RandBoxAll"_json_pointer, container);
		m_consoleCommands["EnableBoxes"] = m_ParseFString("/General/Commands/Console/EnableBoxes"_json_pointer, "/General/Commands/Console/EnableBoxes"_json_pointer, container);
		m_consoleCommands["DisableBoxes"] = m_ParseFString("/General/Commands/Console/DisableBoxes"_json_pointer, "/General/Commands/Console/DisableBoxes"_json_pointer, container);

		m_rconCommands["GiveBox"] = m_ParseFString("/General/Commands/Rcon/GiveBox"_json_pointer, "/General/Commands/Rcon/GiveBox"_json_pointer, container);
		m_rconCommands["SetBox"] = m_ParseFString("/General/Commands/Rcon/SetBox"_json_pointer, "/General/Commands/Rcon/SetBox"_json_pointer, container);
		m_rconCommands["RemoveBox"] = m_ParseFString("/General/Commands/Rcon/RemoveBox"_json_pointer, "/General/Commands/Rcon/RemoveBox"_json_pointer, container);
		m_rconCommands["RandBox"] = m_ParseFString("/General/Commands/Rcon/RandBox"_json_pointer, "/General/Commands/Rcon/RandBox"_json_pointer, container);
		m_rconCommands["GiveBoxAll"] = m_ParseFString("/General/Commands/Rcon/GiveBoxAll"_json_pointer, "/General/Commands/Rcon/GiveBoxAll"_json_pointer, container);
		m_rconCommands["SetBoxAll"] = m_ParseFString("/General/Commands/Rcon/SetBoxAll"_json_pointer, "/General/Commands/Rcon/SetBoxAll"_json_pointer, container);
		m_rconCommands["RemoveBoxAll"] = m_ParseFString("/General/Commands/Rcon/RemoveBoxAll"_json_pointer, "/General/Commands/Rcon/RemoveBoxAll"_json_pointer, container);
		m_rconCommands["RandBoxAll"] = m_ParseFString("/General/Commands/Rcon/RandBoxAll"_json_pointer, "/General/Commands/Rcon/RandBoxAll"_json_pointer, container);
		m_rconCommands["EnableBoxes"] = m_ParseFString("/General/Commands/Rcon/EnableBoxes"_json_pointer, "/General/Commands/Rcon/EnableBoxes"_json_pointer, container);
		m_rconCommands["DisableBoxes"] = m_ParseFString("/General/Commands/Rcon/DisableBoxes"_json_pointer, "/General/Commands/Rcon/DisableBoxes"_json_pointer, container);

		if (!container.contains("/Boxes"_json_pointer))
		{
			throw ConfigurationErrorNotExist("/Boxes"_json_pointer);
		}
		m_ParseBoxes(container["Boxes"]);
	}

	std::string Configuration::GetLicenseKey()
	{
		const std::string configPath = ArkApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/Lootboxes/config.json";
		std::ifstream file{ configPath };

		if (!file.is_open())
		{
			throw std::runtime_error("Can't open config.json");
		}

		file >> container;
		file.close();

		if (!container.contains("/General/LicenseKey"_json_pointer))
		{
			throw std::runtime_error("Can't load license key");
		}

		return container["/General/LicenseKey"_json_pointer].get<std::string>();
	}

	void Configuration::CheckMapCompatibility(const std::string& mapName)
	{
		if (m_mapCompatibilityCheckDone)
		{
			return;
		}

		for (auto& box : m_boxes)
		{
			box->CheckCompatibility(mapName);
		}
		m_mapCompatibilityCheckDone = true;
	}

	Optional<MetricsChannel> Configuration::GetMetricsChannelInfo(const std::string& channelName) const
	{
		const auto it = m_metricsChannels.find(channelName);
		return it != m_metricsChannels.end() ? std::make_optional<MetricsChannel>(it->second) : std::nullopt;
	}

	bool Configuration::IsDinoAllowed(const std::string& mapName, const std::string blueprint) const noexcept
	{
		const auto mapIter = m_dinoExclusions.find(mapName);
		if (mapIter != m_dinoExclusions.end())
		{
			const auto blueprintIter = std::find(mapIter->second.begin(), mapIter->second.end(), blueprint);
			if (blueprintIter != mapIter->second.end())
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		else
		{
			return true;
		}
	}

	std::shared_ptr<Box> Configuration::GetBox(const std::string& boxName) const
	{
		const auto it = std::find_if(m_boxes.begin(), m_boxes.end(), [boxName](const std::shared_ptr<Box>& box) {
			return FString(box->Name()).ToLower() == FString(boxName).ToLower();
		});
		return it != m_boxes.end() ? *it : nullptr;
	}

	void Configuration::m_ParseMetricsParams(const Json& container)
	{
		if (!container.contains("/General/Metrics/Channels"_json_pointer))
		{
			throw ConfigurationErrorNotExist("/General/Metrics/Channels"_json_pointer);
		}
		for (const auto& channel : container["/General/Metrics/Channels"_json_pointer].items())
		{
			const auto tagPtr = JsonPointer{ "/General/Metrics/Channels/" + channel.key() + "/Tag" };
			const auto botNamePtr = JsonPointer{ "/General/Metrics/Channels/" + channel.key() + "/BotName" };
			const auto channelPtr = JsonPointer{ "/General/Metrics/Channels/" + channel.key() };
			m_metricsChannels[channel.key()] = MetricsChannel{
				m_ParseString("/Tag"_json_pointer, tagPtr, container[channelPtr]),
				m_ParseString("/BotName"_json_pointer, botNamePtr, container[channelPtr])
			};
		}
	}

	void Configuration::m_ParseRandomBoxesParams(const Json& container)
	{
		if (!container.contains("/General/RandomBoxes"_json_pointer))
		{
			throw ConfigurationErrorNotExist("/General/RandomBoxes"_json_pointer);
		}
		m_repeatBoxesInterval = m_ParseInt("/General/RandomBoxes/DontRepeatBoxes"_json_pointer, "/General/RandomBoxes/DontRepeatBoxes"_json_pointer, container);
		if (!container.contains("/General/RandomBoxes/Boxes"_json_pointer))
		{
			throw ConfigurationErrorNotExist("/General/RandomBoxes/Boxes"_json_pointer);
		}
		for (const auto& box : container["/General/RandomBoxes/Boxes"_json_pointer])
		{
			m_randomBoxes.push_back(box.get<std::string>());
		}
	}

	void Configuration::m_ParseDinoExclusionsParams(const Json& container)
	{
		if (!container.contains("/General/ExcludeDinos"_json_pointer))
		{
			throw ConfigurationErrorNotExist("/General/ExcludeDinos"_json_pointer);
		}
		for (const auto& map : container["/General/ExcludeDinos"_json_pointer].items())
		{
			std::vector<std::string> tmp{};
			for (const auto& bp : map.value())
			{
				tmp.push_back(bp);
			}
			m_dinoExclusions[map.key()] = std::move(tmp);
		}
	}

	void Configuration::m_ParseBoxes(const Json& container)
	{
		for (const auto& box : container.items())
		{
			std::shared_ptr<Box> parsedBox = m_ParseBox(box.key(), JsonPointer{ std::string{"/Boxes/"} + box.key() }, box.value());
			m_boxes.push_back(parsedBox);
		}
	}

	std::shared_ptr<Box> Configuration::m_ParseBox(std::string name, const JsonPointer& ptr, const Json& container)
	{
		if (!container.contains("/Items"_json_pointer))
		{
			throw ConfigurationErrorNotExist(JsonPointer{ ptr.to_string() + "/Items" });
		}

		ElementsList boxElements{};
		int counter = 0;
		for (const auto& elementJson : container["/Items"_json_pointer])
		{
			const std::string elementTypeStr = m_ParseString("/Type"_json_pointer, JsonPointer{ ptr.to_string() + "/Items/" + std::to_string(counter) + "/Type" }, elementJson);
			BoxElementType elementType = TypeStringToEnum(elementTypeStr);
			std::shared_ptr<BoxElementBase> element = nullptr;

			switch (elementType)
			{
			case BoxElementType::Item:
				element = m_ParseItemElement(JsonPointer{ ptr.to_string() + "/Items/" + std::to_string(counter) }, elementJson);
				break;
			case BoxElementType::Dino:
				element = m_ParseDinoElement(JsonPointer{ ptr.to_string() + "/Items/" + std::to_string(counter) }, elementJson);
				break;
			case BoxElementType::Beacon:
				element = m_ParseBeaconElement(JsonPointer{ ptr.to_string() + "/Items/" + std::to_string(counter) }, elementJson);
				break;
			case BoxElementType::Command:
				element = m_ParseCommandElement(JsonPointer{ ptr.to_string() + "/Items/" + std::to_string(counter) }, elementJson);
				break;
			case BoxElementType::Points:
				element = m_ParsePointsElement(JsonPointer{ ptr.to_string() + "/Items/" + std::to_string(counter) }, elementJson);
				break;
			}

			boxElements.push_back(element);
		}

		return std::make_shared<Box>(
			std::move(name),
			m_ParseInt("/GamblePercent"_json_pointer, JsonPointer{ ptr.to_string() + "/GamblePercent" }, container),
			m_ParseInt("/GamblePrice"_json_pointer, JsonPointer{ ptr.to_string() + "/GamblePrice" }, container),
			m_ParseString("/Permission"_json_pointer, JsonPointer{ ptr.to_string() + "/Permission" }, container),
			m_ParseInt("/MinAmount"_json_pointer, JsonPointer{ ptr.to_string() + "/MinAmount" }, container),
			m_ParseInt("/MaxAmount"_json_pointer, JsonPointer{ ptr.to_string() + "/MaxAmount" }, container),
			std::move(boxElements)
			);
	}

	std::shared_ptr<BoxElementBase> Configuration::m_ParseItemElement(const JsonPointer& ptr, const Json& container)
	{
		if (!container.contains("/Items"_json_pointer))
		{
			throw ConfigurationErrorNotExist(JsonPointer{ ptr.to_string() + "/Items" });
		}

		ItemsList items{};
		int counter = 0;
		for (const auto& element : container["/Items"_json_pointer])
		{
			std::shared_ptr<Item> item = m_ParseItem(JsonPointer{ ptr.to_string() + "/Items/" + std::to_string(counter) }, element);
			items.push_back(item);
			counter++;
		}

		return std::make_shared<BoxElementItem>(
			m_ParseString("/Description"_json_pointer, JsonPointer{ ptr.to_string() + "/Description" }, container),
			m_ParseInt("/Probability"_json_pointer, JsonPointer{ ptr.to_string() + "/Probability" }, container),
			std::move(items)
			);
	}

	std::shared_ptr<BoxElementBase> Configuration::m_ParseDinoElement(const JsonPointer& ptr, const Json& container)
	{
		if (!container.contains("/Dinos"_json_pointer))
		{
			throw ConfigurationErrorNotExist(JsonPointer{ ptr.to_string() + "/Dinos" });
		}

		DinosList dinos{};
		int counter = 0;
		for (const auto& element : container["/Dinos"_json_pointer])
		{
			std::shared_ptr<Dino> dino = m_ParseDino(JsonPointer{ ptr.to_string() + "/Dinos/" + std::to_string(counter) }, element);
			dinos.push_back(dino);
			counter++;
		}

		return std::make_shared<BoxElementDino>(
			m_ParseString("/Description"_json_pointer, JsonPointer{ ptr.to_string() + "/Description" }, container),
			m_ParseInt("/Probability"_json_pointer, JsonPointer{ ptr.to_string() + "/Probability" }, container),
			std::move(dinos)
			);
	}

	std::shared_ptr<BoxElementBase> Configuration::m_ParseBeaconElement(const JsonPointer& ptr, const Json& container)
	{
		return std::make_shared<BoxElementBeacon>(
			m_ParseString("/Description"_json_pointer, JsonPointer{ ptr.to_string() + "/Description" }, container),
			m_ParseInt("/Probability"_json_pointer, JsonPointer{ptr.to_string() + "/Probability"}, container),
			m_ParseString("/ClassName"_json_pointer, JsonPointer{ ptr.to_string() + "/ClassName" }, container)
			);
	}

	std::shared_ptr<BoxElementBase> Configuration::m_ParseCommandElement(const JsonPointer& ptr, const Json& container)
	{
		return std::make_shared<BoxElementCommand>(
			m_ParseString("/Description"_json_pointer, JsonPointer{ ptr.to_string() + "/Description" }, container),
			m_ParseInt("/Probability"_json_pointer, JsonPointer{ ptr.to_string() + "/Probability" }, container),
			m_ParseString("/Command"_json_pointer, JsonPointer{ ptr.to_string() + "/Command" }, container)
			);
	}
	
	std::shared_ptr<BoxElementBase> Configuration::m_ParsePointsElement(const JsonPointer& ptr, const Json& container)
	{
		return std::make_shared<BoxElementPoints>(
			m_ParseString("/Description"_json_pointer, JsonPointer{ ptr.to_string() + "/Description" }, container),
			m_ParseInt("/Probability"_json_pointer, JsonPointer{ ptr.to_string() + "/Probability" }, container),
			m_ParseInt("/MinPoints"_json_pointer, JsonPointer{ ptr.to_string() + "/MinPoints" }, container),
			m_ParseInt("/MaxPoints"_json_pointer, JsonPointer{ ptr.to_string() + "/MaxPoints" }, container)
			);
	}

	std::shared_ptr<Item> Configuration::m_ParseItem(const JsonPointer& ptr, const Json& container)
	{
		return std::make_shared<Item>(
			m_ParseInt("/MinQuality"_json_pointer, JsonPointer{ ptr.to_string() + "/MinQuality" }, container),
			m_ParseInt("/MaxQuality"_json_pointer, JsonPointer{ ptr.to_string() + "/MaxQuality" }, container),
			m_ParseInt("/Amount"_json_pointer, JsonPointer{ ptr.to_string() + "/Amount" }, container),
			m_ParseIntOptional("/AddStat"_json_pointer, JsonPointer{ ptr.to_string() + "/AddStat" }, container),
			m_ParseInt("/BlueprintChance"_json_pointer, JsonPointer{ ptr.to_string() + "/BlueprintChance" }, container),
			m_ParseString("/Blueprint"_json_pointer, JsonPointer{ ptr.to_string() + "/Blueprint" }, container)
			);
	}

	std::shared_ptr<Dino> Configuration::m_ParseDino(const JsonPointer& ptr, const Json& container)
	{
		return std::make_shared<Dino>(
			m_ParseInt("/Level"_json_pointer, JsonPointer{ ptr.to_string() + "/Level" }, container),
			m_ParseBool("/BabyDino"_json_pointer, JsonPointer{ ptr.to_string() + "/BabyDino" }, container),
			m_ParseInt("/ImprintAmount"_json_pointer, JsonPointer{ ptr.to_string() + "/ImprintAmount" }, container),
			m_ParseBoolOptional("/Neutered"_json_pointer, JsonPointer{ ptr.to_string() + "/Neutered" }, container),
			m_ParseString("/Blueprint"_json_pointer, JsonPointer{ ptr.to_string() + "/Blueprint" }, container),
			m_ParseStringOptional("/SaddleBlueprint"_json_pointer, JsonPointer{ ptr.to_string() + "/SaddleBlueprint" }, container),
			m_ParseIntOptional("/SaddleQuality"_json_pointer, JsonPointer{ ptr.to_string() + "/SaddleQuality" }, container)
			);
	}

	FString Configuration::m_ParseFString(const JsonPointer& relativePath, const JsonPointer& absolutePath, const Json& container)
	{
		if (!container.contains(relativePath))
		{
			throw ConfigurationErrorNotExist(absolutePath);
		}
		const Json& val = container.at(relativePath);
		if (val.type() != Json::value_t::string)
		{
			throw ConfigurationErrorWrongType(absolutePath);
		}
		return FString(ArkApi::Tools::Utf8Decode(val.get<std::string>()).c_str());
	};

	std::string Configuration::m_ParseString(const JsonPointer& relativePath, const JsonPointer& absolutePath, const Json& container)
	{
		if (!container.contains(relativePath))
		{
			throw ConfigurationErrorNotExist(absolutePath);
		}
		const Json& val = container.at(relativePath);
		if (val.type() != Json::value_t::string)
		{
			throw ConfigurationErrorWrongType(absolutePath);
		}
		return val.get<std::string>();
	};

	int Configuration::m_ParseInt(const JsonPointer& relativePath, const JsonPointer& absolutePath, const Json& container)
	{
		if (!container.contains(relativePath))
		{
			throw ConfigurationErrorNotExist(absolutePath);
		}
		const Json& val = container.at(relativePath);
		if (val.type() != Json::value_t::number_integer && val.type() != Json::value_t::number_unsigned)
		{
			throw ConfigurationErrorWrongType(absolutePath);
		}
		return val.get<int>();
	};

	bool Configuration::m_ParseBool(const JsonPointer& relativePath, const JsonPointer& absolutePath, const Json& container)
	{
		if (!container.contains(relativePath))
		{
			throw ConfigurationErrorNotExist(absolutePath);
		}
		const Json& val = container.at(relativePath);
		if (val.type() != Json::value_t::boolean)
		{
			throw ConfigurationErrorWrongType(absolutePath);
		}
		return val.get<bool>();
	};

	Optional<std::string> Configuration::m_ParseStringOptional(const JsonPointer& relativePath, const JsonPointer& absolutePath, const Json& container)
	{
		if (!container.contains(relativePath))
		{
			return std::nullopt;
		}
		const Json& val = container.at(relativePath);
		if (val.type() != Json::value_t::string)
		{
			throw ConfigurationErrorWrongType(absolutePath);
		}
		return val.get<std::string>();
	};

	Optional<int> Configuration::m_ParseIntOptional(const JsonPointer& relativePath, const JsonPointer& absolutePath, const Json& container)
	{
		if (!container.contains(relativePath))
		{
			return std::nullopt;
		}
		const Json& val = container.at(relativePath);
		if (val.type() != Json::value_t::number_integer && val.type() != Json::value_t::number_unsigned)
		{
			throw ConfigurationErrorWrongType(absolutePath);
		}
		return val.get<int>();
	};

	Optional<bool> Configuration::m_ParseBoolOptional(const JsonPointer& relativePath, const JsonPointer& absolutePath, const Json& container)
	{
		if (!container.contains(relativePath))
		{
			return std::nullopt;
		}
		const Json& val = container.at(relativePath);
		if (val.type() != Json::value_t::boolean)
		{
			throw ConfigurationErrorWrongType(absolutePath);
		}
		return val.get<bool>();
	}

}