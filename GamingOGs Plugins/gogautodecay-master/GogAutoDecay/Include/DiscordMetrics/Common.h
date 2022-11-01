#ifndef _GOG_DISCORD_METRICS_COMMON_H_
#define _GOG_DISCORD_METRICS_COMMON_H_

#ifdef GOGDISCORDMETRICS_EXPORTS
	#define GOGDISCORDMETRICS_API __declspec(dllexport)
#else
	#define GOGDISCORDMETRICS_API __declspec(dllimport)
#endif

#include <string>

namespace DiscordMetrics
{
	struct DiscordPacket
	{
		DiscordPacket()
			: channel_tag(""), bot_name(""), message("")
		{
		}

		DiscordPacket(const std::string& channel, const std::string& bot, const std::string& msg)
			: channel_tag(channel), bot_name(bot), message(msg)
		{
		}

		std::string channel_tag;
		std::string bot_name;
		std::string message;
	};
}

#endif // !_GOG_DISCORD_METRICS_COMMON_H_
