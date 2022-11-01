#include "Application.h"

#include <Logger/Logger.h>

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

namespace Protection
{
	Application::Application()
	{
	}

	Application::~Application()
	{
	}

	bool Application::Initialize()
	{
		if (!ParseConfig()) {
			Log::GetLog()->error("Error is occurred during read configuration");
			return false;
		}

		storage_ = new Storage();

		if (!storage_->Initialize()) {
			Log::GetLog()->error("Error is occurred during initialize storage");
			return false;
		}

		CreateProtectionRules();
		ParseProtectionRules();

		entity_factory_ = new EntityFactory(ap_timing_rules_);
		prot_manager_ = new ProtManager(storage_, ap_timing_rules_, orp_timing_rules_, protection_common_rules_);
		team_manager_ = new TeamManager(storage_);
		online_monitor_ = new OnlineMonitor(storage_);
		harvest_manager_ = new HarvestManager(static_cast<float>(GetConfig()["AP"]["General"]["HarvestModifier"]));
		black_list_ = new BlackList(storage_);
		discord_logger_ = new DiscordLogger(GetDiscordChannelData());
		notifications_manager_ = new NotificationsManager(ap_timing_rules_, orp_timing_rules_);
		drag_manager_ = new DragManager(GetConfig()["FlyerDinoCarryExceptions"],
			GetConfig()["GroundDinoDragExceptions"],
			GetConfig()["AP"]["General"]["CanCarryProtected"].get<bool>(),
			GetConfig()["AP"]["General"]["CanDragProtected"].get<bool>());
		arena_manager_ = new ArenaManager(GetConfig()["BossArenas"]);

		return true;
	}

	bool Application::Reload()
	{
		if (!ParseConfig())
			return false;

		ParseProtectionRules();

		return true;
	}

	bool Application::OnSaveWorld()
	{
		storage_->SaveData();
		return true;
	}

	void Application::Shutdown()
	{
		if (black_list_)
			delete black_list_;

		if (harvest_manager_)
			delete harvest_manager_;

		if (online_monitor_)
			delete online_monitor_;

		if (team_manager_)
			delete team_manager_;

		if (prot_manager_)
			delete prot_manager_;

		if (entity_factory_)
			delete entity_factory_;

		if (discord_logger_)
			delete discord_logger_;

		if (notifications_manager_)
			delete notifications_manager_;

		if (drag_manager_)
			delete drag_manager_;

		if (arena_manager_)
			delete arena_manager_;

		if (storage_) {
			storage_->SaveData();
			delete storage_;
		}

		if (protection_common_rules_)
			delete protection_common_rules_;

		if (ap_timing_rules_)
			delete ap_timing_rules_;

		if (orp_timing_rules_)
			delete orp_timing_rules_;
	}

	Storage* Application::GetStorage()
	{
		return storage_;
	}

	OnlineMonitor* Application::GetOnlineMonitor()
	{
		return online_monitor_;
	}

	EntityFactory* Application::GetFactory()
	{
		return entity_factory_;
	}

	TeamManager* Application::GetTeamManager()
	{
		return team_manager_;
	}

	HarvestManager* Application::GetHVManager()
	{
		return harvest_manager_;
	}

	ProtManager* Application::GetProtManager()
	{
		return prot_manager_;
	}

	BlackList* Application::GetBlackList()
	{
		return black_list_;
	}

	DiscordLogger* Application::GetDiscordLogger()
	{
		return discord_logger_;
	}

	NotificationsManager* Application::GetNotificationsManager()
	{
		return notifications_manager_;
	}

	DragManager* Application::GetDragManager()
	{
		return drag_manager_;
	}

	ArenaManager* Application::GetArenaManager()
	{
		return arena_manager_;
	}

	void Application::CreateProtectionRules()
	{
		protection_common_rules_ = new ProtectionCommonRules();
		ap_timing_rules_ = new APTimingRules();
		orp_timing_rules_ = new ORPTimingRules();
	}
	void Application::ParseProtectionRules()
	{
		protection_common_rules_->SetPlayersORPFlag(GetConfig()["ORP"]["ProtectPlayers"]);
		protection_common_rules_->SetDinosORPFlag(GetConfig()["ORP"]["ProtectDinos"]);
		protection_common_rules_->SetStructuresORPFlag(GetConfig()["ORP"]["ProtectStructures"]);

		protection_common_rules_->SetPlayersAPFlag(GetConfig()["AP"]["General"]["ProtectPlayers"]);
		protection_common_rules_->SetDinosAPFlag(GetConfig()["AP"]["General"]["ProtectDinos"]);
		protection_common_rules_->SetStructuresAPFlag(GetConfig()["AP"]["General"]["ProtectStructures"]);

		protection_common_rules_->SetPlayersORPPercent(GetConfig()["ORP"]["PlayersProtectionPercent"]);
		protection_common_rules_->SetDinosORPPercent(GetConfig()["ORP"]["DinosProtectionPercent"]);
		protection_common_rules_->SetStructuresORPPercent(GetConfig()["ORP"]["StructuresProtectionPercent"]);

		protection_common_rules_->SetPlayersAPPercent(GetConfig()["AP"]["General"]["PlayersProtectionPercent"]);
		protection_common_rules_->SetDinosAPPercent(GetConfig()["AP"]["General"]["DinosProtectionPercent"]);
		protection_common_rules_->SetStructuresAPPercent(GetConfig()["AP"]["General"]["StructuresProtectionPercent"]);

		protection_common_rules_->SetRangeBasedORPFlag(GetConfig()["ORP"]["EnableRangeBasedORP"]);
		protection_common_rules_->SetDefaultORPPercentOverridenFlag(GetConfig()["ORP"]["OverrideDefaultPercent"]);
		protection_common_rules_->SetRangeBasedORPMaxLocations(GetConfig()["ORP"]["MaxLocations"]);
		protection_common_rules_->SetRangeBasedORPDistance(GetConfig()["ORP"]["MaxRange"]);

		auto orp_overrides = protection_common_rules_->GetORPOverrides();
		for (const auto& conf_element : GetConfig()["ORP"]["PercentOverrides"])
		{
			ORPPercentOverrideData data{
				conf_element["RangeBegin"],
				conf_element["RangeEnd"],
				conf_element["PlayersProtectionPercent"],
				conf_element["DinosProtectionPercent"],
				conf_element["StructuresProtectionPercent"]
			};
			orp_overrides.push_back(data);
		}

		ap_timing_rules_->SetAPDefaultDurationSec(static_cast<int>(GetConfig()["AP"]["General"]["DefaultDuration"]) * 60);
		ap_timing_rules_->SetPlayersNotificationsIntervalSec(static_cast<int>(GetConfig()["AP"]["General"]["PlayersNotificationsInterval"]));
		ap_timing_rules_->SetDinosNotificationsIntervalSec(static_cast<int>(GetConfig()["AP"]["General"]["DinosNotificationsInterval"]));
		ap_timing_rules_->SetStructuresNotificationsIntervalSec(static_cast<int>(GetConfig()["AP"]["General"]["StructuresNotificationsInterval"]));

		orp_timing_rules_->SetMaxDurationSec(static_cast<int>(GetConfig()["ORP"]["MaxDuration"]) * 60);
		orp_timing_rules_->SetNormalDelaySec(static_cast<int>(GetConfig()["ORP"]["ShortDelay"]) * 60);
		orp_timing_rules_->SetLongDelaySec(static_cast<int>(GetConfig()["ORP"]["LongDelay"]) * 60);
		orp_timing_rules_->SetNoBattleTimeSec(static_cast<int>(GetConfig()["ORP"]["NoBattleTime"]) * 60);
		orp_timing_rules_->SetPlayersNotificationsIntervalSec(static_cast<int>(GetConfig()["ORP"]["PlayersNotificationsInterval"]));
		orp_timing_rules_->SetDinosNotificationsIntervalSec(static_cast<int>(GetConfig()["ORP"]["DinosNotificationsInterval"]));
		orp_timing_rules_->SetStructuresNotificationsIntervalSec(static_cast<int>(GetConfig()["ORP"]["StructuresNotificationsInterval"]));
	}

	std::string GetLicenseKey()
	{
		const std::string configPath = ArkApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/PlayerProtection/config.json";
		std::ifstream file{ configPath };

		if (!file.is_open())
		{
			throw std::runtime_error("Can't open config.json");
		}

		nlohmann::json j;
		file >> j;
		file.close();

		if (!j.contains("/LicenseKey"_json_pointer))
		{
			throw std::runtime_error("Can't load license key");
		}

		return j["/LicenseKey"_json_pointer].get<std::string>();
	}
}