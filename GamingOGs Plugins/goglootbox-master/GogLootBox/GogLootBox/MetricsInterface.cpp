#include "MetricsInterface.h"

#include <memory>
#include <Permissions/ArkPermissions.h>

namespace LootBox::Metrics
{

	void MetricsInterface::LogOpenBox(std::shared_ptr<Box> box, std::shared_ptr<BoxElementBase> element, std::shared_ptr<Player> player)
	{
		if (!m_enabled)
		{
			return;
		}

		const auto channel = Configuration::Get().GetMetricsChannelInfo("OpenBoxesChannel");
		if (!channel.has_value())
		{
			Log::GetLog()->error("Metrics channel 'OpenBoxesChannel' is not specified in config.json, Log operation was not performed");
			return;
		}

		const std::string ranks = FormatPlayerGroups(player->SteamId());
		const std::string date = DiscordMetrics::MakeTimeString();

		std::string received{};
		switch (element->Type())
		{
		case BoxElementType::Item:
			received = FormatItemBox(std::static_pointer_cast<BoxElementItem>(element));
			break;
		case BoxElementType::Dino:
			received = FormatDinoBox(std::static_pointer_cast<BoxElementDino>(element));
			break;
		case BoxElementType::Beacon:
			received = FormatBeaconBox(std::static_pointer_cast<BoxElementBeacon>(element));
			break;
		case BoxElementType::Points:
			received = FormatPointsBox(std::static_pointer_cast<BoxElementPoints>(element));
			break;
		case BoxElementType::Command:
			received = FormatCommandBox(std::static_pointer_cast<BoxElementCommand>(element));
			break;
		}

		const std::string out = fmt::format(
			openBoxTemplate,
			player->SteamName(),
			player->CharacterName(),
			player->SteamId(),
			ranks,
			date,
			box->Name(),
			received);
		QueuePacket(std::move(out), channel.value());
	}

	void MetricsInterface::LogTradeBoxes(std::shared_ptr<Box> box, int count, std::shared_ptr<Player> player)
	{
		if (!m_enabled)
		{
			return;
		}

		const auto channel = Configuration::Get().GetMetricsChannelInfo("TradeBoxesChannel");
		if (!channel.has_value())
		{
			Log::GetLog()->error("Metrics channel 'TradeBoxesChannel' is not specified in config.json, Log operation was not performed");
			return;
		}

		const std::string ranks = FormatPlayerGroups(player->SteamId());
		const std::string date = DiscordMetrics::MakeTimeString();

		const std::string out = fmt::format(
			tradeBoxTemplate,
			player->SteamName(),
			player->CharacterName(),
			player->SteamId(),
			ranks,
			date,
			box->Name(),
			count);
		QueuePacket(std::move(out), channel.value());
	}

	void MetricsInterface::LogReceiveBoxes(std::shared_ptr<Box> box, int count, std::shared_ptr<Player> player)
	{
		if (!m_enabled)
		{
			return;
		}

		const auto channel = Configuration::Get().GetMetricsChannelInfo("ReceiveBoxesChannel");
		if (!channel.has_value())
		{
			Log::GetLog()->error("Metrics channel 'ReceiveBoxesChannel' is not specified in config.json, Log operation was not performed");
			return;
		}

		const std::string ranks = FormatPlayerGroups(player->SteamId());
		const std::string date = DiscordMetrics::MakeTimeString();

		const std::string out = fmt::format(
			receiveBoxTemplate,
			player->SteamName(),
			player->CharacterName(),
			player->SteamId(),
			ranks,
			date,
			box->Name(),
			count);
		QueuePacket(std::move(out), channel.value());
	}

	void MetricsInterface::QueuePacket(std::string msg, const MetricsChannel& channel)
	{
		DiscordMetrics::DiscordPacket packet(channel.tag, channel.botName, std::move(msg));
		DiscordMetrics::QueuePacket(packet);
	}

	std::string MetricsInterface::FormatPlayerGroups(uint64 steamId)
	{
		std::string groups{};
		TArray<FString> playerGroups = Permissions::GetPlayerGroups(steamId);
		for (const auto& group : playerGroups)
			groups += group.ToString() + ", ";
		if (!groups.empty())
			groups.erase(groups.size() - 2, 2);
		return groups;
	}

	std::string MetricsInterface::FormatItemBox(std::shared_ptr<BoxElementItem> element)
	{
		std::string out{};
		for (const auto& item : element->Items())
		{
			out += fmt::format(
				"Item (BP - {}, Amount - {}, Quality - from {} to {});\\n",
				item->Blueprint(),
				item->Amount(),
				item->MinQuality(),
				item->MaxQuality());
		}
		return out;
	}

	std::string MetricsInterface::FormatDinoBox(std::shared_ptr<BoxElementDino> element)
	{
		std::string out{};
		for (const auto& dino : element->Dinos())
		{
			out += fmt::format(
				"Dino (BP - {}, SaddleBP - {}, Level - {});\\n",
				dino->Blueprint(),
				dino->SaddleBlueprint().has_value() ? dino->SaddleBlueprint().value() : "None",
				dino->Level());
		}
		return out;
	}

	std::string MetricsInterface::FormatBeaconBox(std::shared_ptr<BoxElementBeacon> element)
	{
		return fmt::format("Beacon (ClassName - {});\\n", element->ClassName());
	}

	std::string MetricsInterface::FormatPointsBox(std::shared_ptr<BoxElementPoints> element)
	{
		return fmt::format("Points (Amount from {} to {});\\n", element->MinAmount(), element->MaxAmount());
	}

	std::string MetricsInterface::FormatCommandBox(std::shared_ptr<BoxElementCommand> element)
	{
		return fmt::format("Command (Syntax - {});\\n", element->Command());
	}

}