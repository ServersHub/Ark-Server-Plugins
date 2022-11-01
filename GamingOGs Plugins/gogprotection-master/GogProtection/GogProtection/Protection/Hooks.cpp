#include "Hooks.h"

#include "../Common/Helpers.h"
#include "../Common/Config.h"
#include "../Storage/Storage.h"
#include "../Modules/OnlineMonitor.h"
#include "../Modules/EntityFactory.h"
#include "../Modules/TeamManager.h"
#include "../Modules/ProtManager.h"
#include "../Modules/HarvestManager.h"
#include "../Modules/BlackList.h"
#include "../Modules/Logging.h"
#include "../Modules/NotificationsManager.h"
#include "../Modules/DragManager.h"
#include "../Modules/ArenaManager.h"
#include "Application.h"

#include <API/ARK/Ark.h>

#pragma warning(disable : 4311)
#pragma warning(disable : 4302)

namespace Protection
{
    namespace Hooks
    {
		DECLARE_HOOK(APrimalCharacter_TakeDamage, float, APrimalCharacter*, float, FDamageEvent*, AController*, AActor*);
		DECLARE_HOOK(APrimalStructure_TakeDamage, float, APrimalStructure*, float, FDamageEvent*, AController*, AActor*);
		DECLARE_HOOK(AShooterGameMode_GenerateTribeId, unsigned int, AShooterGameMode*);
		DECLARE_HOOK(AShooterPlayerState_AddToTribe, bool, AShooterPlayerState*, FTribeData*, bool, bool, bool, APlayerController*);
		DECLARE_HOOK(AShooterPlayerState_ServerRequestLeaveTribe_Implementation, void, AShooterPlayerState*);
		DECLARE_HOOK(AShooterGameMode_HandleNewPlayer_Implementation, bool, AShooterGameMode*, AShooterPlayerController*, UPrimalPlayerData*, AShooterCharacter*, bool);
		DECLARE_HOOK(AShooterGameMode_Logout, void, AShooterGameMode*, AController*);
		DECLARE_HOOK(AShooterGameMode_SaveWorld, void, AShooterGameMode*);
		DECLARE_HOOK(AShooterPlayerState_AcceptJoinAlliance, void, AShooterPlayerState*, unsigned int, unsigned int, FString);
		DECLARE_HOOK(APrimalDinoCharacter_CanCarryCharacter, bool, APrimalDinoCharacter*, APrimalCharacter*);
		DECLARE_HOOK(APrimalCharacter_CanDragCharacter, bool, APrimalCharacter*, APrimalCharacter*);
		DECLARE_HOOK(AShooterCharacter_AllowGrappling_Implementation, bool, AShooterCharacter*);
		DECLARE_HOOK(APrimalStructureTurret_CanFire, bool, APrimalStructureTurret*);
		DECLARE_HOOK(APrimalStructureTurret_DealDamage, void, APrimalStructureTurret*, FHitResult*, FVector*, int, TSubclassOf<UDamageType>, float);
		DECLARE_HOOK(AShooterPlayerController_ClientChatMessage, void, AShooterPlayerController*, FChatMessage);
		DECLARE_HOOK(UPrimalItem_IncrementItemQuantity, int, UPrimalItem*, int, bool, bool, bool, bool, bool);
		DECLARE_HOOK(UPrimalHarvestingComponent_GiveHarvestResource, void, UPrimalHarvestingComponent*, UPrimalInventoryComponent*, float,
			TSubclassOf<UDamageType>, AActor*, TArray<FHarvestResourceEntry, FDefaultAllocator>*);

		void TargetAPProtectedNotification(AController* controller, FLinearColor color, float scale, float time, TeamItemType damaged_team)
		{
			if (!damaged_team || !controller)
				return;

			const FString remaining_protection = Helpers::ToHumanReadableTime(App->GetProtManager()->GetProtectionRemainingTime(damaged_team, ProtectionType::AP));

			ArkApi::GetApiUtils().SendNotification(reinterpret_cast<AShooterPlayerController*>(controller), color, scale, time, nullptr,
				*GetText("APIsActive"), *remaining_protection);
		}

		void TargetORPProtectedNotification(AController* controller, FLinearColor color, float scale, float time, TeamItemType damaged_team, EntityType target_entity_type)
		{
			if (!damaged_team || !controller)
				return;

			const int protection_percent = static_cast<int>(App->GetProtManager()->GetProtectionPercent(target_entity_type, ProtectionType::ORP) * 100);

			ArkApi::GetApiUtils().SendNotification(reinterpret_cast<AShooterPlayerController*>(controller), color, scale, time, nullptr,
				*GetText("ORPIsActive"), protection_percent);
		}

		float  Hook_APrimalCharacter_TakeDamage(APrimalCharacter* _this, float damage, FDamageEvent* damage_event, AController* event_instigator, AActor* damage_causer)
		{
			if ((_this->TargetingTeamField() > 50000) && (damage_causer && damage_causer->TargetingTeamField() > 50000) && (damage > 0.0f) && ((int)_this != (int)damage_causer))
			{
				Storage* storage = App->GetStorage();
				ProtManager* prot_manager = App->GetProtManager();
				NotificationsManager* notifications_manager = App->GetNotificationsManager();

				EntityType damaged_entity_type = EntityType::Unknown;
				EntityType damager_entity_type = EntityType::Unknown;
				EntityType instigator_entity_type = EntityType::Unknown;

				if (_this->IsA(AShooterCharacter::GetPrivateStaticClass()))
					damaged_entity_type = EntityType::Character;
				if (_this->IsA(APrimalDinoCharacter::GetPrivateStaticClass()))
					damaged_entity_type = EntityType::Dino;

				if (damage_causer->IsA(AShooterCharacter::GetPrivateStaticClass()))
					damager_entity_type = EntityType::Character;
				if (damage_causer->IsA(APrimalDinoCharacter::GetPrivateStaticClass()))
					damager_entity_type = EntityType::Dino;
				if (damage_causer->IsA(APrimalStructure::GetPrivateStaticClass()))
					damager_entity_type = EntityType::Turret;

				if (event_instigator && event_instigator->IsA(AShooterPlayerController::GetPrivateStaticClass()))
					instigator_entity_type = EntityType::Character;

				FVector position{ 0.0f, 0.0f, 0.0f };
				if (event_instigator)
				{
					position = event_instigator->DefaultActorLocationField();
					const bool is_in_arena = App->GetArenaManager()->IsInArena(&position);
					if (is_in_arena)
					{
						return APrimalCharacter_TakeDamage_original(_this, damage, damage_event, event_instigator, damage_causer);
					}
				}

				TeamItemType damaged_team = storage->Teams()->Get(_this->TargetingTeamField());
				TeamItemType damager_team = storage->Teams()->Get(damage_causer->TargetingTeamField());

				if ((damaged_team && damager_team) &&
					((damaged_team->GetTeamId() != damager_team->GetTeamId()) ||
						static_cast<bool>(GetConfig()["AP"]["General"]["AllowFriendlyDamage"])))
				{
					if(damaged_team->GetTeamId() != damager_team->GetTeamId())
						prot_manager->TeamInCombat(damaged_team);

					if (damager_entity_type == EntityType::Character || damager_entity_type == EntityType::Dino)
						prot_manager->TeamInCombat(damager_team);

					float calculated_damage = damage;

					if (damager_entity_type != EntityType::Turret)
					{
						if (damaged_entity_type == EntityType::Dino)
						{
							if (prot_manager->IsProtectionActive(damaged_team, ProtectionType::AP) &&
								prot_manager->IsProtectionEnabled(EntityType::Dino, ProtectionType::AP))
							{
								calculated_damage = damage * prot_manager->GetProtectionPercent(EntityType::Dino, ProtectionType::AP);

								if (instigator_entity_type == EntityType::Character)
								{
									PlayerItemType player = storage->Players()->Get(ArkApi::GetApiUtils().GetSteamIdFromController(event_instigator));
									if(notifications_manager->IsAllowed(player, ProtectionType::AP, EntityType::Dino, reinterpret_cast<void*>(_this)))
									{
										TargetAPProtectedNotification(event_instigator, FColorList::Red, 1.2f, 10.0f, damaged_team);
										notifications_manager->OnNotified(player, ProtectionType::AP, EntityType::Dino, reinterpret_cast<void*>(_this));
									}
								}

								return APrimalCharacter_TakeDamage_original(_this, calculated_damage, damage_event, event_instigator, damage_causer);
							}

							if (prot_manager->IsProtectionActive(damager_team, ProtectionType::AP) &&
								!prot_manager->IsProtectionActive(damaged_team, ProtectionType::AP))
							{
								if (!static_cast<bool>(GetConfig()["AP"]["General"]["AllowProtectedPlayersKillEnemyDinos"]))
								{
									if (instigator_entity_type == EntityType::Character)
									{
										ArkApi::GetApiUtils().SendNotification(reinterpret_cast<AShooterPlayerController*>(event_instigator),
											FColorList::Red, 1.2f, 10.0f, nullptr, *GetText("TryingAttackBeingUnderAP"));
									}
									return APrimalCharacter_TakeDamage_original(_this, 0.0f, damage_event, event_instigator, damage_causer);
								}
							}

							if (prot_manager->IsProtectionActive(damaged_team, ProtectionType::ORP) &&
								prot_manager->ORPCheckRangeHit(damaged_team, &position) &&
								prot_manager->IsProtectionEnabled(EntityType::Dino, ProtectionType::ORP))
							{
								calculated_damage = damage * prot_manager->GetProtectionPercent(EntityType::Dino, ProtectionType::ORP);

								if (instigator_entity_type == EntityType::Character)
								{
									PlayerItemType player = storage->Players()->Get(ArkApi::GetApiUtils().GetSteamIdFromController(event_instigator));
									if(notifications_manager->IsAllowed(player, ProtectionType::ORP, EntityType::Dino, reinterpret_cast<void*>(_this)))
									{
										TargetORPProtectedNotification(event_instigator, FColorList::Red, 1.2f, 10.0f, damaged_team, EntityType::Dino);
										notifications_manager->OnNotified(player, ProtectionType::ORP, EntityType::Dino, reinterpret_cast<void*>(_this));
									}
								}

								return APrimalCharacter_TakeDamage_original(_this, calculated_damage, damage_event, event_instigator, damage_causer);
							}
						}

						if (damaged_entity_type == EntityType::Character)
						{
							if (prot_manager->IsProtectionActive(damaged_team, ProtectionType::AP) &&
								prot_manager->IsProtectionEnabled(EntityType::Character, ProtectionType::AP))
							{
								if (damager_entity_type == EntityType::Character)
								{
									PlayerItemType player = storage->Players()->Get(ArkApi::GetApiUtils().GetSteamIdFromController(event_instigator));
									if(notifications_manager->IsAllowed(player, ProtectionType::AP, EntityType::Character, reinterpret_cast<void*>(_this)))
									{
										TargetAPProtectedNotification(event_instigator, FColorList::Red, 1.2f, 10.0f, damaged_team);
										notifications_manager->OnNotified(player, ProtectionType::AP, EntityType::Character, reinterpret_cast<void*>(_this));
									}
								}

								calculated_damage = damage * prot_manager->GetProtectionPercent(EntityType::Character, ProtectionType::AP);
								return APrimalCharacter_TakeDamage_original(_this, calculated_damage, damage_event, event_instigator, damage_causer);
							}

							if (prot_manager->IsProtectionActive(damager_team, ProtectionType::AP) &&
								!prot_manager->IsProtectionActive(damaged_team, ProtectionType::AP))
							{
								if (!static_cast<bool>(GetConfig()["AP"]["General"]["AllowProtectedPlayersKillEnemyPlayers"]))
								{
									if (instigator_entity_type == EntityType::Character)
									{
										ArkApi::GetApiUtils().SendNotification(reinterpret_cast<AShooterPlayerController*>(event_instigator),
											FColorList::Red, 1.2f, 10.0f, nullptr, *GetText("TryingAttackBeingUnderAP"));
									}
									return APrimalCharacter_TakeDamage_original(_this, 0.0f, damage_event, event_instigator, damage_causer);
								}
							}

							if (prot_manager->IsProtectionActive(damaged_team, ProtectionType::ORP) &&
								prot_manager->ORPCheckRangeHit(damaged_team, &position) &&
								prot_manager->IsProtectionEnabled(EntityType::Character, ProtectionType::ORP))
							{
								if (damager_entity_type == EntityType::Character)
								{
									PlayerItemType player = storage->Players()->Get(ArkApi::GetApiUtils().GetSteamIdFromController(event_instigator));
									if(notifications_manager->IsAllowed(player, ProtectionType::ORP, EntityType::Character, reinterpret_cast<void*>(_this)))
									{
										TargetORPProtectedNotification(event_instigator, FColorList::Red, 1.2f, 10.0f, damaged_team, EntityType::Character);
										notifications_manager->OnNotified(player, ProtectionType::ORP, EntityType::Character, reinterpret_cast<void*>(_this));
									}
								}
									
								calculated_damage = damage * prot_manager->GetProtectionPercent(EntityType::Character, ProtectionType::ORP);
								return APrimalCharacter_TakeDamage_original(_this, calculated_damage, damage_event, event_instigator, damage_causer);
							}
						}
					}
				}
			}

			return APrimalCharacter_TakeDamage_original(_this, damage, damage_event, event_instigator, damage_causer);
		}

		float  Hook_APrimalStructure_TakeDamage(APrimalStructure* _this, float damage, FDamageEvent* damage_event, AController* event_instigator, AActor* damage_causer)
		{
			if ((_this->TargetingTeamField() > 50000) && (damage_causer && damage_causer->TargetingTeamField() > 50000) && (damage > 0.0f))
			{
				Storage* storage = App->GetStorage();
				ProtManager* prot_manager = App->GetProtManager();
				BlackList* black_list = App->GetBlackList();
				NotificationsManager* notifications_manager = App->GetNotificationsManager();

				EntityType damager_entity_type = EntityType::Unknown;
				EntityType instigator_entity_type = EntityType::Unknown;

				if (damage_causer->IsA(AShooterCharacter::GetPrivateStaticClass()))
					damager_entity_type = EntityType::Character;
				if (damage_causer->IsA(APrimalDinoCharacter::GetPrivateStaticClass()))
					damager_entity_type = EntityType::Dino;

				if (event_instigator && event_instigator->IsA(AShooterPlayerController::GetPrivateStaticClass()))
					instigator_entity_type = EntityType::Character;

				FVector pos{};
				auto transform_component = _this->MyRootTransformField();
				if (transform_component.Object)
				{
					pos = transform_component->RelativeLocationField();
				}

				TeamItemType damaged_team = storage->Teams()->Get(_this->TargetingTeamField());
				TeamItemType damager_team = storage->Teams()->Get(damage_causer->TargetingTeamField());

				if ((damaged_team && damager_team) &&
					((damaged_team->GetTeamId() != damager_team->GetTeamId()) ||
						static_cast<bool>(GetConfig()["AP"]["General"]["AllowFriendlyDamage"])))
				{
					if (damaged_team->GetTeamId() != damager_team->GetTeamId())
						prot_manager->TeamInCombat(damaged_team);

					prot_manager->TeamInCombat(damager_team);

					if (prot_manager->IsProtectionActive(damaged_team, ProtectionType::AP) &&
						prot_manager->IsProtectionEnabled(EntityType::Structure, ProtectionType::AP))
					{
						if (instigator_entity_type == EntityType::Character)
						{
							PlayerItemType player = storage->Players()->Get(ArkApi::GetApiUtils().GetSteamIdFromController(event_instigator));
							if(notifications_manager->IsAllowed(player, ProtectionType::AP, EntityType::Structure, reinterpret_cast<void*>(_this)))
							{
								TargetAPProtectedNotification(event_instigator, FColorList::Red, 1.2f, 10.0f, damaged_team);
								notifications_manager->OnNotified(player, ProtectionType::AP, EntityType::Structure, reinterpret_cast<void*>(_this));
							}
						}

						const float calculated_damage = damage * prot_manager->GetProtectionPercent(EntityType::Structure, ProtectionType::AP);
						return APrimalStructure_TakeDamage_original(_this, calculated_damage, damage_event, event_instigator, damage_causer);
					}

					if (prot_manager->IsProtectionActive(damager_team, ProtectionType::AP) &&
						!prot_manager->IsProtectionActive(damaged_team, ProtectionType::AP))
					{
						if (!static_cast<bool>(GetConfig()["AP"]["General"]["AllowProtectedPlayersDestroyEnemyStructures"]))
						{
							if (instigator_entity_type == EntityType::Character)
							{
								ArkApi::GetApiUtils().SendNotification(reinterpret_cast<AShooterPlayerController*>(event_instigator),
									FColorList::Red, 1.2f, 10.0f, nullptr, *GetText("TryingAttackBeingUnderAP"));
							}
							return APrimalStructure_TakeDamage_original(_this, 0.0f, damage_event, event_instigator, damage_causer);
						}
					}

					if (prot_manager->IsProtectionActive(damaged_team, ProtectionType::ORP) &&
						prot_manager->ORPCheckRangeHit(damaged_team, &pos) &&
						!black_list->IsTeamBlackListed(damaged_team))
					{
						if (instigator_entity_type == EntityType::Character)
						{
							PlayerItemType player = storage->Players()->Get(ArkApi::GetApiUtils().GetSteamIdFromController(event_instigator));
							if(notifications_manager->IsAllowed(player, ProtectionType::ORP, EntityType::Structure, reinterpret_cast<void*>(_this)))
							{
								TargetORPProtectedNotification(event_instigator, FColorList::Red, 1.2f, 10.0f, damaged_team, EntityType::Structure);
								notifications_manager->OnNotified(player, ProtectionType::ORP, EntityType::Structure, reinterpret_cast<void*>(_this));
							}
						}
						
						const float calculated_damage = damage * prot_manager->GetProtectionPercent(EntityType::Structure, ProtectionType::ORP);
						return APrimalStructure_TakeDamage_original(_this, calculated_damage, damage_event, event_instigator, damage_causer);
					}
				}
			}

			return APrimalStructure_TakeDamage_original(_this, damage, damage_event, event_instigator, damage_causer);
		}

		unsigned int Hook_AShooterGameMode_GenerateTribeId(AShooterGameMode* _this)
		{
			const int result = AShooterGameMode_GenerateTribeId_original(_this);
			
			if (!App->GetStorage()->Teams()->Get(result)) {
				TeamItemType new_team = App->GetFactory()->MakeTeam(result);
				App->GetTeamManager()->RegisterTeam(new_team);
			}

			return result;
		}

		bool  Hook_AShooterPlayerState_AddToTribe(AShooterPlayerState* _this, FTribeData* my_new_tribe, bool merge_tribe, bool force, bool is_from_invite, APlayerController* inviter_controller)
		{
			const int old_team_id = _this->TargetingTeamField();
			auto result = AShooterPlayerState_AddToTribe_original(_this, my_new_tribe, merge_tribe, force, is_from_invite, inviter_controller);
			const int new_team_id = _this->TargetingTeamField();

			Storage* storage = App->GetStorage();
			TeamManager* team_manager = App->GetTeamManager();
			ProtManager* prot_manager = App->GetProtManager();

			TeamItemType old_team = storage->Teams()->Get(old_team_id);
			TeamItemType new_team = storage->Teams()->Get(new_team_id);

			if (old_team && new_team) {
				if (team_manager->IsTeamEmpty(new_team))
					prot_manager->ORPTransferZones(old_team, new_team);
				else
					prot_manager->ORPRemoveZones(new_team);

				team_manager->MergeTeams(old_team, new_team);

				if (team_manager->IsTeamEmpty(old_team))
					team_manager->UnregisterTeam(old_team);
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
				return;

			Storage* storage = App->GetStorage();
			EntityFactory* factory = App->GetFactory();
			TeamManager* team_manager = App->GetTeamManager();
			ProtManager* prot_manager = App->GetProtManager();
			OnlineMonitor* online_monitor = App->GetOnlineMonitor();

			TeamItemType old_team = storage->Teams()->Get(old_team_id);
			TeamItemType new_team = storage->Teams()->Get(new_team_id);
			PlayerItemType player = storage->Players()->Get(ArkApi::GetApiUtils().
				GetSteamIdFromController(reinterpret_cast<AController*>(owner_controller)));
			
			if (!player || !old_team)
				return;

			if (!new_team) {
				new_team = factory->MakeTeamBasedOn(new_team_id, old_team);
				team_manager->RegisterTeam(new_team);
			}

			team_manager->RemovePlayerFromTeam(old_team, player);
			team_manager->AddPlayerToTeam(new_team, player);

			if (team_manager->IsTeamEmpty(old_team))
				prot_manager->ORPTransferZones(old_team, new_team);

			if (team_manager->IsTeamEmpty(old_team))
				team_manager->UnregisterTeam(old_team);
			else if (!online_monitor->IsTeamOnline(old_team))
				prot_manager->ORPCheckOnTeamOffline(old_team);
		}

		bool  Hook_AShooterGameMode_HandleNewPlayer_Implementation(AShooterGameMode* _this, AShooterPlayerController* new_player, UPrimalPlayerData* player_data, AShooterCharacter* player_character,
			bool is_from_login)
		{
			auto result = AShooterGameMode_HandleNewPlayer_Implementation_original(_this, new_player, player_data, player_character, is_from_login);

			if (!new_player)
				return result;

			const uint64 steam_id = ArkApi::GetApiUtils().GetSteamIdFromController(reinterpret_cast<AController*>(new_player));
			const int team_id = reinterpret_cast<AController*>(new_player)->TargetingTeamField();

			Storage* storage = App->GetStorage();
			EntityFactory* factory = App->GetFactory();
			TeamManager* team_manager = App->GetTeamManager();
			ProtManager* prot_manager = App->GetProtManager();
			OnlineMonitor* online_monitor = App->GetOnlineMonitor();

			PlayerItemType player = storage->Players()->Get(steam_id);
			TeamItemType team = storage->Teams()->Get(team_id);

			if (!player) {
				player = factory->MakePlayer(steam_id);
				team_manager->RegisterPlayer(player);
			}

			if (!team) {
				team = factory->MakeTeam(team_id);
				team_manager->RegisterTeam(team);

				const FString character_name = ArkApi::GetApiUtils().GetCharacterName(new_player);
				const FString ap_initial_time = Helpers::ToHumanReadableTime(prot_manager->GetProtectionRemainingTime(team, ProtectionType::AP));
				const uint64 steam_id = ArkApi::GetApiUtils().GetSteamIdFromController(reinterpret_cast<AController*>(new_player));

				App->GetDiscordLogger()->LogNewPlayer(character_name, ap_initial_time, steam_id);
			}

			if (player->GetTeamId() != team->GetTeamId())
				team_manager->AddPlayerToTeam(team, player);

			online_monitor->PlayerOnline(player);

			prot_manager->ORPCheckOnTeamOnline(team);

			return result;
		}

		void Hook_AShooterGameMode_Logout(AShooterGameMode* _this, AController* exiting)
		{
			const uint64 steam_id = ArkApi::GetApiUtils().GetSteamIdFromController(exiting);
			const int team_id = exiting->TargetingTeamField();

			Storage* storage = App->GetStorage();
			OnlineMonitor* online_monitor = App->GetOnlineMonitor();

			AShooterGameMode_Logout_original(_this, exiting);

			auto player = storage->Players()->Get(steam_id);
			auto team = storage->Teams()->Get(team_id);

			if (player)
				online_monitor->PlayerOffline(player);

			if (team && !online_monitor->IsTeamOnline(team))
				App->GetProtManager()->ORPCheckOnTeamOffline(team);
		}

		void  Hook_AShooterGameMode_SaveWorld(AShooterGameMode* _this)
		{
			App->OnSaveWorld();

			AShooterGameMode_SaveWorld_original(_this);
		}

		void  Hook_AShooterPlayerState_AcceptJoinAlliance(AShooterPlayerState* _this, unsigned int alliance_id, unsigned int new_member_id, FString new_member_name)
		{
			if (_this && static_cast<bool>(GetConfig()["AP"]["General"]["HandleAlliances"])) {
				Storage* storage = App->GetStorage();

				TeamItemType first_team = storage->Teams()->Get(_this->TargetingTeamField());
				TeamItemType second_team = storage->Teams()->Get(new_member_id);

				if (first_team && second_team)
					App->GetTeamManager()->AllyTeams(first_team, second_team);
			}

			AShooterPlayerState_AcceptJoinAlliance_original(_this, alliance_id, new_member_id, new_member_name);
		}

		bool Hook_APrimalDinoCharacter_CanCarryCharacter(APrimalDinoCharacter* _this, APrimalCharacter* can_carry_pawn)
		{
			if (can_carry_pawn && _this->TargetingTeamField() >= 50000 && can_carry_pawn->TargetingTeamField() >= 50000)
			{
				TeamItemType carrying_team = App->GetStorage()->Teams()->Get(_this->TargetingTeamField());
				TeamItemType carried_team = App->GetStorage()->Teams()->Get(can_carry_pawn->TargetingTeamField());

				if (!carrying_team || !carried_team)
				{
					return APrimalDinoCharacter_CanCarryCharacter_original(_this, can_carry_pawn);
				}

				const bool are_allied = ArkApi::GetApiUtils().GetShooterGameMode()->AreTribesAllied(carrying_team->GetTeamId(), carried_team->GetTeamId());
				bool can_carry;
				if (_this->CanFly())
				{
					can_carry = App->GetDragManager()->CanCarry(_this);
				}
				else
				{
					can_carry = App->GetDragManager()->CanDrag(_this);
				}

				if (!are_allied && carrying_team->GetTeamId() != carried_team->GetTeamId() && !can_carry)
				{
					if (App->GetProtManager()->IsProtectionActive(carried_team, ProtectionType::AP) ||
						App->GetProtManager()->IsProtectionActive(carrying_team, ProtectionType::AP))
					{
						return false;
					}
				}
			}

			return APrimalDinoCharacter_CanCarryCharacter_original(_this, can_carry_pawn);
		}

		bool Hook_APrimalCharacter_CanDragCharacter(APrimalCharacter* _this, APrimalCharacter* character)
		{
			if (_this->TargetingTeamField() < 50000 || character->TargetingTeamField() < 50000)
			{
				return APrimalCharacter_CanDragCharacter_original(_this, character);
			}

			const int dragger_team_id = _this->TargetingTeamField();
			const int dragged_team_id = character->TargetingTeamField();
			TeamItemType dragger_team = App->GetStorage()->Teams()->Get(dragger_team_id);
			TeamItemType dragged_team = App->GetStorage()->Teams()->Get(dragged_team_id);

			if (!dragger_team || !dragged_team || (dragger_team_id == dragged_team_id))
			{
				return APrimalCharacter_CanDragCharacter_original(_this, character);
			}

			EntityType dragger_entity_type = EntityType::Unknown;
			EntityType dragged_entity_type = EntityType::Unknown;

			if (_this->IsA(AShooterCharacter::GetPrivateStaticClass()))
			{
				dragger_entity_type = EntityType::Character;
			}
			else if (_this->IsA(APrimalDinoCharacter::GetPrivateStaticClass()))
			{
				dragger_entity_type = EntityType::Dino;
			}

			if (character->IsA(AShooterCharacter::GetPrivateStaticClass()))
			{
				dragged_entity_type = EntityType::Character;
			}
			else if (character->IsA(APrimalDinoCharacter::GetPrivateStaticClass()))
			{
				dragged_entity_type = EntityType::Dino;
			}

			if (dragger_entity_type == EntityType::Character)
			{
				if (dragged_entity_type == EntityType::Character)
				{
					if (reinterpret_cast<AShooterCharacter*>(_this)->GetLinkedPlayerDataID() == reinterpret_cast<AShooterCharacter*>(character)->GetLinkedPlayerDataID())
					{
						return true;
					}
					else
					{
						return !(App->GetProtManager()->IsProtectionActive(dragged_team, ProtectionType::AP) && !GetConfig()["AP"]["General"]["CanDragProtected"].get<bool>());
					}
				}
				else if(dragger_team_id != dragged_team_id)
				{
					return !(App->GetProtManager()->IsProtectionActive(dragged_team, ProtectionType::AP) && !GetConfig()["AP"]["General"]["CanDragProtected"].get<bool>());
				}
			}

			return APrimalCharacter_CanDragCharacter_original(_this, character);
		}

		bool  Hook_AShooterCharacter_AllowGrappling_Implementation(AShooterCharacter* _this)
		{
			int team_id = _this->TargetingTeamField();
			TeamItemType team = App->GetStorage()->Teams()->Get(team_id);

			if (team)
			{
				if (App->GetProtManager()->IsProtectionActive(team, ProtectionType::AP))
				{
					return false;
				}
			}

			return AShooterCharacter_AllowGrappling_Implementation_original(_this);
		}

		bool Hook_APrimalStructureTurret_CanFire(APrimalStructureTurret* _this)
		{
			TeamItemType team = App->GetStorage()->Teams()->Get(_this->TargetingTeamField());
			
			if (team && App->GetProtManager()->IsProtectionActive(team, ProtectionType::ORP)) {
				const bool is_allow_firing = GetConfig()["ORP"]["AllowTurrets"];

				if (!is_allow_firing)
					return false;
			}

			return APrimalStructureTurret_CanFire_original(_this);
		}

		void  Hook_APrimalStructureTurret_DealDamage(APrimalStructureTurret* _this, FHitResult* impact, FVector* shoot_dir,
			int damage_amount, TSubclassOf<UDamageType> damage_type, float impulse)
		{
			TeamItemType team = App->GetStorage()->Teams()->Get(_this->TargetingTeamField());
			float calculated_damage = static_cast<float>(damage_amount);

			if (team && App->GetProtManager()->IsProtectionActive(team, ProtectionType::ORP))
				calculated_damage *= static_cast<float>(GetConfig()["ORP"]["TurretDamageModifier"]);

			APrimalStructureTurret_DealDamage_original(_this, impact, shoot_dir, static_cast<int>(calculated_damage), damage_type, impulse);
		}

		void Hook_AShooterPlayerController_ClientChatMessage(AShooterPlayerController* _this, FChatMessage msg)
		{
			TeamItemType team = App->GetStorage()->Teams()->Get(msg.SenderTeamIndex);

			if (team && App->GetProtManager()->IsProtectionActive(team, ProtectionType::AP)) {
				if (!msg.SenderSteamName.IsEmpty()) {
					const std::string iconPath = GetConfig()["AP"]["General"]["Icon"];
					const std::string prefix = GetConfig()["AP"]["General"]["Tag"];

					UTexture2D* icon = reinterpret_cast<UTexture2D*>(Globals::StaticLoadObject(UTexture2D::StaticClass(), nullptr,
						ArkApi::Tools::ConvertToWideStr(iconPath).c_str(), nullptr, 0, 0, true));

					if (icon) {
						msg.SenderIcon = icon;
						msg.SenderName = FString(prefix) + msg.SenderName;
					}
				}
			}

			AShooterPlayerController_ClientChatMessage_original(_this, msg);
		}

		int  Hook_UPrimalItem_IncrementItemQuantity(UPrimalItem* _this, int amount, bool replicate_to_client, bool dont_update_weight, bool is_from_use_consumption,
			bool is_ark_tribute_item, bool is_from_crafting_consumption)
		{
			AActor* owner = _this->GetOwnerActor();

			if (owner && owner->IsA(AShooterCharacter::GetPrivateStaticClass())) {
				TeamItemType team = App->GetStorage()->Teams()->Get(owner->TargetingTeamField());
				if (team && App->GetProtManager()->IsProtectionActive(team, ProtectionType::AP)) {
					FString item_type;
					_this->GetItemTypeString(&item_type);

					if (item_type == FString("Resource")) {
						if (App->GetHVManager()->TeamHasBonus(team)) {
							App->GetHVManager()->UnsetBonus(team);
							return UPrimalItem_IncrementItemQuantity_original(_this, App->GetHVManager()->CalculateAmount(amount), replicate_to_client,
								dont_update_weight, is_from_use_consumption, is_ark_tribute_item, is_from_crafting_consumption);
						}
					}
				}
			}

			return UPrimalItem_IncrementItemQuantity_original(_this, amount, replicate_to_client, dont_update_weight, is_from_use_consumption,
				is_ark_tribute_item, is_from_crafting_consumption);
		}

		void Hook_UPrimalHarvestingComponent_GiveHarvestResource(UPrimalHarvestingComponent* _this, UPrimalInventoryComponent* inv_comp, float additional_effectiveness,
			TSubclassOf<UDamageType> damage_type_class, AActor* to_actor, TArray<FHarvestResourceEntry, FDefaultAllocator>* harvest_resource_entry_overrides)
		{
			if (to_actor && to_actor->IsA(AShooterCharacter::GetPrivateStaticClass())) {
				TeamItemType team = App->GetStorage()->Teams()->Get(to_actor->TargetingTeamField());

			if (team && App->GetProtManager()->IsProtectionActive(team, ProtectionType::AP))
					App->GetHVManager()->SetBonus(team);
			}

			UPrimalHarvestingComponent_GiveHarvestResource_original(_this, inv_comp, additional_effectiveness, damage_type_class, to_actor, harvest_resource_entry_overrides);
		}

		void RegisterHooks()
		{
			auto& hooks = ArkApi::GetHooks();

			hooks.SetHook("APrimalCharacter.TakeDamage", &Hook_APrimalCharacter_TakeDamage, &APrimalCharacter_TakeDamage_original);
			hooks.SetHook("APrimalStructure.TakeDamage", &Hook_APrimalStructure_TakeDamage, &APrimalStructure_TakeDamage_original);
			hooks.SetHook("AShooterGameMode.GenerateTribeId", &Hook_AShooterGameMode_GenerateTribeId, &AShooterGameMode_GenerateTribeId_original);
			hooks.SetHook("AShooterPlayerState.AddToTribe", &Hook_AShooterPlayerState_AddToTribe, &AShooterPlayerState_AddToTribe_original);
			hooks.SetHook("AShooterPlayerState.ServerRequestLeaveTribe_Implementation", &Hook_AShooterPlayerState_ServerRequestLeaveTribe_Implementation,
				&AShooterPlayerState_ServerRequestLeaveTribe_Implementation_original);
			hooks.SetHook("AShooterGameMode.HandleNewPlayer_Implementation", &Hook_AShooterGameMode_HandleNewPlayer_Implementation, &AShooterGameMode_HandleNewPlayer_Implementation_original);
			hooks.SetHook("AShooterGameMode.Logout", &Hook_AShooterGameMode_Logout, &AShooterGameMode_Logout_original);
			hooks.SetHook("AShooterGameMode.SaveWorld", &Hook_AShooterGameMode_SaveWorld, &AShooterGameMode_SaveWorld_original);
			hooks.SetHook("AShooterPlayerState.AcceptJoinAlliance", &Hook_AShooterPlayerState_AcceptJoinAlliance, &AShooterPlayerState_AcceptJoinAlliance_original);
			hooks.SetHook("APrimalDinoCharacter.CanCarryCharacter", &Hook_APrimalDinoCharacter_CanCarryCharacter, &APrimalDinoCharacter_CanCarryCharacter_original);
			hooks.SetHook("APrimalCharacter.CanDragCharacter", &Hook_APrimalCharacter_CanDragCharacter, &APrimalCharacter_CanDragCharacter_original);
			hooks.SetHook("AShooterCharacter.AllowGrappling_Implementation", &Hook_AShooterCharacter_AllowGrappling_Implementation, &AShooterCharacter_AllowGrappling_Implementation_original);
			hooks.SetHook("APrimalStructureTurret.CanFire", &Hook_APrimalStructureTurret_CanFire, &APrimalStructureTurret_CanFire_original);
			hooks.SetHook("APrimalStructureTurret.DealDamage", &Hook_APrimalStructureTurret_DealDamage, &APrimalStructureTurret_DealDamage_original);
			hooks.SetHook("AShooterPlayerController.ClientChatMessage", &Hook_AShooterPlayerController_ClientChatMessage, &AShooterPlayerController_ClientChatMessage_original);
			hooks.SetHook("UPrimalItem.IncrementItemQuantity", &Hook_UPrimalItem_IncrementItemQuantity, &UPrimalItem_IncrementItemQuantity_original);
			hooks.SetHook("UPrimalHarvestingComponent.GiveHarvestResource", &Hook_UPrimalHarvestingComponent_GiveHarvestResource, &UPrimalHarvestingComponent_GiveHarvestResource_original);
		}

		void UnregisterHooks()
		{
			auto& hooks = ArkApi::GetHooks();

			hooks.DisableHook("APrimalCharacter.TakeDamage", &Hook_APrimalCharacter_TakeDamage);
			hooks.DisableHook("APrimalStructure.TakeDamage", &Hook_APrimalStructure_TakeDamage);
			hooks.DisableHook("AShooterGameMode.GenerateTribeId", &Hook_AShooterGameMode_GenerateTribeId);
			hooks.DisableHook("AShooterPlayerState.AddToTribe", &Hook_AShooterPlayerState_AddToTribe);
			hooks.DisableHook("AShooterPlayerState.ServerRequestLeaveTribe_Implementation", &Hook_AShooterPlayerState_ServerRequestLeaveTribe_Implementation);
			hooks.DisableHook("AShooterGameMode.HandleNewPlayer_Implementation", &Hook_AShooterGameMode_HandleNewPlayer_Implementation);
			hooks.DisableHook("AShooterGameMode.Logout", &Hook_AShooterGameMode_Logout);
			hooks.DisableHook("AShooterGameMode.SaveWorld", &Hook_AShooterGameMode_SaveWorld);
			hooks.DisableHook("AShooterPlayerState.AcceptJoinAlliance", &Hook_AShooterPlayerState_AcceptJoinAlliance);
			hooks.DisableHook("APrimalDinoCharacter.CanCarryCharacter", &Hook_APrimalDinoCharacter_CanCarryCharacter);
			hooks.DisableHook("APrimalCharacter.CanDragCharacter", &Hook_APrimalCharacter_CanDragCharacter);
			hooks.DisableHook("AShooterCharacter.AllowGrappling_Implementation", &Hook_AShooterCharacter_AllowGrappling_Implementation);
			hooks.DisableHook("APrimalStructureTurret.CanFire", &Hook_APrimalStructureTurret_CanFire);
			hooks.DisableHook("APrimalStructureTurret.DealDamage", &Hook_APrimalStructureTurret_DealDamage);
			hooks.DisableHook("AShooterPlayerController.ClientChatMessage", &Hook_AShooterPlayerController_ClientChatMessage);
			hooks.DisableHook("UPrimalItem.IncrementItemQuantity", &Hook_UPrimalItem_IncrementItemQuantity);
			hooks.DisableHook("UPrimalHarvestingComponent.GiveHarvestResource", &Hook_UPrimalHarvestingComponent_GiveHarvestResource);
		}
}
}