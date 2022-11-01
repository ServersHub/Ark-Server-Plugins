#pragma once

#include "Platform.h"

#include <json.hpp>

namespace BountyHunter
{
	class RewardManager
	{
	public:
		RewardManager(const nlohmann::json& in, AShooterPlayerController* player, const std::string& target_name)
			: config_(in), player_(player), target_name_(target_name)
		{
			steam_id_ = ArkApi::GetApiUtils().GetSteamIdFromController(player);
			FString killer_name = ArkApi::GetApiUtils().GetCharacterName(player);
			killer_name_ = killer_name.ToString();
		}

		void RewardPlayer()
		{
			if (config_["RandomReward"].get<bool>())
			{
				RewardRandom();
			}
			else
			{
				RewardAll();
			}
		}

	private:
		void Notify(const std::string& type)
		{
			ArkApi::GetApiUtils().SendChatMessageToAll(Msg("Sender"), *Msg("Rewarded"), killer_name_, type, target_name_);
		}

		void RewardRandom()
		{
			int iterations = Platform::RandNumber(
				config_["Min"].get<int>(), config_["Max"].get<int>()
			);

			while (iterations--)
			{
				std::vector<nlohmann::json> enabled_rewards{};
				for (const auto& item : config_["Data"])
				{
					if (item["Enabled"].get<bool>())
					{
						enabled_rewards.push_back(item);
					}
				}

				int index;
				if (enabled_rewards.size() == 1)
				{
					index = 0;
				}
				else
				{
					index = Platform::RandNumber(0, enabled_rewards.size() - 1);
				}

				HandleTypeNode(enabled_rewards[index]);
			}
		}

		void RewardAll()
		{
			for (const auto& item : config_["Data"])
			{
				if (item["Enabled"].get<bool>())
				{
					HandleTypeNode(item);
				}
			}
		}

		void HandleTypeNode(const nlohmann::json& j)
		{
			if (j["Type"].get<std::string>() == "Points")
			{
				HandlePoints(j);
				Notify("Point(s)");
			}
			else if (j["Type"].get<std::string>() == "Commands")
			{
				HandleCommands(j);
				Notify("Command(s)");
			}
			else if (j["Type"].get<std::string>() == "Dinos")
			{
				HandleDinos(j);
				Notify("Dino(s)");
			}
			else if (j["Type"].get<std::string>() == "Items")
			{
				HandleItems(j);
				Notify("Item(s)");
			}
			else if (j["Type"].get<std::string>() == "Boxes")
			{
				HandleLootBoxes(j);
				Notify("LootBox(s)");
			}
		}

		void HandlePoints(const nlohmann::json& j)
		{
			const int amount = Platform::RandNumber(j["Min"].get<int>(), j["Max"].get<int>());
			Platform::GivePoints(amount, steam_id_);
		}

		void HandleCommands(const nlohmann::json& j)
		{
			int iterations = Platform::RandNumber(j["Min"].get<int>(), j["Max"].get<int>());
			while (iterations--)
			{
				int index;
				if (j["Commands"].size() == 1)
				{
					index = 0;
				}
				else
				{
					index = Platform::RandNumber(0, j["Commands"].size() - 1);
				}

				const std::string command = j["Commands"][index].get<std::string>();

				FString result;
				FString fcommand = fmt::format(command, steam_id_).c_str();
				player_->ConsoleCommand(&result, &fcommand, false);
			}
		}

		void HandleLootBoxes(const nlohmann::json& j)
		{
			int iterations = Platform::RandNumber(j["Min"].get<int>(), j["Max"].get<int>());
			while (iterations--)
			{
				int index;
				if (j["Boxes"].size() == 1)
				{
					index = 0;
				}
				else
				{
					index = Platform::RandNumber(0, j["Boxes"].size() - 1);
				}

				const std::string box_name = j["Boxes"][index].get<std::string>();
				GogLootBox::GiveBoxes(steam_id_, box_name, 1, "Bounty Hunter Reward", "GogBountyHunter");
			}
		}

		void HandleDinos(const nlohmann::json& j)
		{
			int iterations = Platform::RandNumber(j["Min"].get<int>(), j["Max"].get<int>());
			while (iterations--)
			{
				int index;
				if (j["Dinos"].size() == 1)
				{
					index = 0;
				}
				else
				{
					index = Platform::RandNumber(0, j["Dinos"].size() - 1);
				}

				const auto& dino_conf = j["Dinos"][index];

				const int level = dino_conf["Level"].get<int>();
				const bool is_baby = dino_conf["BabyDino"].get<bool>();
				const float imprint_amount = dino_conf["ImprintAmount"].get<float>();
				const bool neutered = dino_conf["Neutered"].get<bool>();
				const std::string bp = dino_conf["Blueprint"].get<std::string>();
				const std::string saddle_bp = dino_conf["SaddleBlueprint"].get<std::string>();
				const float saddle_queality = dino_conf["SaddleQuality"].get<float>();

				APrimalDinoCharacter* dino_spawn = ArkApi::GetApiUtils().SpawnDino(
					player_, FString(bp), nullptr, level, true, neutered);

				if (dino_spawn && is_baby)
				{
					dino_spawn->SetBabyAge(0.0);
					dino_spawn->RefreshBabyScaling();
				}

				if (dino_spawn && imprint_amount > 0.0f && !ArkApi::GetApiUtils().IsPlayerDead(player_))
				{
					AShooterCharacter* player_character = static_cast<AShooterCharacter*>(player_->CharacterField());
					if (player_character)
					{
						FString char_name = ArkApi::GetApiUtils().GetCharacterName(player_);

						UPrimalPlayerData* primal_data = player_character->GetPlayerData();
						if (primal_data)
						{
							FPrimalPlayerDataStruct* fprimal_data = primal_data->MyDataField();
							if (fprimal_data)
							{
								uint64 data_id = fprimal_data->PlayerDataIDField();
								if (!char_name.IsEmpty() && data_id > 0)
								{
									float calculated_amount = imprint_amount / 100.0f;

									dino_spawn->UpdateImprintingDetails(&char_name, data_id);
									dino_spawn->UpdateImprintingQuality(calculated_amount > 1.0f ? 1.0f : calculated_amount);
								}
							}
						}
					}
				}

				if (!saddle_bp.empty())
				{
					Platform::GiveItem(player_, saddle_bp, 1, saddle_queality, false, 0);
				}
			}
		}

		void HandleItems(const nlohmann::json& j)
		{
			int index;
			if (j["Items"].size() == 1)
			{
				index = 0;
			}
			else
			{
				index = Platform::RandNumber(0, j["Items"].size() - 1);
			}

			const auto& item = j["Items"][index];

			const std::string bp = item["Blueprint"].get<std::string>();
			const bool force_bp = item["ForceBlueprint"].get<bool>();
			const int amount = Platform::RandNumber(item["Min"].get<int>(), item["Max"].get<int>());
			const float quality = item["Quality"].get<float>();

			Platform::GiveItem(player_, bp, amount, quality, force_bp, 0);
		}

		nlohmann::json config_;
		AShooterPlayerController* player_;
		uint64 steam_id_;
		std::string target_name_;
		std::string killer_name_;
	};
}