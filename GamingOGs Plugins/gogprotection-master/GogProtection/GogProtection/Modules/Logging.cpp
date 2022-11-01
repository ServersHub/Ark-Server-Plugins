#include "Logging.h"

#include <DiscordMetrics/GogDiscordMetrics.h>
#include <Logger/spdlog/formatter.h>

namespace Protection
{
	inline const std::string delim = "-=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=-";
	inline const std::string new_player_templ =
		"__**New player joined to server:**__"
		"**Time:** {}\\n"
		"**CharacterName:** {}\\n"
		"**SteamID:** {}\\n"
		"**Initial Time:** {}\\n"
		"{}";

	DiscordLogger::DiscordLogger(const DiscordChannelData& chan_data)
		: chan_data_(chan_data)
	{
	}

	DiscordLogger::~DiscordLogger()
	{
	}

	void DiscordLogger::LogNewPlayer(const FString& character_name, const FString& initial_time, unsigned long long steam_id)
	{
		if (IsMetricsEnabled())
		{
			const std::string current_date_time = DiscordMetrics::MakeTimeString();
			const std::string final_output = fmt::format(new_player_templ, current_date_time,
				character_name.ToString(), steam_id, initial_time.ToString(), delim);

			LogBase(final_output);
		}
	}

	void DiscordLogger::LogBase(const std::string& message)
	{
		DiscordMetrics::DiscordPacket packet(chan_data_.tag, chan_data_.bot_name, message);
		DiscordMetrics::QueuePacket(packet);
	}
}