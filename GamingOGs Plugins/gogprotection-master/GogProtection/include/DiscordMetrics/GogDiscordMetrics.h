#ifndef _GOG_DISCORD_METRICS_H_
#define _GOG_DISCORD_METRICS_H_

#include "Common.h"

namespace DiscordMetrics
{
	void GOGDISCORDMETRICS_API QueuePacket(const DiscordPacket& packet);

	std::string GOGDISCORDMETRICS_API MakeTimeString();

	void GOGDISCORDMETRICS_API EscapeString(std::string& string);
}

#endif // !_GOG_DISCORD_METRICS_H_
