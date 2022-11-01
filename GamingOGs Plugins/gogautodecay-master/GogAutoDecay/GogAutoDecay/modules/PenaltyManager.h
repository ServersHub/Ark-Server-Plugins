#pragma once

#include <json.hpp>

#include "../storage/PlayerProps.h"
#include "../storage/TeamProps.h"
#include "../external/LootBoxInterface.h"

namespace AutoDecay
{
	class Storage;

	struct PointsPenaltiesSettings
	{
		int min_day;
		int max_day;
		int base_penalty;
		int step;
	};

	struct BoxesPenaltiesSettings
	{
		int min_day;
		int max_day;
		int base_penalty;
		int step;
		std::vector<std::string> boxes;
	};

	class PenaltyManager
	{
	public:
		PenaltyManager() = delete;
		~PenaltyManager() = default;

		PenaltyManager(const nlohmann::basic_json<>& j, Storage* storage);
		void ProcessTeam(Team& team);
		void OnTeamOnline(Team& team);

	private:
		void HandlePoints(Team& team, int days_offline);
		void HandleBoxes(Team& team, int days_offline);
		int OfflineDaysAmount(Team& team);
		int PenalizePointsAmount(int days_offline);
		int PenalizeBoxesAmount(int days_offline);
		int TryPenalizePoints(Player& player, int amount_to_penalize);
		int TryPenalizeBoxes(Player& player, int amount_to_penalize);
		TArray<Boxes::BoxData> GetBoxes(Player& player);

		Storage* storage_;
		PointsPenaltiesSettings points_settings_;
		BoxesPenaltiesSettings boxes_settings_;
		bool points_enabled_;
		bool boxes_enabled_;
	};
}