#ifndef _GOG_PROT_NOTIFICATIONS_MANAGER_H_
#define _GOG_PROT_NOTIFICATIONS_MANAGER_H_

#include <vector>
#include <array>

#include "../Storage/Storage.h"
#include "ProtManager.h"

namespace Protection
{

	class NotificationsManager
	{
		class PlayerTimings
		{
			using Timing = std::pair<void*, std::time_t>;
			using TimingsList = std::vector<Timing>;

		public:
			PlayerTimings(NotificationsManager* manager, PlayerItemType player);

			~PlayerTimings() = default;

			PlayerItemType GetPlayer();

			bool IsAllowed(ProtectionType protection_type, EntityType entity_type, void* ptr);
			void OnNotified(ProtectionType protection_type, EntityType entity_type, void* ptr);
			void Update(std::time_t current_time);

		private:
			TimingsList& GetTimingsList(ProtectionType protection_type, EntityType entity_type);
			Timing* FindTiming(TimingsList& list, void* ptr);
			int GetInterval(ProtectionType protection_type, EntityType entity_type);
			void UpdateTimingsList(TimingsList& list, std::time_t current_time, int interval);

			NotificationsManager* manager_;
			PlayerItemType player_;
			std::array<std::array<TimingsList, 3>, 2> matrix_;
		};

	public:
		NotificationsManager(APTimingRules* ap_rules, ORPTimingRules* orp_rules);
		~NotificationsManager();

		bool IsAllowed(const PlayerItemType& player, ProtectionType protection_type, EntityType entity_type, void* ptr);
		void OnNotified(PlayerItemType player, ProtectionType protection_type, EntityType entity_type, void* ptr);
		void Update();

	private:
		int GetInterval(ProtectionType protection_type, EntityType entity_type) const;
		int GetAPInterval(EntityType entity_type) const;
		int GetORPInterval(EntityType entity_type) const;
		PlayerTimings* FindPlayer(const PlayerItemType& player);

		APTimingRules* ap_rules_;
		ORPTimingRules* orp_rules_;
		std::time_t last_check_;
		std::vector<PlayerTimings> players_;
	};

}

#endif // !_GOG_PROT_NOTIFICATIONS_MANAGER_H_