#include "Hooks.h"

#include "ApiProxy.h"
#include "AdvertManager.h"

namespace GogAdvert::Hooks
{
	DECLARE_HOOK(AShooterGameMode_StartNewShooterPlayer, void, AShooterGameMode*, APlayerController*, bool, bool, FPrimalPlayerCharacterConfigStruct*, UPrimalPlayerData*);

	void Hook_AShooterGameMode_StartNewShooterPlayer(AShooterGameMode* _this, APlayerController* new_player, bool force_create_new_player_data, bool is_from_login,
		FPrimalPlayerCharacterConfigStruct* char_config, UPrimalPlayerData* ark_player_data)
	{
		AShooterGameMode_StartNewShooterPlayer_original(_this, new_player, force_create_new_player_data, is_from_login, char_config, ark_player_data);

		const auto steam_id = ArkApi::GetApiUtils().GetSteamIdFromController(reinterpret_cast<AController*>(new_player));
		AdvertManager::Get().OnNewPlayer(steam_id);
	}

	void AdvertUpdate()
	{
		AdvertManager::Get().Update();
	}

	void Initialize()
	{
		ArkApi::GetHooks().SetHook("AShooterGameMode.StartNewShooterPlayer", &Hook_AShooterGameMode_StartNewShooterPlayer, &AShooterGameMode_StartNewShooterPlayer_original);
		ArkApi::GetCommands().AddOnTimerCallback("Advert_Update", &AdvertUpdate);
	}

	void Shutdown()
	{
		ArkApi::GetHooks().DisableHook("AShooterGameMode.StartNewShooterPlayer", &Hook_AShooterGameMode_StartNewShooterPlayer);
		ArkApi::GetCommands().RemoveOnTimerCallback("Advert_Update");
	}
}