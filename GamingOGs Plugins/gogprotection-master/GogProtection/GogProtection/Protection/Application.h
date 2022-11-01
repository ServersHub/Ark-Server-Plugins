#ifndef _GOG_PROT_APPLICATION_H_
#define _GOG_PROT_APPLICATION_H_

#include <memory>
#include <string>

namespace Protection
{
	class Storage;
	class OnlineMonitor;
	class EntityFactory;
	class TeamManager;
	class HarvestManager;
	class ProtManager;
	class BlackList;
	class ProtectionCommonRules;
	class APTimingRules;
	class ORPTimingRules;
	class DiscordLogger;
	class NotificationsManager;
	class DragManager;
	class ArenaManager;

	class Application
	{
	public:
		Application();
		~Application();

		bool Initialize();
		bool Reload();
		bool OnSaveWorld();
		void Shutdown();

		Storage* GetStorage();
		OnlineMonitor* GetOnlineMonitor();
		EntityFactory* GetFactory();
		TeamManager* GetTeamManager();
		HarvestManager* GetHVManager();
		ProtManager* GetProtManager();
		BlackList* GetBlackList();
		DiscordLogger* GetDiscordLogger();
		NotificationsManager* GetNotificationsManager();
		DragManager* GetDragManager();
		ArenaManager* GetArenaManager();

	private:
		void CreateProtectionRules();
		void ParseProtectionRules();

		Storage* storage_								= nullptr;
		OnlineMonitor* online_monitor_					= nullptr;
		EntityFactory* entity_factory_					= nullptr;
		TeamManager* team_manager_						= nullptr;
		HarvestManager* harvest_manager_				= nullptr;
		ProtManager* prot_manager_						= nullptr;
		BlackList* black_list_							= nullptr;
		DiscordLogger* discord_logger_					= nullptr;
		NotificationsManager* notifications_manager_	= nullptr;
		DragManager* drag_manager_						= nullptr;
		ArenaManager* arena_manager_					= nullptr;
		ProtectionCommonRules* protection_common_rules_ = nullptr;
		APTimingRules* ap_timing_rules_					= nullptr;
		ORPTimingRules* orp_timing_rules_				= nullptr;
	};

	inline std::unique_ptr<Application> App = nullptr;

	std::string GetLicenseKey();
}

#endif // !_GOG_PROT_APPLICATION_H_
