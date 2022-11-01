#ifndef _LOOTBOX_METRICS_INTERFACE_H_
#define _LOOTBOX_METRICS_INTERFACE_H_

#include <string>
#include <DiscordMetrics/GogDiscordMetrics.h>

#include "Configuration.h"
#include "Box.h"
#include "Player.h"

namespace LootBox::Metrics
{

	const std::string openBoxTemplate =
		"```Player has opened the box:\\n"
		"Steam Name: {}\\n"
		"Character Name: {}\\n"
		"SteamID: {}\\n"
		"Ranks: {}\\n"
		"Date / Time: {}\\n"
		"Box Name: {}\\n"
		"Items Earned: {}```";
	const std::string tradeBoxTemplate =
		"```Player has traded the box(es):\\n"
		"Steam Name: {}\\n"
		"Character Name: {}\\n"
		"SteamID: {}\\n"
		"Ranks: {}\\n"
		"Date / Time: {}\\n"
		"Box Name: {}\\n"
		"Boxes Amount: {}```";
	const std::string receiveBoxTemplate =
		"```Player has received the box(es):\\n"
		"Steam Name: {}\\n"
		"Character Name: {}\\n"
		"SteamID: {}\\n"
		"Ranks: {}\\n"
		"Date / Time: {}\\n"
		"Box Name: {}\\n"
		"Boxes Amount: {}```";

	class MetricsInterface
	{
	public:
		MetricsInterface() : m_enabled(false)
		{
		}

		static MetricsInterface& Get()
		{
			static MetricsInterface instance;
			return instance;
		}

		void Enable()
		{
			m_enabled = true;
		}

		void LogOpenBox(std::shared_ptr<Box> box, std::shared_ptr<BoxElementBase> element, std::shared_ptr<Player> player);
		void LogTradeBoxes(std::shared_ptr<Box> box, int count, std::shared_ptr<Player> player);
		void LogReceiveBoxes(std::shared_ptr<Box> box, int count, std::shared_ptr<Player> player);

	private:
		void QueuePacket(std::string msg, const MetricsChannel& channel);
		std::string FormatPlayerGroups(uint64 steamId);
		std::string FormatItemBox(std::shared_ptr<BoxElementItem> element);
		std::string FormatDinoBox(std::shared_ptr<BoxElementDino> element);
		std::string FormatBeaconBox(std::shared_ptr<BoxElementBeacon> element);
		std::string FormatPointsBox(std::shared_ptr<BoxElementPoints> element);
		std::string FormatCommandBox(std::shared_ptr<BoxElementCommand> element);

		bool m_enabled;
	};

}

#endif // !_LOOTBOX_METRICS_INTERFACE_H_
