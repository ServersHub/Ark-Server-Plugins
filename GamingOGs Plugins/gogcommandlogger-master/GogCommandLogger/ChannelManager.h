#ifndef _GOG_COMMAND_LOGGER_CHANNEL_MANAGER_H_
#define _GOG_COMMAND_LOGGER_CHANNEL_MANAGER_H_

#include <string>
#include <vector>
#include <memory>
#include <future>
#include <API/UE/Containers/FString.h>

#include "Config.h"

namespace CommandLogger
{
	enum class CommandType
	{
		Chat	= 0,
		Console = 1,
		Rcon	= 2
	};

	inline bool CompareCommandIdentifier(const FString& command, const FString& etalon)
	{
		TArray<FString> parsed;
		command.ParseIntoArray(parsed, L" ");

		if (parsed.IsValidIndex(0))
			return parsed[0] == etalon;

		return false;
	}

	class DiscordChannelData
	{
	public:
		DiscordChannelData(const std::string& tag, const std::string& bot_name)
			: tag_(tag), bot_name_(bot_name)
		{
		}

		std::string GetTag() const				{ return tag_; }
		std::string GetBotName() const			{ return bot_name_; }

		void SetTag(const std::string& val)		{ tag_ = val; }
		void SetBotName(const std::string& val) { bot_name_ = val; }

	private:
		std::string tag_		= "";
		std::string bot_name_	= "";
	};

	class Channel
	{
	public:
		Channel() = delete;
		~Channel() = default;

		Channel(std::shared_ptr<DiscordChannelData> chan_data)
			: discord_data_(chan_data)
		{
		}

		std::shared_ptr<DiscordChannelData> GetDiscordData()
		{
			return discord_data_;
		}

		bool ContainsCommand(const FString& command)
		{
			for (const auto& cmd : commands_) {
				if (CompareCommandIdentifier(command, cmd))
					return true;
			}

			return false;
		}

		void InsertCommand(const FString& command)
		{
			commands_.push_back(command);
		}

	private:
		std::shared_ptr<DiscordChannelData> discord_data_;
		std::vector<FString> commands_;
	};

	class ChannelGroup
	{
	public:
		ChannelGroup() = delete;
		~ChannelGroup() = default;

		ChannelGroup(std::unique_ptr<Channel> && chat_chan, std::unique_ptr<Channel> && console_chan,
			std::unique_ptr<Channel> && rcon_chan, const std::string& identifier)
			: chat_channel_(std::move(chat_chan)), console_channel_(std::move(console_chan)),
			rcon_channel_(std::move(rcon_chan)), identifier_(identifier)
		{
		}

		bool IsCommandPresent(const FString& command, CommandType type)
		{
			switch (type)
			{
			case CommandType::Chat:
				return chat_channel_->ContainsCommand(command);
			case CommandType::Console:
				return console_channel_->ContainsCommand(command);
			case CommandType::Rcon:
				return rcon_channel_->ContainsCommand(command);
			default:
				return false;
			}
		}

		std::shared_ptr<DiscordChannelData> GetDiscordChannelData(CommandType type)
		{
			switch (type)
			{
			case CommandType::Chat:
				return chat_channel_->GetDiscordData();
			case CommandType::Console:
				return console_channel_->GetDiscordData();
			case CommandType::Rcon:
				return rcon_channel_->GetDiscordData();
			default:
				return nullptr;
			}
		}

		std::string GetIdentifier() const
		{
			return identifier_;
		}

	private:
		std::unique_ptr<Channel> chat_channel_;
		std::unique_ptr<Channel> console_channel_;
		std::unique_ptr<Channel> rcon_channel_;
		std::string identifier_;
	};

	class ChannelManager
	{
	public:
		ChannelManager() = delete;
		~ChannelManager() = default;

		ChannelManager(nlohmann::json& json_entry)
		{
			for (auto& channel_group_entry : json_entry)
				channel_groups_.push_back(ParseChannelGroup(channel_group_entry));
		}

		std::shared_ptr<DiscordChannelData> FindChannelByCommand(const FString& command, CommandType type)
		{
			std::vector<std::future<std::shared_ptr<DiscordChannelData>>> results{};

			auto cbk = [&](ChannelGroup* chan_group) -> std::shared_ptr<DiscordChannelData>
			{
				if (chan_group->IsCommandPresent(command, type))
					return chan_group->GetDiscordChannelData(type);
				else
					return nullptr;
			};

			for (auto& chan_group : channel_groups_)
				results.push_back(std::async(std::launch::async, cbk, chan_group.get()));

			std::shared_ptr<DiscordChannelData> out = nullptr;

			for (auto& result : results) {
				if (!out)
					out = result.get();
			}

			return out;
		}

		std::shared_ptr<DiscordChannelData> FindChannelByIdentifier(const std::string& identifier, CommandType type)
		{
			auto search_pred = [&identifier](const std::unique_ptr<ChannelGroup>& chan_group)
			{
				return chan_group->GetIdentifier() == identifier;
			};

			const auto it = std::find_if(channel_groups_.begin(), channel_groups_.end(), search_pred);

			if (it != channel_groups_.end())
				return (*it)->GetDiscordChannelData(type);
			else
				return nullptr;
		}

	private:
		std::unique_ptr<ChannelGroup> ParseChannelGroup(nlohmann::json& json_entry)
		{
			const std::string identifier = json_entry["Identifier"];

			std::unique_ptr<Channel> chat_chan = ParseChannel(json_entry["Chat"]);
			std::unique_ptr<Channel> console_chan = ParseChannel(json_entry["Console"]);
			std::unique_ptr<Channel> rcon_chan = ParseChannel(json_entry["Rcon"]);

			std::unique_ptr<ChannelGroup> chan_group =
				std::make_unique<ChannelGroup>(std::move(chat_chan), std::move(console_chan), std::move(rcon_chan), identifier);

			return chan_group;
		}

		std::unique_ptr<Channel> ParseChannel(nlohmann::json& json_entry)
		{
			const std::string tag = json_entry["ChannelTag"];
			const std::string bot_name = json_entry["BotName"];

			std::shared_ptr<DiscordChannelData> discord_channel_data = std::make_shared<DiscordChannelData>(tag, bot_name);

			std::unique_ptr<Channel> channel = std::make_unique<Channel>(discord_channel_data);

			for (const auto& command : json_entry["Commands"])
				channel->InsertCommand(FString(static_cast<std::string>(command)));

			return channel;
		}

		std::vector<std::unique_ptr<ChannelGroup>> channel_groups_;
	};

	class CommandExceptionsManager
	{
	public:
		CommandExceptionsManager() = delete;
		~CommandExceptionsManager() = default;

		CommandExceptionsManager(nlohmann::json& json_entry)
		{
			ParseNode(json_entry["IgnoreChatCommands"], ignored_chat_commands_);
			ParseNode(json_entry["IgnoreConsoleCommands"], ignored_console_commands_);
			ParseNode(json_entry["IgnoreRconCommands"], ignored_rcon_commands_);
			ParseNode(json_entry["BlacklistChatCommands"], blacklisted_chat_commands_);
			ParseNode(json_entry["BlacklistConsoleCommands"], blacklisted_console_commands_);
			ParseNode(json_entry["BlacklistRconCommands"], blacklisted_rcon_commands_);
		}

		bool IsCommandIgnored(const FString& command, CommandType type)
		{
			switch (type)
			{
			case CommandType::Chat:
				return IsCommandExists(command, ignored_chat_commands_);
			case CommandType::Console:
				return IsCommandExists(command, ignored_console_commands_);
			case CommandType::Rcon:
				return IsCommandExists(command, ignored_rcon_commands_);
			default:
				return false;
			}
		}

		bool IsCommandBlacklisted(const FString& command, CommandType type)
		{
			switch (type)
			{
			case CommandType::Chat:
				return IsCommandExists(command, blacklisted_chat_commands_);
			case CommandType::Console:
				return IsCommandExists(command, blacklisted_console_commands_);
			case CommandType::Rcon:
				return IsCommandExists(command, blacklisted_rcon_commands_);
			default:
				return false;
			}
		}

	private:
		void ParseNode(nlohmann::json& json_entry, std::vector<FString>& out)
		{
			for (const auto& command : json_entry)
			{
				out.push_back(FString(static_cast<std::string>(command)));
			}
		}

		bool IsCommandExists(const FString& command, std::vector<FString>& cache)
		{
			auto search_pred = [&command](const FString& item)
			{
				return CompareCommandIdentifier(command, item);
			};

			const auto it = std::find_if(cache.begin(), cache.end(), search_pred);

			return it != cache.end();
		}

		std::vector<FString> ignored_chat_commands_;
		std::vector<FString> ignored_console_commands_;
		std::vector<FString> ignored_rcon_commands_;
		std::vector<FString> blacklisted_chat_commands_;
		std::vector<FString> blacklisted_console_commands_;
		std::vector<FString> blacklisted_rcon_commands_;
	};

	inline std::unique_ptr<ChannelManager> ChanManager = nullptr;
	inline std::unique_ptr<CommandExceptionsManager> IgnoreList = nullptr;

	inline bool Initialize()
	{
		try {
			ChanManager = std::make_unique<ChannelManager>(std::ref(GetConfig()["ChannelGroups"]));
			IgnoreList = std::make_unique<CommandExceptionsManager>(std::ref(GetConfig()));
		}
		catch (const std::exception&) {
			return false;
		}

		return true;
	}
}

#endif // !_GOG_COMMAND_LOGGER_CHANNEL_MANAGER_H_
