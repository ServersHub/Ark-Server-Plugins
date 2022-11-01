#include "Application.h"

#include <Logger/Logger.h>
#include <DiscordMetrics/GogDiscordMetrics.h>

#include "../modules/TeamManager.h"
#include "../modules/EntityFactory.h"
#include "../modules/OnlineManager.h"
#include "../modules/StructureOverrides.h"
#include "../modules/BaseTimesManager.h"
#include "../modules/PenaltyManager.h"
#include "../modules/RankGroupsManager.h"
#include "../storage/Storage.h"
#include "../utils/Config.h"
#include "../utils/Helpers.h"
#include "../utils/ApiProxy.h"

namespace AutoDecay
{
	inline const std::string structure_destroyed_templ =
		"```Structure was destroyed:\\n"
		"Time: {}\\n"
		"TeamID: {}\\n"
		"Blueprint: {}\\n"
		"OfflineTime: {}```";

	Application::Application()
	{
	}

	Application::~Application()
	{
	}

	void Application::Initialize()
	{
		if (!app)
		{
			app = std::make_unique<Application>();
			app->InitializeImpl();
		}
	}

	void Application::Update()
	{
		if (app)
		{
			app->UpdateImpl();
		}
	}

	void Application::Shutdown()
	{
		if (app)
		{
			app->ShutdownImpl();
		}
	}

	Storage* Application::GetStorage()
	{
		return storage_;
	}

	TeamManager* Application::GetTeamManager()
	{
		return team_manager_;
	}

	EntityFactory* Application::GetFactory()
	{
		return entity_factory_;
	}

	OnlineManager* Application::GetOnlineManager()
	{
		return online_manager_;
	}

	PenaltyManager* Application::GetPenaltyManager()
	{
		return penalty_manager_;
	}

	FString Application::CheckTribeCommandImpl(int team_id)
	{
		Team team = storage_->Teams()->Get(team_id);
		if (!team)
		{
			return L"Team not found";
		}

		if (online_manager_->IsTeamOnline(team))
		{
			return GetMsg("CheckTribeOnline");
		}

		int seconds = GetBaseSettings(team)->structures;

		return FString::Format(*GetMsg("CheckTribe"), Helpers::Seconds2String(seconds).ToString());
	}

	void Application::InitializeImpl()
	{
		sync_storage_counter_ = TickCounter::Create(SYNC_DATABASE_INTERVAL);
		check_common_counter_ = TickCounter::Create(60);
		check_unclaimed_dinos_counter_ = TickCounter::Create(GetConfiguration()["CheckUnclaimedDinosInterval"] * 60);

		storage_ = new Storage();

		try {
			if (!storage_->Initialize(GetConfiguration()))
			{
				const std::string err = "Can't initialize Database Service";
				Log::GetLog()->error(err);
				throw std::runtime_error(err);
			}
		}
		catch (const std::exception&)
		{
			Log::GetLog()->warn("Error storage init");
		}

		team_manager_ = new TeamManager(storage_);
		entity_factory_ = new EntityFactory();
		online_manager_ = new OnlineManager(storage_);
		solo_overrides_ = new SoloOverrides(GetConfiguration()["SoloStructureOverrides"]);
		duo_overrides_ = new DuoOverrides(GetConfiguration()["DuoStructureOverrides"]);
		penalty_manager_ = new PenaltyManager(GetConfiguration()["Penalties"], storage_);
		rank_groups_manager_ = new RankGroupsManager(GetConfiguration()["RankGroups"], storage_);
		base_times_manager_ = new BaseTimesManager(GetConfiguration()["DecayTimings"]);
	}

	void Application::UpdateImpl()
	{
		sync_storage_counter_->Tick();
		if (sync_storage_counter_->IsReady())
		{
			storage_->SaveData();
			sync_storage_counter_->Reset();
		}

		check_unclaimed_dinos_counter_->Tick();
		if (check_unclaimed_dinos_counter_->IsReady())
		{
			if (GetConfiguration()["EnableAutoBreedingControl"].get<bool>())
			{
				CheckUnclaimedDinos();
			}
			check_unclaimed_dinos_counter_->Reset();
		}

		check_common_counter_->Tick();
		if (check_common_counter_->IsReady())
		{
			TArray<APrimalDinoCharacter*> dinos{};
			TArray<APrimalStructure*> structures{};
			SelectEntities(dinos, structures);

			for (auto it = storage_->Teams()->Begin(); it != storage_->Teams()->End(); it++)
			{
				penalty_manager_->ProcessTeam(*it);

				if (online_manager_->IsTeamOnline(*it) || (*it)->GetLastSeenTime() == 0)
				{
					continue;
				}

				RefPtr<TimesSettings> settings = GetBaseSettings((*it));

				if (GetConfiguration()["EnableStructuresDestroyer"].get<bool>())
				{
					CheckStructuresTeamOwned(*it, structures, settings);
				}
				int team_offline_time = static_cast<int>(time(0) - (*it)->GetLastSeenTime());
				if (team_offline_time > settings->dinos)
				{
					if (GetConfiguration()["EnableDinosDestroyer"].get<bool>())
					{
						DestroyTamedDinos((*it)->GetTeamId(), dinos);
					}
				}
			}
			check_common_counter_->Reset();
		}
	}

	void Application::ShutdownImpl()
	{
		if (storage_)
		{
			storage_->Shutdown();
			delete storage_;
		}

		auto safe_delete = [](auto* ptr)
		{
			if (ptr)
			{
				delete ptr;
			}
		};

		safe_delete(team_manager_);
		safe_delete(online_manager_);
		safe_delete(entity_factory_);
		safe_delete(solo_overrides_);
		safe_delete(duo_overrides_);
		safe_delete(penalty_manager_);
		safe_delete(rank_groups_manager_);
		safe_delete(base_times_manager_);
	}

	void Application::CheckStructuresTeamOwned(Team& team, TArray<APrimalStructure*>& structures, RefPtr<TimesSettings> base_settings)
	{
		int team_offline_time = static_cast<int>(time(0) - team->GetLastSeenTime());

		for (auto structure : structures)
		{
			if (structure && structure->TargetingTeamField() == team->GetTeamId())
			{
				int decay_time = FindOverrideTime(structure);

				if (decay_time == 0)
				{
					decay_time = base_settings->structures;
				}

				if (decay_time < team_offline_time)
				{
					const FString bp = Helpers::GetStructureBlueprint(structure);
					LogDestroyStructure(team->GetTeamId(), bp, team_offline_time);
					ArkApi::GetApiUtils().GetWorld()->DestroyActor(structure, true, true);
					ArkApi::GetApiUtils().GetWorld()->RemoveActor(structure, true);
				}
			}
		}
	}

	void Application::DestroyTamedDinos(int team_id, TArray<APrimalDinoCharacter*>& dinos)
	{
		for (auto dino : dinos)
		{
			if (dino && dino->TargetingTeamField() == team_id && !dino->bBabyInitiallyUnclaimed()())
			{
				ArkApi::GetApiUtils().GetWorld()->DestroyActor(dino, true, true);
				ArkApi::GetApiUtils().GetWorld()->RemoveActor(dino, true);
			}
		}
	}

	void Application::CheckUnclaimedDinos()
	{
		TArray<APrimalDinoCharacter*> dinos{};
		TArray<AActor*> actors = ArkApi::GetApiUtils().GetWorld()->PersistentLevelField()->GetActorsField();

		for (auto actor : actors)
		{
			if (actor && actor->IsA(APrimalDinoCharacter::GetPrivateStaticClass()) && actor->TargetingTeamField() >= 50000)
			{
				dinos.Push(reinterpret_cast<APrimalDinoCharacter*>(actor));
			}
		}

		int decay_time = GetConfiguration()["UnclaimedDinosDecayTime"] * 60;
		for (auto dino : dinos)
		{
			if (dino && dino->bBabyInitiallyUnclaimed()())
			{
				long double creation_time = dino->CreationTimeField();
				long double word_time = ArkApi::GetApiUtils().GetWorld()->TimeSecondsField();

				if (static_cast<int>(word_time - creation_time) > decay_time)
				{
					ArkApi::GetApiUtils().GetWorld()->DestroyActor(dino, true, true);
					ArkApi::GetApiUtils().GetWorld()->RemoveActor(dino, true);
				}
			}
		}
	}

	int Application::FindOverrideTime(APrimalStructure* target)
	{
		int result = 0;
		FString target_bp = Helpers::GetStructureBlueprint(target);

		if (target->bIsFoundation()())
		{
			result = solo_overrides_->GetOverride(target_bp);
		}
		else
		{
			TArray<APrimalStructure*> linked_structures = target->LinkedStructuresField();
			for (APrimalStructure* linked : linked_structures)
			{
				if (linked && linked->bIsFoundation()())
				{
					FString foundation_bp = Helpers::GetStructureBlueprint(linked);
					result = duo_overrides_->GetOverride(target_bp, foundation_bp);
					if (result > 0)
					{
						break;
					}
				}
			}

			if (result == 0)
			{
				result = solo_overrides_->GetOverride(target_bp);
			}
		}

		return result;
	}

	void Application::SelectEntities(TArray<APrimalDinoCharacter*>& dinos, TArray<APrimalStructure*>& structures)
	{
		TArray<AActor*> actors = ArkApi::GetApiUtils().GetWorld()->PersistentLevelField()->GetActorsField();

		for (auto actor : actors)
		{
			if (actor && actor->IsA(APrimalDinoCharacter::GetPrivateStaticClass()))
			{
				dinos.Push(reinterpret_cast<APrimalDinoCharacter*>(actor));
			}
			else if (actor && actor->IsA(APrimalStructure::GetPrivateStaticClass()))
			{
				structures.Push(reinterpret_cast<APrimalStructure*>(actor));
			}
		}
	}

	RefPtr<TimesSettings> Application::GetBaseSettings(Team& team)
	{
		RankGroup rank_group = rank_groups_manager_->GetGroup(team);
		RefPtr<TimesSettings> settings = base_times_manager_->GetSettings(rank_group.GetIdentifier());

		return settings;
	}

	void Application::LogDestroyStructure(int team_id, const FString& bp, int offline_time)
	{
		const std::string channel_id = GetConfiguration()["DiscordChannel"]["Tag"].get<std::string>();
		const std::string bot_name = GetConfiguration()["DiscordChannel"]["BotName"].get<std::string>();

		const std::string message = fmt::format(structure_destroyed_templ,
			DiscordMetrics::MakeTimeString(), team_id, bp.ToString(), offline_time);

		DiscordMetrics::DiscordPacket packet{ channel_id, bot_name, message };
		DiscordMetrics::QueuePacket(packet);
	}

	std::string GetLicenseKey()
	{
		const std::string configPath = ArkApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/AutoDecay/config.json";
		std::ifstream file{ configPath };

		if (!file.is_open())
		{
			throw std::runtime_error("Can't open config.json");
		}

		nlohmann::json j{};
		file >> j;
		file.close();

		if (!j.contains("/LicenseKey"_json_pointer))
		{
			throw std::runtime_error("Can't load license key");
		}

		return j["/LicenseKey"_json_pointer].get<std::string>();
	}
}