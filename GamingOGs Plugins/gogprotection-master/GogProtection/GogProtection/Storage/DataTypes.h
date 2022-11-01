#ifndef _GOG_PROT_PRIMAL_TYPES_H_
#define _GOG_PROT_PRIMAL_TYPES_H_

#include <vector>
#include "..\API\ARK\Ark.h"

namespace Protection
{
	struct ORPZone
	{
		ORPZone()
			: id(""), x(0.0f), y(0.0f), z(0.0f)
		{
		}

		ORPZone(const std::string& id, float x, float y, float z) :
			id(id), x(x), y(y), z(z)
		{
		}

		std::string id;
		float x, y, z;
	};

	struct ORPPercentOverrideData
	{
		ORPPercentOverrideData()
			: range_begin(0), range_end(0), players_percent(0.0f), dinos_percent(0.0f), structures_percent(0.0f)
		{
		}

		ORPPercentOverrideData(int range_begin, int range_end, float players_percent, float dinos_percent, float structures_percent)
			: range_begin(range_begin), range_end(range_end), players_percent(players_percent), dinos_percent(dinos_percent), structures_percent(structures_percent)
		{
		}

		int range_begin;
		int range_end;
		float players_percent;
		float dinos_percent;
		float structures_percent;
	};

	class PlayerData
	{
	private:
		unsigned long long steam_id_;
		int team_id_;
		bool is_default_blacklisted_;
		bool is_online_;

	public:
		PlayerData()
			: steam_id_(0ULL), team_id_(0), is_default_blacklisted_(false), is_online_(false)
		{
		}

		PlayerData(unsigned long long steam_id, int team_id, bool blacklist_flag, bool online_flag)
			: steam_id_(steam_id), team_id_(team_id), is_default_blacklisted_(blacklist_flag), is_online_(online_flag)
		{
		}

		~PlayerData()
		{
		}

		bool operator==(const unsigned long long key) { return steam_id_ == key; }

		inline void SetSteamId(unsigned long long steam_id)		{ steam_id_ = steam_id; }
		inline void SetTeamId(int team_id)						{ team_id_ = team_id; }
		inline void SetBlackListFlag(bool flag)					{ is_default_blacklisted_ = flag; }
		inline void SetOnlineFlag(bool flag)					{ is_online_ = flag; }

		inline unsigned long long GetKeyField() const			{ return GetSteamId(); }
		inline unsigned long long GetSteamId() const			{ return steam_id_; }
		inline int GetTeamId() const							{ return team_id_; }
		inline bool GetBlackListFlag() const					{ return is_default_blacklisted_; }
		inline bool GetOnlineFlag() const						{ return is_online_; }
	};

	class TeamData
	{
	private:
		int team_id_;
		unsigned long long orp_activation_time_;
		unsigned long long orp_deactivation_time_;
		unsigned long long ap_activation_time_;
		unsigned long long ap_deactivation_time_;
		unsigned long long last_combat_time_;
		bool is_blacklisted_;
		bool harvest_flag_;
		std::vector<ORPZone> orp_zones_;

	public:
		TeamData()
			: team_id_(0), orp_activation_time_(0ULL), orp_deactivation_time_(0ULL), ap_activation_time_(0ULL), ap_deactivation_time_(0ULL),
			last_combat_time_(0ULL), is_blacklisted_(false), harvest_flag_(false), orp_zones_({})
		{
		}

		TeamData(int team_id, unsigned long long orp_activation_time, unsigned long long orp_deactivation_time, unsigned long long ap_activation_time, unsigned long long ap_deactivation_time,
			unsigned long long last_combat_time, const std::vector<ORPZone>& orp_zones, bool blacklist_flag, bool harvest_flag)
			: team_id_(team_id), orp_activation_time_(orp_activation_time), orp_deactivation_time_(orp_deactivation_time), ap_activation_time_(ap_activation_time), ap_deactivation_time_(ap_deactivation_time),
			last_combat_time_(last_combat_time), is_blacklisted_(blacklist_flag), harvest_flag_(harvest_flag), orp_zones_(orp_zones)
		{
		}

		~TeamData()
		{
		}

		inline bool operator==(const int key) const { return team_id_ == key; }

		inline void SetTeamId(int team_id)												{ team_id_ = team_id; }
		inline void SetORPActivationTime(unsigned long long orp_activation_time)		{ orp_activation_time_ = orp_activation_time; }
		inline void SetORPDeactivationTime(unsigned long long orp_deactivation_time)	{ orp_deactivation_time_ = orp_deactivation_time; }
		inline void SetAPActivationTime(unsigned long long ap_activation_time)			{ ap_activation_time_ = ap_activation_time; }
		inline void SetAPDeactivationTime(unsigned long long ap_deactivation_time)		{ ap_deactivation_time_ = ap_deactivation_time; }
		inline void SetLastCombatTime(unsigned long long last_combat_time)				{ last_combat_time_ = last_combat_time; }
		inline void SetBlackListFlag(bool flag)											{ is_blacklisted_ = flag; }
		inline void SetHarvestFlag(bool flag)											{ harvest_flag_ = flag; }
		inline void SetORPZones(const std::vector<ORPZone>& areas)						{ orp_zones_ = areas; }

		inline int GetKeyField() const													{ return GetTeamId(); }
		inline int GetTeamId() const													{ return team_id_; }
		inline unsigned long long GetORPActivationTime() const							{ return orp_activation_time_; }
		inline unsigned long long GetORPDeactivationTime() const						{ return orp_deactivation_time_; }
		inline unsigned long long GetAPActivationTime() const							{ return ap_activation_time_; }
		inline unsigned long long GetAPDeactivationTime() const							{ return ap_deactivation_time_; }
		inline unsigned long long GetLastCombatTime() const								{ return last_combat_time_; }
		inline bool GetBlackListFlag() const											{ return is_blacklisted_; }
		inline bool GetHarvestFlag() const												{ return harvest_flag_; }
		inline std::vector<ORPZone>& GetORPZones()										{ return orp_zones_; }
	};

	class ORPTimingRules
	{
	private:
		int max_duration_sec_;
		int no_battle_time_sec_;
		int normal_delay_sec_;
		int long_delay_sec_;
		int players_notifications_interval_sec_;
		int dinos_notifications_interval_sec_;
		int structures_notifications_interval_sec_;

	public:
		ORPTimingRules()
			: max_duration_sec_(0),
			  no_battle_time_sec_(0),
			  normal_delay_sec_(0),
			  long_delay_sec_(0),
			  players_notifications_interval_sec_(0),
			  dinos_notifications_interval_sec_(0),
			  structures_notifications_interval_sec_(0)
		{
		}

		~ORPTimingRules() = default;

		inline int GetMaxDurationSec() const						{ return max_duration_sec_; }
		inline int GetNoBattleTimeSec() const						{ return no_battle_time_sec_; }
		inline int GetNormalDelaySec() const						{ return normal_delay_sec_; }
		inline int GetLongDelaySec() const							{ return long_delay_sec_; }
		inline int GetPlayersNotificationsIntervalSec() const		{ return players_notifications_interval_sec_; }
		inline int GetDinosNotificationsIntervalSec() const			{ return dinos_notifications_interval_sec_; }
		inline int GetStructuresNotificationsIntervalSec() const	{ return structures_notifications_interval_sec_; }

		inline void SetMaxDurationSec(int secs)						{ max_duration_sec_ = secs; }
		inline void SetNoBattleTimeSec(int secs)					{ no_battle_time_sec_ = secs; }
		inline void SetNormalDelaySec(int secs)						{ normal_delay_sec_ = secs; }
		inline void SetLongDelaySec(int secs)						{ long_delay_sec_ = secs; }
		inline void SetPlayersNotificationsIntervalSec(int secs)	{ players_notifications_interval_sec_ = secs; }
		inline void SetDinosNotificationsIntervalSec(int secs)		{ dinos_notifications_interval_sec_ = secs; }
		inline void SetStructuresNotificationsIntervalSec(int secs)	{ structures_notifications_interval_sec_ = secs; }
	};

	class APTimingRules
	{
	private:
		int ap_default_duration_;
		int players_notifications_interval_sec_;
		int dinos_notifications_interval_sec_;
		int structures_notifications_interval_sec_;

	public:
		APTimingRules()
			: ap_default_duration_(0),
			  players_notifications_interval_sec_(0),
			  dinos_notifications_interval_sec_(0),
			  structures_notifications_interval_sec_(0)
		{
		}

		APTimingRules(int ap_default_duration, int players_notifications_interval, int dinos_notifications_interval, int structures_notifications_interval)
			: ap_default_duration_(ap_default_duration),
			  players_notifications_interval_sec_(players_notifications_interval),
			  dinos_notifications_interval_sec_(dinos_notifications_interval),
			  structures_notifications_interval_sec_(structures_notifications_interval)
		{
		}

		~APTimingRules() = default;

		inline int GetAPDefaultDurationSec() const						{ return ap_default_duration_; }
		inline int GetPlayersNotificationsIntervalSec() const			{ return players_notifications_interval_sec_; }
		inline int GetDinosNotificationsIntervalSec() const				{ return dinos_notifications_interval_sec_; }
		inline int GetStructuresNotificationsIntervalSec() const		{ return structures_notifications_interval_sec_; }

		inline void SetAPDefaultDurationSec(int ap_default_duration)	{ ap_default_duration_ = ap_default_duration; }
		inline void SetPlayersNotificationsIntervalSec(int secs)		{ players_notifications_interval_sec_ = secs; }
		inline void SetDinosNotificationsIntervalSec(int secs)			{ dinos_notifications_interval_sec_ = secs; }
		inline void SetStructuresNotificationsIntervalSec(int secs)		{ structures_notifications_interval_sec_ = secs; }
	};

	class ProtectionCommonRules
	{
		using OverridesORPData = std::vector<ORPPercentOverrideData>;

	private:
		bool is_players_orp_protected_				= false;
		bool is_dinos_orp_protected_				= false;
		bool is_structures_orp_protected_			= false;
		float orp_players_protection_percent_		= 0.0f;
		float orp_dinos_protection_percent_			= 0.0f;
		float orp_structures_protection_percent_	= 0.0f;

		bool is_players_ap_protected_				= false;
		bool is_dinos_ap_protected_					= false;
		bool is_structures_ap_protected_			= false;
		float ap_players_protection_percent_		= false;
		float ap_dinos_protection_percent_			= false;
		float ap_structures_protection_percent_		= false;

		bool is_range_based_orp_enabled_			= false;
		bool is_default_orp_percent_overriden		= false;
		unsigned int range_based_orp_max_location_	= 0;
		float range_based_orp_distance_				= 0.0f;

		std::vector<ORPPercentOverrideData> orp_overrides_;

	public:
		ProtectionCommonRules() = default;
		~ProtectionCommonRules() = default;

		inline bool GetPlayersORPFlag() const						{ return is_players_orp_protected_; }
		inline bool GetDinosORPFlag() const							{ return is_dinos_orp_protected_; }
		inline bool GetStructuresORPFlag() const					{ return is_structures_orp_protected_; }
		inline bool GetPlayersAPFlag() const						{ return is_players_ap_protected_; }
		inline bool GetDinosAPFlag() const							{ return is_dinos_ap_protected_; }
		inline bool GetStructuresAPFlag() const						{ return is_structures_ap_protected_; }

		inline float GetPlayersORPPercent() const					{ return orp_players_protection_percent_; }
		inline float GetDinosORPPercent() const						{ return orp_dinos_protection_percent_; }
		inline float GetStructuresORPPercent() const				{ return orp_structures_protection_percent_; }
		inline float GetPlayersAPPercent() const					{ return ap_players_protection_percent_; }
		inline float GetDinosAPPercent() const						{ return ap_dinos_protection_percent_; }
		inline float GetStructuresAPPercent() const					{ return ap_structures_protection_percent_; }

		inline bool GetRangeBasedORPFlag() const					{ return is_range_based_orp_enabled_; }
		inline bool GetDefaultORPPercentOverridenFlag() const		{ return is_default_orp_percent_overriden; }
		inline unsigned int GetRangeBasedORPMaxLocations() const	{ return range_based_orp_max_location_; }
		inline float GetRangeBasedORPDistance() const				{ return range_based_orp_distance_; }
		OverridesORPData& GetORPOverrides()							{ return orp_overrides_; }



		inline void SetPlayersORPFlag(bool val)						{ is_players_orp_protected_ = val; }
		inline void SetDinosORPFlag(bool val)						{ is_dinos_orp_protected_ = val; }
		inline void SetStructuresORPFlag(bool val)					{ is_structures_orp_protected_ = val; }
		inline void SetPlayersAPFlag(bool val)						{ is_players_ap_protected_ = val; }
		inline void SetDinosAPFlag(bool val)						{ is_dinos_ap_protected_ = val; }
		inline void SetStructuresAPFlag(bool val)					{ is_structures_ap_protected_ = val; }

		inline void SetPlayersORPPercent(float val)					{ orp_players_protection_percent_ = val; }
		inline void SetDinosORPPercent(float val)					{ orp_dinos_protection_percent_ = val; }
		inline void SetStructuresORPPercent(float val)				{ orp_structures_protection_percent_ = val; }
		inline void SetPlayersAPPercent(float val)					{ ap_players_protection_percent_ = val; }
		inline void SetDinosAPPercent(float val)					{ ap_dinos_protection_percent_ = val; }
		inline void SetStructuresAPPercent(float val)				{ ap_structures_protection_percent_ = val; }

		inline void SetRangeBasedORPFlag(bool val)					{ is_range_based_orp_enabled_ = val; }
		inline void SetDefaultORPPercentOverridenFlag(bool val)		{ is_default_orp_percent_overriden = val; }
		inline void SetRangeBasedORPMaxLocations(unsigned int val)	{ range_based_orp_max_location_ = val; }
		inline void SetRangeBasedORPDistance(float val)				{ range_based_orp_distance_ = val; };
	};
}

#endif // !_GOG_PROT_PRIMAL_TYPES_H_