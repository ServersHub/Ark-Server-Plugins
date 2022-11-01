#ifndef _GOG_PROT_PROT_MANAGER_H_
#define _GOG_PROT_PROT_MANAGER_H_

#include "../Storage/Storage.h"
#include <API/UE/Math/Vector.h>

namespace Protection
{
	enum class EntityType
	{
		Character = 0,
		Dino =		1,
		Structure = 2,
		Turret =	3,
		Unknown =	4,
	};

	enum class ProtectionType
	{
		AP =	0,
		ORP =	1
	};

	class ProtManager
	{
	public:
		ProtManager(Storage* storage, APTimingRules* ap_rules, ORPTimingRules* orp_rules, ProtectionCommonRules* common_rules);
		~ProtManager();

		bool IsProtectionEnabled(EntityType entity_type, ProtectionType protection_type);
		float GetProtectionPercent(EntityType entity_type, ProtectionType protection_type);
		bool IsProtectionActive(TeamItemType& team, ProtectionType type);
		unsigned long long GetProtectionRemainingTime(TeamItemType& team, ProtectionType type);
		void AddAPSeconds(TeamItemType& team, int seconds);
		void SetAPSeconds(TeamItemType& team, int seconds);
		void DisableAP(TeamItemType& team);
		void ResetAP(TeamItemType& team);
		void EnableNoORPMode(int seconds);
		void DisableNoORPMode();
		void ORPCheckOnTeamOnline(TeamItemType& team);
		void ORPCheckOnTeamOffline(TeamItemType& team);
		void TeamInCombat(TeamItemType& team);
		bool IsORPZonesLimitExceeded(TeamItemType& team);
		void ORPAddZone(TeamItemType& team, const std::string& id, FVector* pivot);
		bool ORPRemoveZone(TeamItemType& team, const std::string& id);
		bool ORPCheckRangeHit(TeamItemType& team, FVector* pivot);
		void ORPTransferZones(TeamItemType& from, TeamItemType& to);
		void ORPRemoveZones(TeamItemType& team);
		std::string ORPGetZonesInfo(TeamItemType& team);
		std::string GetActiveORPZoneId(TeamItemType& team, FVector* pivot);


	private:
		bool IsAPEnabled(EntityType entity_type);
		bool IsORPEnabled(EntityType entity_type);
		float GetAPPercent(EntityType entity_type);
		float GetORPPercent(EntityType entity_type);
		bool IsAPActive(TeamItemType& team);
		bool IsORPActive(TeamItemType& team);
		bool IsNoORPModeEnabled();
		unsigned long long GetAPRemainingTime(TeamItemType& team);
		unsigned long long GetORPRemainingTime(TeamItemType& team);

		Storage* storage_;
		ProtectionCommonRules* common_rules_;
		APTimingRules* ap_timing_rules_;
		ORPTimingRules* orp_timing_rules_;
		bool no_orp_mode_enabled_;
		time_t no_orp_mode_enabled_until_;
	};
}

#endif // !_GOG_PROT_PROT_MANAGER_H_
