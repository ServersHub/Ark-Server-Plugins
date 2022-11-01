#include "Hooks.h"

#include "../storage/Storage.h"
#include "../modules/OnlineManager.h"
#include "../modules/TeamManager.h"
#include "../modules/EntityFactory.h"
#include "../modules/PenaltyManager.h"
#include "../utils/ApiProxy.h"
#include "Application.h"

namespace AutoDecay
{
	namespace Hooks
	{
		DECLARE_HOOK(AShooterGameMode_GenerateTribeId, unsigned int, AShooterGameMode*);
		DECLARE_HOOK(AShooterPlayerState_AddToTribe, bool, AShooterPlayerState*, FTribeData*, bool, bool, bool, APlayerController*);
		DECLARE_HOOK(AShooterPlayerState_ServerRequestLeaveTribe_Implementation, void, AShooterPlayerState*);
		DECLARE_HOOK(AShooterGameMode_HandleNewPlayer_Implementation, bool, AShooterGameMode*, AShooterPlayerController*, UPrimalPlayerData*, AShooterCharacter*, bool);
		DECLARE_HOOK(AShooterGameMode_Logout, void, AShooterGameMode*, AController*);

		unsigned int Hook_AShooterGameMode_GenerateTribeId(AShooterGameMode* _this)
		{
			const int result = AShooterGameMode_GenerateTribeId_original(_this);

			if (!app->GetStorage()->Teams()->Get(result))
			{
				Team new_team = app->GetFactory()->CreateTeam(result);
				app->GetTeamManager()->RegisterTeam(new_team);
			}

			return result;
		}

		bool  Hook_AShooterPlayerState_AddToTribe(AShooterPlayerState* _this, FTribeData* my_new_tribe, bool merge_tribe, bool force, bool is_from_invite, APlayerController* inviter_controller)
		{
			const int old_team_id = _this->TargetingTeamField();
			auto result = AShooterPlayerState_AddToTribe_original(_this, my_new_tribe, merge_tribe, force, is_from_invite, inviter_controller);
			const int new_team_id = _this->TargetingTeamField();

			Storage* storage = app->GetStorage();

			Team old_team = storage->Teams()->Get(old_team_id);
			Team new_team = storage->Teams()->Get(new_team_id);

			if (old_team && new_team)
			{
				app->GetTeamManager()->MergeTeams(old_team, new_team);
			}

			return result;
		}

		void  Hook_AShooterPlayerState_ServerRequestLeaveTribe_Implementation(AShooterPlayerState* _this)
		{
			const int old_team_id = _this->TargetingTeamField();
			AShooterPlayerState_ServerRequestLeaveTribe_Implementation_original(_this);
			const int new_team_id = _this->TargetingTeamField();

			APlayerController* owner_controller = _this->GetOwnerController();
			if (!owner_controller)
			{
				return;
			}

			Storage* storage = app->GetStorage();
			TeamManager* team_manager = app->GetTeamManager();

			Team old_team = storage->Teams()->Get(old_team_id);
			Team new_team = storage->Teams()->Get(new_team_id);
			Player player = storage->Players()->Get(ArkApi::GetApiUtils().
				GetSteamIdFromController(reinterpret_cast<AController*>(owner_controller)));

			if (!player || !old_team)
			{
				return;
			}

			if (!new_team)
			{
				new_team = app->GetFactory()->CreateTeam(new_team_id);
				team_manager->RegisterTeam(new_team);
			}

			team_manager->RemovePlayerFromTeam(old_team, player);
			team_manager->AddPlayerToTeam(new_team, player);
		}

		bool  Hook_AShooterGameMode_HandleNewPlayer_Implementation(AShooterGameMode* _this, AShooterPlayerController* new_player, UPrimalPlayerData* player_data, AShooterCharacter* player_character,
			bool is_from_login)
		{
			auto result = AShooterGameMode_HandleNewPlayer_Implementation_original(_this, new_player, player_data, player_character, is_from_login);

			if (!new_player)
			{
				return result;
			}

			const uint64 steam_id = ArkApi::GetApiUtils().GetSteamIdFromController(reinterpret_cast<AController*>(new_player));
			const int team_id = reinterpret_cast<AController*>(new_player)->TargetingTeamField();

			Storage* storage = app->GetStorage();
			EntityFactory* factory = app->GetFactory();
			TeamManager* team_manager = app->GetTeamManager();
			PenaltyManager* penalty_manager = app->GetPenaltyManager();
			OnlineManager* online_manager = app->GetOnlineManager();

			Player player = storage->Players()->Get(steam_id);
			Team team = storage->Teams()->Get(team_id);

			if (!player)
			{
				player = factory->CreatePlayer(steam_id, team_id);
				team_manager->RegisterPlayer(player);
			}

			if (!team)
			{
				team = factory->CreateTeam(team_id);
				team_manager->RegisterTeam(team);
			}

			if (player->GetTeamId() != team->GetTeamId())
			{
				team_manager->AddPlayerToTeam(team, player);
			}

			online_manager->PlayerOnline(player);
			penalty_manager->OnTeamOnline(team);

			return result;
		}

		void Hook_AShooterGameMode_Logout(AShooterGameMode* _this, AController* exiting)
		{
			const uint64 steam_id = ArkApi::GetApiUtils().GetSteamIdFromController(exiting);
			const int team_id = exiting->TargetingTeamField();

			Storage* storage = app->GetStorage();

			AShooterGameMode_Logout_original(_this, exiting);

			auto player = storage->Players()->Get(steam_id);
			auto team = storage->Teams()->Get(team_id);

			if (player)
			{
				app->GetOnlineManager()->PlayerOffline(player);
			}
		}

		void Initialize()
		{
			auto& hooks = ArkApi::GetHooks();

			hooks.SetHook("AShooterGameMode.GenerateTribeId", &Hook_AShooterGameMode_GenerateTribeId, &AShooterGameMode_GenerateTribeId_original);
			hooks.SetHook("AShooterPlayerState.AddToTribe", &Hook_AShooterPlayerState_AddToTribe, &AShooterPlayerState_AddToTribe_original);
			hooks.SetHook("AShooterPlayerState.ServerRequestLeaveTribe_Implementation", &Hook_AShooterPlayerState_ServerRequestLeaveTribe_Implementation,
				&AShooterPlayerState_ServerRequestLeaveTribe_Implementation_original);
			hooks.SetHook("AShooterGameMode.HandleNewPlayer_Implementation", &Hook_AShooterGameMode_HandleNewPlayer_Implementation, &AShooterGameMode_HandleNewPlayer_Implementation_original);
			hooks.SetHook("AShooterGameMode.Logout", &Hook_AShooterGameMode_Logout, &AShooterGameMode_Logout_original);
		}

		void Deinitialize()
		{
			auto& hooks = ArkApi::GetHooks();

			hooks.DisableHook("AShooterGameMode.GenerateTribeId", &Hook_AShooterGameMode_GenerateTribeId);
			hooks.DisableHook("AShooterPlayerState.AddToTribe", &Hook_AShooterPlayerState_AddToTribe);
			hooks.DisableHook("AShooterPlayerState.ServerRequestLeaveTribe_Implementation", &Hook_AShooterPlayerState_ServerRequestLeaveTribe_Implementation);
			hooks.DisableHook("AShooterGameMode.HandleNewPlayer_Implementation", &Hook_AShooterGameMode_HandleNewPlayer_Implementation);
			hooks.DisableHook("AShooterGameMode.Logout", &Hook_AShooterGameMode_Logout);
		}
	}
}