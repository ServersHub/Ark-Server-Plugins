#include "BountyHunter.h"

#include <fstream>
#include <random>

#include "Reward.h"

namespace BountyHunter
{
	DECLARE_HOOK(AShooterGameMode_HandleNewPlayer_Implementation, bool, AShooterGameMode*, AShooterPlayerController*, UPrimalPlayerData*, AShooterCharacter*, bool);
	DECLARE_HOOK(AShooterGameMode_Logout, void, AShooterGameMode*, AController*);
	DECLARE_HOOK(AShooterCharacter_Die, bool, AShooterCharacter*, float, FDamageEvent*, AController*, AActor*);
	DECLARE_HOOK(AShooterPlayerState_ServerRequestLeaveTribe, void, AShooterPlayerState*);

	bool  Hook_AShooterGameMode_HandleNewPlayer_Implementation(AShooterGameMode* _this, AShooterPlayerController* new_player, UPrimalPlayerData* player_data, AShooterCharacter* player_character,
		bool is_from_login)
	{
		auto result = AShooterGameMode_HandleNewPlayer_Implementation_original(_this, new_player, player_data, player_character, is_from_login);

		if (new_player)
		{
			const uint64 steam_id = ArkApi::GetApiUtils().GetSteamIdFromController(reinterpret_cast<AController*>(new_player));
			if (target.steam_id_ == steam_id)
			{
				target.logout_time_ = 0;
			}
		}

		return result;
	}

	void Hook_AShooterGameMode_Logout(AShooterGameMode* _this, AController* exiting)
	{
		const uint64 steam_id = ArkApi::GetApiUtils().GetSteamIdFromController(exiting);
		if (target.steam_id_ == steam_id)
		{
			target.logout_time_ = time(0);
		}
		AShooterGameMode_Logout_original(_this, exiting);
	}

	bool Hook_AShooterCharacter_Die(AShooterCharacter* _this, float damage, FDamageEvent* damage_event, AController* event_instigator, AActor* damage_causer)
	{
		UPrimalPlayerData* player_data = _this->GetPlayerData();
		if (player_data) {
			const auto steam_id = ArkApi::GetApiUtils().GetSteamIDForPlayerID(ArkApi::GetApiUtils().GetPlayerID(_this));
			if (steam_id == target.steam_id_ && damage_causer && damage_causer->IsA(AShooterCharacter::GetPrivateStaticClass()) &&
				damage_causer->TargetingTeamField() != _this->TargetingTeamField() && event_instigator && event_instigator->IsA(AShooterPlayerController::GetPrivateStaticClass()))
			{
				const int target_team = _this->TargetingTeamField();
				const int damager_team = damage_causer->TargetingTeamField();

				const bool is_allied = ArkApi::GetApiUtils().GetShooterGameMode()->AreTribesAllied(target_team, damager_team);
				const bool cant_be_rewarded = penalized.find(ArkApi::GetApiUtils().GetSteamIdFromController(event_instigator)) != penalized.end();

				NotifyOnClaimTarget(reinterpret_cast<AShooterPlayerController*>(event_instigator));
				if (!is_allied && !cant_be_rewarded && target_team != damager_team)
				{
					RewardPlayer(reinterpret_cast<AShooterPlayerController*>(event_instigator));
				}
				PickTarget();
			}
		}
		return AShooterCharacter_Die_original(_this, damage, damage_event, event_instigator, damage_causer);
	}

	void  Hook_AShooterPlayerState_ServerRequestLeaveTribe(AShooterPlayerState* _this)
	{
		AShooterPlayerState_ServerRequestLeaveTribe_original(_this);
		int player_id = _this->PlayerIdField();
		const uint64 steam_id = ArkApi::GetApiUtils().GetSteamIDForPlayerID(player_id);
		penalized[steam_id] = time(0);
	}

	void CliBounty(AShooterPlayerController* shooter_controller, FString* message, int mode)
	{
		if (target.steam_id_ != 0)
		{
			AShooterPlayerController* controller = ArkApi::GetApiUtils().FindPlayerFromSteamId(target.steam_id_);

			if (controller)
			{
				const FString name = ArkApi::GetApiUtils().GetCharacterName(controller);

				if (json_conf["General"]["ShowPos"].get<bool>())
				{
					const FVector pos = ArkApi::GetApiUtils().GetPosition(controller);
					AWorldSettings* WorldSettings = ArkApi::GetApiUtils().GetWorld()->GetWorldSettings(false, true);
					APrimalWorldSettings* PWorldSettings = static_cast<APrimalWorldSettings*>(WorldSettings);

					float LatScale = PWorldSettings->LatitudeScaleField() != 0 ? PWorldSettings->LatitudeScaleField() : 800.0f;
					float LongScale = PWorldSettings->LongitudeScaleField() != 0 ? PWorldSettings->LongitudeScaleField() : 800.0f;
					float LatOrigin = PWorldSettings->LatitudeOriginField() != 0 ? PWorldSettings->LatitudeOriginField() : -400000.0f;
					float LongOrigin = PWorldSettings->LongitudeOriginField() != 0 ? PWorldSettings->LongitudeOriginField() : -400000.0f;

					float LatDiv = 100.f / LatScale;
					float Lat = (LatDiv * pos.Y + LatDiv * abs(LatOrigin)) / 1000.f;

					float LongDiv = 100.f / LongScale;
					float Long = (LongDiv * pos.X + LongDiv * abs(LongOrigin)) / 1000.f;

					ArkApi::GetApiUtils().SendChatMessage(shooter_controller, Msg("Sender"), *Msg("ShowBountyPos"),
						name.ToString(), std::floor(Lat * 10.) / 10., std::floor(Long * 10.) / 10.);
				}
				else
				{
					ArkApi::GetApiUtils().SendChatMessage(shooter_controller, Msg("Sender"), *Msg("ShowBounty"), name.ToString());
				}
			}
			else
			{
				ArkApi::GetApiUtils().SendChatMessage(shooter_controller, Msg("Sender"), *Msg("BountyOffline"));
			}
		}
		else
		{
			ArkApi::GetApiUtils().SendChatMessage(shooter_controller, Msg("Sender"), L"No bounty");
		}
	}

	void AdminReward(APlayerController* player_controller, FString* message, bool write_to_log)
	{
		RewardManager mgr(json_conf["Rewards"], reinterpret_cast<AShooterPlayerController*>(player_controller), "Tester");
		mgr.RewardPlayer();
	}

	void AdminSetBounty(APlayerController* player_controller, FString* message, bool write_to_log)
	{
		TArray<FString> parsed{};
		message->ParseIntoArray(parsed, L" ");

		if (parsed.IsValidIndex(1))
		{
			const uint64 steam_id = std::strtoull(parsed[1].ToString().c_str(), nullptr, 10);
			AShooterPlayerController* controller = ArkApi::GetApiUtils().FindPlayerFromSteamId(steam_id);

			if (!controller)
			{
				ArkApi::GetApiUtils().SendServerMessage(reinterpret_cast<AShooterPlayerController*>(player_controller),
					FColorList::Red, L"Requested player is not found or offline");
			}
			else
			{
				PickTarget(steam_id);
				ArkApi::GetApiUtils().SendServerMessage(reinterpret_cast<AShooterPlayerController*>(player_controller),
					FColorList::Red, L"Bounty player changed");
			}
		}
		else
		{
			ArkApi::GetApiUtils().SendServerMessage(reinterpret_cast<AShooterPlayerController*>(player_controller),
				FColorList::Red, L"Bad syntax");
		}
	}

	void TimerCallback()
	{
		const int penalty_time = json_conf["General"]["PenaltyTime"].get<int>() * 60;
		penalized.erase(std::find_if(penalized.begin(), penalized.end(),
			[&](const std::pair<uint64, time_t>& val)
			{
				return time(0) >= val.second + penalty_time;
			}), penalized.end());

		if (target.logout_time_ != 0)
		{
			const int allow_offline = json_conf["General"]["AllowOffline"].get<int>() * 60;
			const time_t target_offline = time(0) - target.logout_time_;

			if (target_offline > allow_offline)
			{
				PickTarget();
				return;
			}
		}

		if (target.marked_time_ != 0)
		{
			const int bounty_time = json_conf["General"]["BountyTime"].get<int>() * 60;
			const time_t target_alive = time(0) - target.marked_time_;

			if (target_alive > bounty_time)
			{
				PickTarget();
			}
		}
		else
		{
			PickTarget();
		}
	}

	std::string GetLicenseKey()
	{
		const std::string config_path = Platform::GetConfigPath();

		std::ifstream file{ config_path };

		if (!file.is_open())
		{
			throw std::runtime_error("Can't open config.json");
		}

		file >> json_conf;
		file.close();

		if (!json_conf.contains("/LicenseKey"_json_pointer))
		{
			throw std::runtime_error("Can't load license key");
		}

		return json_conf["/LicenseKey"_json_pointer].get<std::string>();
	}

	FString Msg(const std::string& title)
	{
		return FString(ArkApi::Tools::Utf8Decode(json_conf["Messages"][title].get<std::string>().c_str()));
	}

	FString Cmd(const std::string& title)
	{
		return FString(ArkApi::Tools::Utf8Decode(json_conf["Commands"][title].get<std::string>().c_str()));
	}

	void RewardPlayer(AShooterPlayerController* controller)
	{
		RewardManager mgr(json_conf["Rewards"], controller, target.target_name_);
		mgr.RewardPlayer();
	}

	void PickTarget(uint64 steam_id)
	{
		if (steam_id == 0)
		{
			auto players = ArkApi::GetApiUtils().GetWorld()->PlayerControllerListField();
			TArray<TAutoWeakObjectPtr<APlayerController>> final_list{};

			if (players.Num() < json_conf["General"]["MinPlayers"].get<int>())
			{
				return;
			}

			std::vector<uint64> blacklist_ids{};
			for (const auto& id : json_conf["BlacklistIDs"])
			{
				blacklist_ids.push_back(std::strtoull(id.get<std::string>().c_str(), nullptr, 10));
			}
			for (auto p : players)
			{
				if (p.Get() && !Platform::IsTeamProtected(p->TargetingTeamField()))
				{
					if (std::find(blacklist_ids.begin(), blacklist_ids.end(),
						ArkApi::GetApiUtils().GetSteamIdFromController(p.Get())) == blacklist_ids.end())
					{
						final_list.Push(p);
					}
				}
			}

			if (final_list.Num() <= 0)
			{
				return;
			}

			int rnd;

			if (final_list.Num() == 1)
			{
				rnd = 0;
			}
			else
			{
				rnd = Platform::RandNumber(0, final_list.Num() - 1);
			}

			if (final_list[rnd].Get())
			{
				steam_id = ArkApi::GetApiUtils().GetSteamIdFromController(final_list[rnd].Get());
			}
		}

		target.marked_time_ = time(0);
		target.logout_time_ = 0;
		target.steam_id_ = steam_id;

		AShooterPlayerController* controller = ArkApi::GetApiUtils().FindPlayerFromSteamId(target.steam_id_);
		if (controller)
		{
			const FString name = ArkApi::GetApiUtils().GetCharacterName(controller);
			target.target_name_ = name.ToString();
		}

		NotifyOnNewTarget();
	}

	void NotifyOnNewTarget()
	{
		AShooterPlayerController* controller = ArkApi::GetApiUtils().FindPlayerFromSteamId(target.steam_id_);
		if (controller)
		{
			const FString name = ArkApi::GetApiUtils().GetCharacterName(controller);
			ArkApi::GetApiUtils().SendChatMessageToAll(Msg("Sender"), *Msg("NewBounty"), name.ToString());
		}
	}

	void NotifyOnClaimTarget(AShooterPlayerController* controller)
	{
		FString name = ArkApi::GetApiUtils().GetCharacterName(controller);
		ArkApi::GetApiUtils().SendChatMessageToAll(Msg("Sender"), *Msg("BountyKilled"), name.ToString());
	}

	void Load()
	{
		auto& hooks = ArkApi::GetHooks();
		hooks.SetHook("AShooterGameMode.HandleNewPlayer_Implementation", &Hook_AShooterGameMode_HandleNewPlayer_Implementation, &AShooterGameMode_HandleNewPlayer_Implementation_original);
		hooks.SetHook("AShooterGameMode.Logout", &Hook_AShooterGameMode_Logout, &AShooterGameMode_Logout_original);
		hooks.SetHook("AShooterCharacter.Die", &Hook_AShooterCharacter_Die, &AShooterCharacter_Die_original);
		hooks.SetHook("AShooterPlayerState.ServerRequestLeaveTribe", &Hook_AShooterPlayerState_ServerRequestLeaveTribe,
			&AShooterPlayerState_ServerRequestLeaveTribe_original);

		auto& commands = ArkApi::GetCommands();
		commands.AddChatCommand(Cmd("Bounty"), &CliBounty);
		commands.AddConsoleCommand(Cmd("Reward"), &AdminReward);
		commands.AddConsoleCommand(Cmd("SetBounty"), &AdminSetBounty);
		commands.AddOnTimerCallback(L"BountyHunter_Update", &TimerCallback);
	}

	void Unload()
	{
		auto& hooks = ArkApi::GetHooks();
		hooks.DisableHook("AShooterGameMode.HandleNewPlayer_Implementation", &Hook_AShooterGameMode_HandleNewPlayer_Implementation);
		hooks.DisableHook("AShooterGameMode.Logout", &Hook_AShooterGameMode_Logout);
		hooks.DisableHook("AShooterCharacter.Die", &Hook_AShooterCharacter_Die);
		hooks.DisableHook("AShooterPlayerState.ServerRequestLeaveTribe", &Hook_AShooterPlayerState_ServerRequestLeaveTribe);

		auto& commands = ArkApi::GetCommands();
		commands.RemoveChatCommand(Cmd("Bounty"));
		commands.RemoveConsoleCommand(Cmd("Reward"));
		commands.RemoveConsoleCommand(Cmd("SetBounty"));
		commands.RemoveOnTimerCallback(L"BountyHunter_Update");
	}
}