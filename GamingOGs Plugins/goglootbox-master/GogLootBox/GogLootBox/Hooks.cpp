#include "Hooks.h"

#include <API/ARK/Ark.h>
#include "App.h"

namespace LootBox::Hooks
{
	DECLARE_HOOK(AShooterGameMode_HandleNewPlayer, bool, AShooterGameMode*, AShooterPlayerController*, UPrimalPlayerData*, AShooterCharacter*, bool);
	DECLARE_HOOK(AShooterGameMode_Logout, void, AShooterGameMode*, AController*);

	bool Hook_AShooterGameMode_HandleNewPlayer(AShooterGameMode* _this, AShooterPlayerController* newPlayer, UPrimalPlayerData* playerData, AShooterCharacter* playerCharacter, bool isFromLogin)
	{
		auto result = AShooterGameMode_HandleNewPlayer_original(_this, newPlayer, playerData, playerCharacter, isFromLogin);
		if (newPlayer)
		{
			App::Get().OnLogin(newPlayer);
		}
		return result;
	}

	void Hook_AShooterGameMode_Logout(AShooterGameMode* _this, AController* exiting)
	{
		if (exiting)
		{
			App::Get().OnLogout(static_cast<AShooterPlayerController*>(exiting));
		}
		AShooterGameMode_Logout_original(_this, exiting);
	}

	void SetHooks()
	{
		auto& hooks = ArkApi::GetHooks();
		hooks.SetHook("AShooterGameMode.HandleNewPlayer_Implementation", &Hook_AShooterGameMode_HandleNewPlayer, &AShooterGameMode_HandleNewPlayer_original);
		hooks.SetHook("AShooterGameMode.Logout", &Hook_AShooterGameMode_Logout, &AShooterGameMode_Logout_original);
	}

	void UnsetHooks()
	{
		auto& hooks = ArkApi::GetHooks();
		hooks.DisableHook("AShooterGameMode.HandleNewPlayer_Implementation", &Hook_AShooterGameMode_HandleNewPlayer);
		hooks.DisableHook("AShooterGameMode.Logout", &Hook_AShooterGameMode_Logout);
	}
}