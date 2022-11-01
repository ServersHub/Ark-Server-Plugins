#pragma once

#include "../storage/TeamProps.h"
#include "../utils/Types.h"
#include "../utils/ApiProxy.h"

namespace AutoDecay
{
	constexpr unsigned int SYNC_DATABASE_INTERVAL = 60;

	class Storage;
	class TeamManager;
	class EntityFactory;
	class OnlineManager;
	class PenaltyManager;
	class RankGroupsManager;
	class BaseTimesManager;
	class SoloOverrides;
	class DuoOverrides;
	class TimesSettings;

	struct TickCounter
	{
		TickCounter(__int64 interval) : interval_(interval), counter_(0I64)
		{
		}

		static ScopePtr<TickCounter> Create(__int64 interval)
		{
			return std::make_unique<TickCounter>(interval);
		}

		bool IsReady() const
		{
			return counter_ >= interval_;
		}

		void Tick()
		{
			counter_++;
		}

		void Reset()
		{
			counter_ = 0I64;
		}

		__int64 interval_;
		__int64 counter_;
	};

	class Application
	{
	public:
		Application();
		~Application();

		static void Initialize();
		static void Update();
		static void Shutdown();

		Storage* GetStorage();
		TeamManager* GetTeamManager();
		EntityFactory* GetFactory();
		OnlineManager* GetOnlineManager();
		PenaltyManager* GetPenaltyManager();

		FString CheckTribeCommandImpl(int team_id);

	private:
		void InitializeImpl();
		void UpdateImpl();
		void ShutdownImpl();
		void CheckStructuresTeamOwned(Team& team, TArray<APrimalStructure*>& structures, RefPtr<TimesSettings> base_settings);
		void DestroyTamedDinos(int team_id, TArray<APrimalDinoCharacter*>& dinos);
		void CheckUnclaimedDinos();
		int FindOverrideTime(APrimalStructure* target);
		void SelectEntities(TArray<APrimalDinoCharacter*>& dinos, TArray<APrimalStructure*>& structures);
		RefPtr<TimesSettings> GetBaseSettings(Team& team);
		void LogDestroyStructure(int team_id, const FString& bp, int offline_time);

		Storage* storage_;
		TeamManager* team_manager_;
		EntityFactory* entity_factory_;
		OnlineManager* online_manager_;
		PenaltyManager* penalty_manager_;
		RankGroupsManager* rank_groups_manager_;
		BaseTimesManager* base_times_manager_;
		SoloOverrides* solo_overrides_;
		DuoOverrides* duo_overrides_;

		ScopePtr<TickCounter> sync_storage_counter_;
		ScopePtr<TickCounter> check_common_counter_;
		ScopePtr<TickCounter> check_unclaimed_dinos_counter_;
	};

	inline std::unique_ptr<Application> app = nullptr;

	std::string GetLicenseKey();
}