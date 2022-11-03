#ifndef _LOOTBOX_CONFIGURATION_H_
#define _LOOTBOX_CONFIGURATION_H_

#include <API/UE/Containers/FString.h>
#include <string>
#include <vector>
#include <map>

#include "Types.h"

namespace LootBox
{

	class ConfigurationErrorNotExist : public std::exception
	{
	public:
		ConfigurationErrorNotExist(const JsonPointer& ptr)
		{
			m_msg = "Path '" + ptr.to_string() + "' can't be read or doesn't exist";
		}

		const char* what() const override
		{
			return m_msg.c_str();
		}

	private:
		std::string m_msg;
	};

	class ConfigurationErrorWrongType : public std::exception
	{
	public:
		ConfigurationErrorWrongType(const JsonPointer& ptr)
		{
			m_msg = "Value at path '" + ptr.to_string() + "' has wrong type";
		}

		const char* what() const override
		{
			return m_msg.c_str();
		}

	private:
		std::string m_msg;
	};

	struct MetricsChannel
	{
		std::string tag;
		std::string botName;
	};

	class Configuration
	{
	public:
		static Configuration& Get()
		{
			static Configuration instance;
			return instance;
		}

		void Load();
		void LoadConfig();
		void CheckMapCompatibility(const std::string& mapName);

		[[nodiscard]] const std::string& MysqlHost() const noexcept { return m_mysqlHost; }
		[[nodiscard]] const std::string& MysqlUser() const noexcept { return m_mysqlUser; }
		[[nodiscard]] const std::string& MysqlPass() const noexcept { return m_mysqlPass; }
		[[nodiscard]] const std::string& MysqlDB() const noexcept { return m_mysqlDB; }
		[[nodiscard]] bool EnableMetrics() const noexcept { return m_enableMetrics; }
		[[nodiscard]] Optional<MetricsChannel> GetMetricsChannelInfo(const std::string& channelName) const;
		[[nodiscard]] int RandomBoxesRepeatInterval() const noexcept { return m_repeatBoxesInterval; }
		[[nodiscard]] bool RandomBoxesContainsBox(const std::string& box) const noexcept { return std::find(m_randomBoxes.begin(), m_randomBoxes.end(), box) != m_randomBoxes.end(); }
		[[nodiscard]] bool IsDinoAllowed(const std::string& mapName, const std::string blueprint) const noexcept;
		[[nodiscard]] const FString& GetChatCommand(const std::string& key) const { return m_chatCommands.find(key)->second; }
		[[nodiscard]] const FString& GetConsoleCommand(const std::string& key) const { return m_consoleCommands.find(key)->second; }
		[[nodiscard]] const FString& GetRconCommand(const std::string& key) const { return m_rconCommands.find(key)->second; }
		[[nodiscard]] const FString& GetText(const std::string& key) const { return m_messages.find(key)->second; }
		[[nodiscard]] std::shared_ptr<Box> GetBox(const std::string& boxName) const;
		[[nodiscard]] const BoxesList& GetBoxesList() const noexcept { return m_boxes; }

	private:
		void m_ParseMetricsParams(const Json& container);
		void m_ParseRandomBoxesParams(const Json& container);
		void m_ParseDinoExclusionsParams(const Json& container);
		void m_ParseBoxes(const Json& container);
		std::shared_ptr<Box> m_ParseBox(std::string name, const JsonPointer& ptr, const Json& container);
		std::shared_ptr<BoxElementBase> m_ParseItemElement(const JsonPointer& ptr, const Json& container);
		std::shared_ptr<BoxElementBase> m_ParseDinoElement(const JsonPointer& ptr, const Json& container);
		std::shared_ptr<BoxElementBase> m_ParseBeaconElement(const JsonPointer& ptr, const Json& container);
		std::shared_ptr<BoxElementBase> m_ParseCommandElement(const JsonPointer& ptr, const Json& container);
		std::shared_ptr<BoxElementBase> m_ParsePointsElement(const JsonPointer& ptr, const Json& container);
		std::shared_ptr<Item> m_ParseItem(const JsonPointer& ptr, const Json& container);
		std::shared_ptr<Dino> m_ParseDino(const JsonPointer& ptr, const Json& container);

		FString m_ParseFString(const JsonPointer& relativePath, const JsonPointer& absolutePath, const Json& container);
		std::string m_ParseString(const JsonPointer& relativePath, const JsonPointer& absolutePath, const Json& container);
		int m_ParseInt(const JsonPointer& relativePath, const JsonPointer& absolutePath, const Json& container);
		bool m_ParseBool(const JsonPointer& relativePath, const JsonPointer& absolutePath, const Json& container);
		Optional<std::string> m_ParseStringOptional(const JsonPointer& relativePath, const JsonPointer& absolutePath, const Json& container);
		Optional<int> m_ParseIntOptional(const JsonPointer& relativePath, const JsonPointer& absolutePath, const Json& container);
		Optional<bool> m_ParseBoolOptional(const JsonPointer& relativePath, const JsonPointer& absolutePath, const Json& container);

		Json container;
		std::string m_mysqlHost;
		std::string m_mysqlUser;
		std::string m_mysqlPass;
		std::string m_mysqlDB;
		bool m_enableMetrics;
		std::map<std::string, MetricsChannel> m_metricsChannels;
		int m_repeatBoxesInterval;
		std::vector<std::string> m_randomBoxes;
		std::map<std::string, std::vector<std::string>> m_dinoExclusions;
		std::map<std::string, FString> m_chatCommands;
		std::map<std::string, FString> m_consoleCommands;
		std::map<std::string, FString> m_rconCommands;
		std::map<std::string, FString> m_messages;
		BoxesList m_boxes;
		bool m_mapCompatibilityCheckDone = false;
	};

}

#endif // !_LOOTBOX_CONFIGURATION_H_
