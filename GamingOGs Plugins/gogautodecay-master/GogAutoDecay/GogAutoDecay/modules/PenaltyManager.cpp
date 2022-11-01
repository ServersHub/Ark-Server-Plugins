#include "PenaltyManager.h"

#include "../storage/Storage.h"
#include "../external/ShopInterface.h"
#include "../external/LootBoxInterface.h"

namespace AutoDecay
{
	PenaltyManager::PenaltyManager(const nlohmann::basic_json<>& j, Storage* storage)
		: storage_(storage), points_enabled_(false), boxes_enabled_(false)
	{
		points_enabled_ = j["Points"]["Enabled"];
		boxes_enabled_ = j["Boxes"]["Enabled"];

		if (points_enabled_)
		{
			points_settings_.min_day = j["Points"]["MinDay"];
			points_settings_.max_day = j["Points"]["MaxDay"];
			points_settings_.base_penalty = j["Points"]["BasePenalty"];
			points_settings_.step = j["Points"]["PenaltyStep"];
		}

		if (boxes_enabled_)
		{
			boxes_settings_.min_day = j["Boxes"]["MinDay"];
			boxes_settings_.max_day = j["Boxes"]["MaxDay"];
			boxes_settings_.base_penalty = j["Boxes"]["BasePenalty"];
			boxes_settings_.step = j["Boxes"]["PenaltyStep"];

			for (const auto& box_type : j["Boxes"]["Boxes"])
			{
				boxes_settings_.boxes.push_back(box_type.get<std::string>());
			}
		}
	}

	void PenaltyManager::ProcessTeam(Team& team)
	{
		int days_offline = OfflineDaysAmount(team);
		
		if (points_enabled_)
		{
			HandlePoints(team, days_offline);
		}

		if (boxes_enabled_)
		{
			HandleBoxes(team, days_offline);
		}
	}

	void PenaltyManager::OnTeamOnline(Team& team)
	{
		for (auto it = storage_->Players()->Begin(); it != storage_->Players()->End(); it++)
		{
			if ((*it)->GetTeamId() == team->GetTeamId())
			{
				(*it)->SetPointsOrdered(0);
				(*it)->SetBoxesOrdered(0);
			}
		}
	}

	void PenaltyManager::HandlePoints(Team& team, int days_offline)
	{
		if (days_offline >= points_settings_.min_day && days_offline <= points_settings_.max_day)
		{
			int points_to_penalize = PenalizePointsAmount(days_offline);

			for (auto it = storage_->Players()->Begin(); it != storage_->Players()->End(); it++)
			{
				auto player = *it;
				if (player->GetTeamId() == team->GetTeamId() && player->GetPointsOrdered() < points_to_penalize)
				{
					int penalized = TryPenalizePoints(player, points_to_penalize - player->GetPointsOrdered());
					player->SetPointsOrdered(player->GetPointsOrdered() + penalized);
				}
			}
		}
	}

	void PenaltyManager::HandleBoxes(Team& team, int days_offline)
	{
		if (days_offline >= boxes_settings_.min_day && days_offline <= boxes_settings_.max_day)
		{
			int boxes_to_penalize = PenalizeBoxesAmount(days_offline);

			for (auto it = storage_->Players()->Begin(); it != storage_->Players()->End(); it++)
			{
				auto player = *it;
				if (player->GetTeamId() == team->GetTeamId() && player->GetBoxesOrdered() < boxes_to_penalize)
				{
					int penalized = TryPenalizeBoxes(player, boxes_to_penalize - player->GetBoxesOrdered());
					player->SetBoxesOrdered(player->GetPointsOrdered() + penalized);
				}
			}
		}
	}

	int PenaltyManager::OfflineDaysAmount(Team& team)
	{
		if (team->GetLastSeenTime() == 0)
		{
			return 0;
		}

		__int64 seconds = time(0) - team->GetLastSeenTime();
		return static_cast<int>(seconds / 60 / 60 / 24);
	}

	int PenaltyManager::PenalizePointsAmount(int days_offline)
	{
		int result = 0;
		int penalized_days = days_offline - points_settings_.min_day + 1;

		std::function<int(const PointsPenaltiesSettings&, int, int)> f;
		f = [&f](const PointsPenaltiesSettings& s, int result, int penalized_days) -> int
		{
			if (penalized_days == 1)
			{
				return result + s.base_penalty;
			}
			else
			{
				result += s.base_penalty;
				result += s.step * (penalized_days - 1);
				return f(s, result, --penalized_days);
			}
		};

		return f(points_settings_, result, penalized_days);
	}

	int PenaltyManager::PenalizeBoxesAmount(int days_offline)
	{
		int result = 0;
		int penalized_days = days_offline - boxes_settings_.min_day + 1;

		std::function<int(const BoxesPenaltiesSettings&, int, int)> f;
		f = [&f](const BoxesPenaltiesSettings& s, int result, int penalized_days) -> int
		{
			if (penalized_days == 1)
			{
				return result + s.base_penalty;
			}
			else
			{
				result += s.base_penalty;
				result += s.step * (penalized_days - 1);
				return f(s, result, --penalized_days);
			}
		};

		return f(boxes_settings_, result, penalized_days);
	}

	int PenaltyManager::TryPenalizePoints(Player& player, int amount_to_penalize)
	{
		int players_points = Shop::GetPoints(player->GetSteamId());

		if(players_points >= amount_to_penalize)
		{
			Shop::RemovePoints(player->GetSteamId(), amount_to_penalize);
			return amount_to_penalize;
		}
		else
		{
			Shop::RemovePoints(player->GetSteamId(), players_points);
			return players_points;
		}
	}

	int PenaltyManager::TryPenalizeBoxes(Player& player, int amount_to_penalize)
	{
		int result = 0;
		TArray<Boxes::BoxData> player_boxes = GetBoxes(player);

		auto penalize_box = [&player](const Boxes::BoxData& box, int amount_to_penalize) -> int
		{
			if (box.amount >= amount_to_penalize)
			{
				Boxes::RemoveBoxes(player->GetSteamId(), box.box_name, amount_to_penalize);
				return amount_to_penalize;
			}
			else
			{
				Boxes::RemoveBoxes(player->GetSteamId(), box.box_name, box.amount);
				return box.amount;
			}
		};

		for (const Boxes::BoxData& box : player_boxes)
		{
			result += penalize_box(box, amount_to_penalize - result);
			if (result >= amount_to_penalize)
			{
				break;
			}
		}

		return result;
	}

	TArray<Boxes::BoxData> PenaltyManager::GetBoxes(Player& player)
	{
		TArray<Boxes::BoxData> boxes = Boxes::GetPlayerBoxes(player->GetSteamId());
		TArray<Boxes::BoxData> out{};

		for (auto& item : boxes)
		{
			if (std::find(boxes_settings_.boxes.begin(),
						  boxes_settings_.boxes.end(),
						  item.box_name.ToString()) != boxes_settings_.boxes.end())
			{
				out.Push(item);
			}
		}

		return out;
	}
}