#include "Hooks.h"

#include <API/ARK/Ark.h>
#include "Store.h"

namespace Stats
{
	DECLARE_HOOK(AShooterGameMode_HandleNewPlayer_Implementation, bool, AShooterGameMode*, AShooterPlayerController*, UPrimalPlayerData*, AShooterCharacter*, bool);
	DECLARE_HOOK(AShooterGameMode_Logout, void, AShooterGameMode*, AController*);
	DECLARE_HOOK(AShooterPlayerState_AddToTribe, bool, AShooterPlayerState*, FTribeData*, bool, bool, bool, APlayerController*);
	DECLARE_HOOK(AShooterPlayerState_ServerRequestLeaveTribe_Implementation, void, AShooterPlayerState*);
	DECLARE_HOOK(AShooterPlayerState_ServerRequestRenameTribe_Implementation, void, AShooterPlayerState*, FString*);
	DECLARE_HOOK(AShooterPlayerState_ServerRequestSetTribeMemberGroupRank_Implementation, void, AShooterPlayerState*, int, int);
	DECLARE_HOOK(AShooterCharacter_Die, bool, AShooterCharacter*, float, FDamageEvent*, AController*, AActor*);
	DECLARE_HOOK(APrimalDinoCharacter_Die, bool, APrimalDinoCharacter*, float, FDamageEvent*, AController*, AActor*);

	std::string GetBlueprint(UObjectBase* object)
	{

		if (object != nullptr && object->ClassField() != nullptr)
		{
			FString pathName;
			object->ClassField()->GetDefaultObject(true)->GetFullName(&pathName, nullptr);

			if (int findIndex = 0; pathName.FindChar(' ', findIndex))
			{
				pathName = "Blueprint'" + pathName.Mid(findIndex + 1,
					pathName.Len() - (findIndex + (pathName.EndsWith(
						"_C", ESearchCase::
						CaseSensitive)
						? 3
						: 1))) + "'";

				return pathName.Replace(L"Default__", L"", ESearchCase::CaseSensitive).ToString();
			}
		}
		return std::string{};
	}

	class Watcher
	{
	public:
		static Watcher& Get()
		{
			static Watcher instance_;
			return instance_;
		}

		void Initialize()
		{
			Store::Get().LoadPlayersData(GetServerId(), m_data);
		}

		void CheckPlayer(AShooterPlayerController* controller)
		{
			const uint64 steamId = ArkApi::GetApiUtils().GetSteamIdFromController(controller);
			auto it = std::find_if(m_data.begin(), m_data.end(), [steamId](const PlayerData& item) { return steamId == item.steamId; });
			if (it != m_data.end())
			{
				if (it->needUpdate)
				{
					const int playerId = ArkApi::GetApiUtils().GetPlayerID(controller);
					if (playerId > 0)
					{
						const FString charName = ArkApi::GetApiUtils().GetCharacterName(static_cast<AShooterPlayerController*>(controller));
						Store::Get().UpdatePlayer(steamId, playerId, charName.ToString(), GetServerId());
						Store::Get().PlayerUpdated(steamId, GetServerId());
						it->needUpdate = false;
					}
				}

				AShooterPlayerState* playerState = static_cast<AShooterPlayerState*>(controller->PlayerStateField());
				if (playerState && playerState->MyTribeDataField())
				{
					FTribeData* tribeData = playerState->MyTribeDataField();
					FString rankGroup{};
					tribeData->GetRankNameForPlayerID(&rankGroup, ArkApi::GetApiUtils().GetPlayerID(controller));
					if (it->rankTag != rankGroup.ToString())
					{
						Store::Get().SetTribeRankTag(steamId, rankGroup.ToString(), GetServerId());
						it->rankTag = rankGroup.ToString();
					}
				}
			}
		}

		void OnNewPlayer(uint64 steamId)
		{
			m_data.push_back(PlayerData{ steamId, true, "" });
		}

	private:
		std::vector<PlayerData> m_data;
	};

	bool  Hook_AShooterGameMode_HandleNewPlayer_Implementation(AShooterGameMode* _this, AShooterPlayerController* newPlayer, UPrimalPlayerData* playerData, AShooterCharacter* playerCharacter,
		bool isFromLogin)
	{
		if (!newPlayer)
		{
			return AShooterGameMode_HandleNewPlayer_Implementation_original(_this, newPlayer, playerData, playerCharacter, isFromLogin);
		}

		const uint64 steamId = ArkApi::GetApiUtils().GetSteamIdFromController(reinterpret_cast<AController*>(newPlayer));
		if (!Store::Get().IsPlayerExists(steamId, GetServerId()))
		{
			Store::Get().CreatePlayer(steamId, GetServerId());
			Watcher::Get().OnNewPlayer(steamId);
		}
		Store::Get().PlayerOnline(steamId, GetServerId());

		if (newPlayer->IsInTribe())
		{
			AShooterPlayerState* playerState = static_cast<AShooterPlayerState*>(newPlayer->PlayerStateField());
			if (playerState && playerState->MyTribeDataField())
			{
				FTribeData* tribeData = playerState->MyTribeDataField();
				if (!Store::Get().IsTribeExists(tribeData->TribeIDField(), GetServerId()))
				{
					Store::Get().CreateTribe(tribeData->TribeIDField(), tribeData->TribeNameField().ToString(), GetServerId());
					Store::Get().SetTribeId(steamId, tribeData->TribeIDField(), GetServerId());
					FString rankGroup{};
					tribeData->GetRankNameForPlayerID(&rankGroup, ArkApi::GetApiUtils().GetPlayerID(newPlayer));
					Store::Get().SetTribeRankTag(steamId, rankGroup.ToString(), GetServerId());
				}
			}
		}

		return AShooterGameMode_HandleNewPlayer_Implementation_original(_this, newPlayer, playerData, playerCharacter, isFromLogin);
	}

	void Hook_AShooterGameMode_Logout(AShooterGameMode* _this, AController* exiting)
	{
		const uint64 steamId = ArkApi::GetApiUtils().GetSteamIdFromController(exiting);
		Store::Get().PlayerOffline(steamId, GetServerId());
		AShooterGameMode_Logout_original(_this, exiting);
	}

	bool  Hook_AShooterPlayerState_AddToTribe(AShooterPlayerState* _this, FTribeData* myNewTribe, bool mergeTribe, bool force, bool isIromInvite, APlayerController* inviterController)
	{
		const bool res = AShooterPlayerState_AddToTribe_original(_this, myNewTribe, mergeTribe, force, isIromInvite, inviterController);

		if (myNewTribe && res)
		{
			const int tribeId = myNewTribe->TribeIDField();
			if (!Store::Get().IsTribeExists(tribeId, GetServerId()))
			{
				const FString& tribeName = myNewTribe->TribeNameField();
				Store::Get().CreateTribe(tribeId, tribeName.ToString(), GetServerId());
			}

			const uint64 steamId = ArkApi::GetApiUtils().GetSteamIdFromController(_this->GetOwnerController());
			FString rankGroup{};
			myNewTribe->GetRankNameForPlayerID(&rankGroup, _this->MyPlayerDataStructField()->PlayerDataIDField());
			Store::Get().SetTribeId(steamId, tribeId, GetServerId());
			Store::Get().SetTribeRankTag(steamId, rankGroup.ToString(), GetServerId());
		}
		
		return res;
	}

	void  Hook_AShooterPlayerState_ServerRequestLeaveTribe_Implementation(AShooterPlayerState* _this)
	{
		const uint64 steamId = ArkApi::GetApiUtils().GetSteamIdFromController(_this->GetOwnerController());
		Store::Get().SetTribeId(steamId, 0, GetServerId());
		Store::Get().SetTribeRankTag(steamId, "", GetServerId());

		FTribeData* tribeData = _this->MyTribeDataField();
		if (tribeData)
		{
			Store::Get().MaybeRemoveTribe(tribeData->TribeIDField(), GetServerId());
		}

		AShooterPlayerState_ServerRequestLeaveTribe_Implementation_original(_this);
	}

	void  Hook_AShooterPlayerState_ServerRequestRenameTribe_Implementation(AShooterPlayerState* _this, FString* serverRequestRenameTribe)
	{
		AShooterPlayerState_ServerRequestRenameTribe_Implementation_original(_this, serverRequestRenameTribe);
		FTribeData* tribeData = _this->MyTribeDataField();
		if (tribeData)
		{
			Store::Get().RenameTribe(tribeData->TribeIDField(), tribeData->TribeNameField().ToString(), GetServerId());
		}
	}

	void  Hook_AShooterPlayerState_ServerRequestSetTribeMemberGroupRank_Implementation(AShooterPlayerState* _this, int playerIndexInTribe, int rankGroupIndex)
	{
		AShooterPlayerState_ServerRequestSetTribeMemberGroupRank_Implementation_original(_this, playerIndexInTribe, rankGroupIndex);
		FTribeData* tribeData = _this->MyTribeDataField();
		uint64 steamId = 0ULL;
		if (_this->GetOwnerController())
		{
			steamId = ArkApi::GetApiUtils().GetSteamIdFromController(_this->GetOwnerController());
		}
		FString rankGroup{};
		tribeData->GetRankNameForPlayerID(&rankGroup, _this->MyPlayerDataStructField()->PlayerDataIDField());
		Store::Get().SetTribeRankTag(steamId, rankGroup.ToString(), GetServerId());
	}

	bool  Hook_AShooterCharacter_Die(AShooterCharacter* _this, float killingDamage, FDamageEvent* damageEvent, AController* killer, AActor* damageCauser)
	{
		if (damageCauser && damageCauser->IsA(AShooterCharacter::GetPrivateStaticClass()) &&
			_this != damageCauser &&
			_this->TargetingTeamField() != damageCauser->TargetingTeamField())
		{
			const uint64 killedSteamId = ArkApi::GetApiUtils().GetSteamIDForPlayerID(ArkApi::GetApiUtils().GetPlayerID(_this));
			const int killedTribeId = _this->TargetingTeamField();
			Store::Get().Death(killedSteamId, GetServerId());
			if (Store::Get().IsTribeExists(killedTribeId, GetServerId()))
			{
				Store::Get().TribePlayerDeath(killedTribeId, GetServerId());
			}

			if (AShooterPlayerController* killerController = static_cast<AShooterPlayerController*>(killer))
			{
				const uint64 killerSteamId = ArkApi::GetApiUtils().GetSteamIdFromController(killerController);
				Store::Get().KillPlayer(killerSteamId, GetServerId());
				if (killerController->IsInTribe())
				{
					AShooterPlayerState* killerPlayerState = static_cast<AShooterPlayerState*>(killerController->PlayerStateField());
					if (killerPlayerState)
					{
						int killerTribeId = killerPlayerState->MyTribeDataField()->TribeIDField();
						Store::Get().TribeKillPlayer(killerTribeId, GetServerId());
					}
				}
			}
		}

		return AShooterCharacter_Die_original(_this, killingDamage, damageEvent, killer, damageCauser);
	}

	bool  Hook_APrimalDinoCharacter_Die(APrimalDinoCharacter* _this, float killingDamage, FDamageEvent* damageEvent, AController* killer, AActor* damageCauser)
	{
		if (damageCauser && damageCauser->IsA(AShooterCharacter::GetPrivateStaticClass()) &&
			_this->TargetingTeamField() != damageCauser->TargetingTeamField())
		{
			if (AShooterPlayerController* killerController = static_cast<AShooterPlayerController*>(killer))
			{
				const uint64 killerSteamId = ArkApi::GetApiUtils().GetSteamIdFromController(killerController);
				int killerTribeId = 0;
				int killedTribeId = _this->TargetingTeamField();
				if (killerController->IsInTribe())
				{
					AShooterPlayerState* killerPlayerState = static_cast<AShooterPlayerState*>(killerController->PlayerStateField());
					if (killerPlayerState)
					{
						killerTribeId = killerPlayerState->MyTribeDataField()->TribeIDField();
					}
				}

				const auto blueprint = GetBlueprint(_this);
				if (killedTribeId <= 50000)
				{
					if (IsRareDino(blueprint))
					{
						Store::Get().KillRareDino(killerSteamId, GetCustomFieldName(blueprint), GetServerId());
					}
					else
					{
						Store::Get().KillWildDino(killerSteamId, GetServerId());
					}
					if (killerTribeId > 0)
					{
						Store::Get().TribeKillWildDino(killerTribeId, GetServerId());
					}
				}
				else if(killedTribeId != 2000000000)
				{
					if (IsRareDino(blueprint))
					{
						Store::Get().KillRareDino(killerSteamId, GetCustomFieldName(blueprint), GetServerId());
					}
					else
					{
						Store::Get().KillTamedDino(killerSteamId, GetServerId());
					}
					if (killerTribeId > 0)
					{
						Store::Get().TribeKillTamedDino(killerTribeId, GetServerId());
					}
					if (Store::Get().IsTribeExists(killedTribeId, GetServerId()))
					{
						Store::Get().TribeDinoDeath(killedTribeId, GetServerId());
					}
				}
			}
		}

		return APrimalDinoCharacter_Die_original(_this, killingDamage, damageEvent, killer, damageCauser);
	}

	void PlayTimeWatch()
	{
		static int counter = 0;
		if (counter == 30)
		{
			auto& players = ArkApi::GetApiUtils().GetWorld()->PlayerControllerListField();
			for (auto& player : players)
			{
				if (player.Get())
				{
					Watcher::Get().CheckPlayer(static_cast<AShooterPlayerController*>(player.Get()));
				}
			}
			counter = 0;
		}
		else
		{
			counter++;
		}
		
	}

	void SetHooks()
	{
		auto& hooks = ArkApi::GetHooks();
		hooks.SetHook("AShooterGameMode.HandleNewPlayer_Implementation", &Hook_AShooterGameMode_HandleNewPlayer_Implementation, &AShooterGameMode_HandleNewPlayer_Implementation_original);
		hooks.SetHook("AShooterGameMode.Logout", &Hook_AShooterGameMode_Logout, &AShooterGameMode_Logout_original);
		hooks.SetHook("AShooterPlayerState.AddToTribe", &Hook_AShooterPlayerState_AddToTribe, &AShooterPlayerState_AddToTribe_original);
		hooks.SetHook("AShooterPlayerState.ServerRequestLeaveTribe_Implementation", &Hook_AShooterPlayerState_ServerRequestLeaveTribe_Implementation,
			&AShooterPlayerState_ServerRequestLeaveTribe_Implementation_original);
		hooks.SetHook("AShooterPlayerState.ServerRequestRenameTribe_Implementation", &Hook_AShooterPlayerState_ServerRequestRenameTribe_Implementation,
			&AShooterPlayerState_ServerRequestRenameTribe_Implementation_original);
		hooks.SetHook("AShooterPlayerState.ServerRequestSetTribeMemberGroupRank_Implementation", &Hook_AShooterPlayerState_ServerRequestSetTribeMemberGroupRank_Implementation,
			&AShooterPlayerState_ServerRequestSetTribeMemberGroupRank_Implementation_original);
		hooks.SetHook("AShooterCharacter.Die", &Hook_AShooterCharacter_Die, &AShooterCharacter_Die_original);
		hooks.SetHook("APrimalDinoCharacter.Die", &Hook_APrimalDinoCharacter_Die, &APrimalDinoCharacter_Die_original);


		ArkApi::GetCommands().AddOnTimerCallback(L"GogStats_WatchDog", &PlayTimeWatch);
		Watcher::Get().Initialize();
	}

	void UnsetHooks()
	{
		auto& hooks = ArkApi::GetHooks();
		hooks.DisableHook("AShooterGameMode.HandleNewPlayer_Implementation", &Hook_AShooterGameMode_HandleNewPlayer_Implementation);
		hooks.DisableHook("AShooterGameMode.Logout", &Hook_AShooterGameMode_Logout);
		hooks.DisableHook("AShooterPlayerState.AddToTribe", &Hook_AShooterPlayerState_AddToTribe);
		hooks.DisableHook("AShooterPlayerState.ServerRequestLeaveTribe_Implementation", &Hook_AShooterPlayerState_ServerRequestLeaveTribe_Implementation);
		hooks.DisableHook("AShooterPlayerState.ServerRequestRenameTribe_Implementation", &Hook_AShooterPlayerState_ServerRequestRenameTribe_Implementation);
		hooks.DisableHook("AShooterPlayerState.ServerRequestSetTribeMemberGroupRank_Implementation", &Hook_AShooterPlayerState_ServerRequestSetTribeMemberGroupRank_Implementation);
		hooks.DisableHook("AShooterCharacter.Die", &Hook_AShooterCharacter_Die);
		hooks.DisableHook("APrimalDinoCharacter.Die", &Hook_APrimalDinoCharacter_Die);


		ArkApi::GetCommands().RemoveOnTimerCallback(L"GogStats_WatchDog");
	}
}