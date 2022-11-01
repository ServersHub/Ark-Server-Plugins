#include "ProtManager.h"
#include <vector>

namespace Protection
{
	ProtManager::ProtManager(Storage* storage, APTimingRules* ap_rules, ORPTimingRules* orp_rules, ProtectionCommonRules* common_rules)
		: storage_(storage), common_rules_(common_rules), ap_timing_rules_(ap_rules), orp_timing_rules_(orp_rules), no_orp_mode_enabled_(false), no_orp_mode_enabled_until_(0)
	{
	}

	ProtManager::~ProtManager()
	{
	}

	bool ProtManager::IsProtectionEnabled(EntityType entity_type, ProtectionType protection_type)
	{
		switch (protection_type)
		{
		case ProtectionType::AP:
			return IsAPEnabled(entity_type);
		case ProtectionType::ORP:
			return IsORPEnabled(entity_type);
		default:
			return false;
		}
	}

	float ProtManager::GetProtectionPercent(EntityType entity_type, ProtectionType protection_type)
	{
		switch (protection_type)
		{
		case ProtectionType::AP:
			return GetAPPercent(entity_type);
		case ProtectionType::ORP:
			return GetORPPercent(entity_type);
		default:
			return 0.0f;
		}
	}

	bool ProtManager::IsProtectionActive(TeamItemType& team, ProtectionType type)
	{
		if (!team)
			return false;

		if (type == ProtectionType::AP)
			return IsAPActive(team);
		else if (type == ProtectionType::ORP)
			return IsORPActive(team);
		else
			return false;
	}

	unsigned long long ProtManager::GetProtectionRemainingTime(TeamItemType& team, ProtectionType type)
	{
		if (!team)
			return 0ULL;

		if (type == ProtectionType::AP)
			return GetAPRemainingTime(team);
		else if (type == ProtectionType::ORP)
			return GetORPRemainingTime(team);
		else
			return 0ULL;
	}

	void ProtManager::AddAPSeconds(TeamItemType& team, int seconds)
	{
		if (!team)
			return;

		const unsigned long long new_deactivation_time = team->GetAPDeactivationTime() + std::abs(seconds);
		team->SetAPDeactivationTime(new_deactivation_time);
	}

	void ProtManager::SetAPSeconds(TeamItemType& team, int seconds)
	{
		if (!team)
			return;

		const unsigned long long activation_time = time(0);

		team->SetAPActivationTime(activation_time);
		team->SetAPDeactivationTime(activation_time + std::abs(seconds));
	}

	void ProtManager::DisableAP(TeamItemType& team)
	{
		if (!team)
			return;

		const unsigned long long diff_time = team->GetAPDeactivationTime() - team->GetAPActivationTime();
		const unsigned long long new_deactivation_time = static_cast<unsigned long long>(time(0) - 1);
		const unsigned long long new_activation_time = static_cast<unsigned long long>(new_deactivation_time - diff_time);

		team->SetAPActivationTime(new_activation_time);
		team->SetAPDeactivationTime(new_deactivation_time);
		
	}

	void ProtManager::ResetAP(TeamItemType& team)
	{
		if (!team)
			return;

		const unsigned long long new_activation_time = time(0);
		const unsigned long long new_deactivation_time = new_activation_time + ap_timing_rules_->GetAPDefaultDurationSec();

		team->SetAPActivationTime(new_activation_time);
		team->SetAPDeactivationTime(new_deactivation_time);
	}

	void ProtManager::EnableNoORPMode(int seconds)
	{
		no_orp_mode_enabled_ = true;
		no_orp_mode_enabled_until_ = time(0) + seconds;
	}

	void ProtManager::DisableNoORPMode()
	{
		no_orp_mode_enabled_ = false;
		no_orp_mode_enabled_until_ = 0;
	}

	void ProtManager::ORPCheckOnTeamOnline(TeamItemType& team)
	{
		if (!team)
			return;

		team->SetORPActivationTime(0);
		team->SetORPDeactivationTime(0);
	}

	void ProtManager::ORPCheckOnTeamOffline(TeamItemType& team)
	{
		if (!team)
			return;

		unsigned long long now = time(0);

		unsigned long long time_since_last_battle;
		unsigned long long new_orp_activation_time;
		unsigned long long new_orp_deactivation_time;

		if (team->GetLastCombatTime() == 0ULL)
			time_since_last_battle = 0ULL;
		else
			time_since_last_battle = now - team->GetLastCombatTime();

		if (time_since_last_battle <= orp_timing_rules_->GetNoBattleTimeSec())
			new_orp_activation_time = now + orp_timing_rules_->GetLongDelaySec();
		else
			new_orp_activation_time = now + orp_timing_rules_->GetNormalDelaySec();

		new_orp_deactivation_time = new_orp_activation_time + orp_timing_rules_->GetMaxDurationSec();

		team->SetORPActivationTime(new_orp_activation_time);
		team->SetORPDeactivationTime(new_orp_deactivation_time);

		team->SetLastCombatTime(0ULL);
	}

	void ProtManager::TeamInCombat(TeamItemType& team)
	{
		if (!team)
			return;

		team->SetLastCombatTime(time(0));
	}

	bool ProtManager::IsORPZonesLimitExceeded(TeamItemType& team)
	{
		if (!team)
			return true;

		const size_t count = team->GetORPZones().size();
		return count >= common_rules_->GetRangeBasedORPMaxLocations();
	}

	void ProtManager::ORPAddZone(TeamItemType& team, const std::string& id, FVector* pivot)
	{
		if (!team)
			return;

		std::vector<ORPZone>& zones = team->GetORPZones();
		zones.push_back(ORPZone{ id, pivot->X, pivot->Y, pivot->Z });
	}

	bool ProtManager::ORPRemoveZone(TeamItemType& team, const std::string& id)
	{
		if (!team)
			return false;

		std::vector<ORPZone>& zones = team->GetORPZones();

		auto remove_cbk = [&](const ORPZone& zone)
		{
			return zone.id == id;
		};

		const auto iter = std::remove_if(zones.begin(), zones.end(), remove_cbk);
		if (iter != zones.end()) {
			zones.erase(iter);
			return true;
		}

		return false;
	}

	bool ProtManager::ORPCheckRangeHit(TeamItemType& team, FVector* pivot)
	{
		if (!team || !common_rules_->GetRangeBasedORPFlag())
			return true;

		const std::vector<ORPZone>& zones = team->GetORPZones();

		for (const auto& zone : zones) {
			const float distance = FVector::Distance(*pivot, FVector{ zone.x, zone.y, zone.z });
			if (distance <= common_rules_->GetRangeBasedORPDistance())
				return true;
		}

		return false;
	}

	void ProtManager::ORPTransferZones(TeamItemType& from, TeamItemType& to)
	{
		if (!from || !to)
			return;

		std::vector<ORPZone>& from_zones = from->GetORPZones();
		std::vector<ORPZone>& to_zones = to->GetORPZones();

		to_zones.clear();
		to_zones.insert(to_zones.end(), from_zones.begin(), from_zones.end());
	}

	void ProtManager::ORPRemoveZones(TeamItemType& team)
	{
		if (!team)
			return;

		team->GetORPZones().clear();
	}

	std::string ProtManager::ORPGetZonesInfo(TeamItemType& team)
	{
		if (!team)
			return "";

		const std::string templ = "ID: {}, Middle Point: {}:{}:{}\n";
		std::string out{};

		const std::vector<ORPZone> zones = team->GetORPZones();

		for (const auto& zone : zones)
			out += fmt::format(templ, zone.id, zone.x, zone.y, zone.z);

		return out;
	}

	std::string ProtManager::GetActiveORPZoneId(TeamItemType& team, FVector* pivot)
	{
		if (!team || !common_rules_->GetRangeBasedORPFlag())
			return "";

		const std::vector<ORPZone>& zones = team->GetORPZones();

		for (const auto& zone : zones) {
			const float distance = FVector::Distance(*pivot, FVector{ zone.x, zone.y, zone.z });
			if (distance <= common_rules_->GetRangeBasedORPDistance())
				return zone.id;
		}

		return "";
	}

	bool ProtManager::IsAPEnabled(EntityType entity_type)
	{
		switch (entity_type)
		{
		case EntityType::Character:
			return common_rules_->GetPlayersAPFlag();
		case EntityType::Dino:
			return common_rules_->GetDinosAPFlag();
		case EntityType::Structure:
			return common_rules_->GetStructuresAPFlag();
		default:
			return false;
		}
	}

	bool ProtManager::IsORPEnabled(EntityType entity_type)
	{
		switch (entity_type)
		{
		case EntityType::Character:
			return common_rules_->GetPlayersORPFlag();
		case EntityType::Dino:
			return common_rules_->GetDinosORPFlag();
		case EntityType::Structure:
			return common_rules_->GetStructuresORPFlag();
		default:
			return false;
		}
	}

	float ProtManager::GetAPPercent(EntityType entity_type)
	{
		switch (entity_type)
		{
		case EntityType::Character:
			return common_rules_->GetPlayersAPPercent();
		case EntityType::Dino:
			return common_rules_->GetDinosAPPercent();
		case EntityType::Structure:
			return common_rules_->GetStructuresAPPercent();
		default:
			return false;
		}
	}

	float ProtManager::GetORPPercent(EntityType entity_type)
	{
		switch (entity_type)
		{
		case EntityType::Character:
			return common_rules_->GetPlayersORPPercent();
		case EntityType::Dino:
			return common_rules_->GetDinosORPPercent();
		case EntityType::Structure:
			return common_rules_->GetStructuresORPPercent();
		default:
			return false;
		}
	}

	bool ProtManager::IsAPActive(TeamItemType& team)
	{
		if (team->GetAPDeactivationTime() >= static_cast<unsigned long long>(time(0)))
			return true;
		else
			return false;
	}

	bool ProtManager::IsORPActive(TeamItemType& team)
	{
		if (IsNoORPModeEnabled())
			return false;

		if (team->GetORPDeactivationTime() >= static_cast<unsigned long long>(time(0)))
			return true;
		else
			return false;
	}

	bool ProtManager::IsNoORPModeEnabled()
	{
		if (!no_orp_mode_enabled_)
			return false;

		if (no_orp_mode_enabled_until_ < time(0)) {
			no_orp_mode_enabled_ = false;
			no_orp_mode_enabled_until_ = 0;
			return false;
		}

		return true;
	}

	unsigned long long ProtManager::GetAPRemainingTime(TeamItemType& team)
	{
		if (team->GetAPDeactivationTime() == 0ULL)
			return 0ULL;

		const unsigned long long remaining_seconds = team->GetAPDeactivationTime() - time(0);

		return (remaining_seconds > 0) ? remaining_seconds : 0ULL;
	}

	unsigned long long ProtManager::GetORPRemainingTime(TeamItemType& team)
	{
		if (team->GetORPDeactivationTime() == 0ULL)
			return 0ULL;

		const unsigned long long remaining_seconds = team->GetORPDeactivationTime() - time(0);

		return (remaining_seconds > 0) ? remaining_seconds : 0ULL;
	}
}