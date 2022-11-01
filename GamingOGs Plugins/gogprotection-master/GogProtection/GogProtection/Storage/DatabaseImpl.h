#ifndef _GOG_PROT_DATABASE_IMPL_H_
#define _GOG_PROT_DATABASE_IMPL_H_

#include <string>
#include <memory>

#pragma warning(push, 0)
	#include <mysql++11.h>
#pragma warning(pop)

#include <json.hpp>
#include <Logger/Logger.h>

#pragma warning(push)
#pragma warning(disable : 26444)

namespace Protection
{
	namespace DB
	{
		class DatabaseImpl
		{
		public:
			DatabaseImpl()
			{
			}

			~DatabaseImpl()
			{
			}

			bool Connect(const std::string& host, const std::string& user, const std::string& pass, const std::string& db)
			{
				daotk::mysql::connect_options options;
				options.server = host;
				options.username = user;
				options.password = pass;
				options.dbname = db;
				options.timeout = 30;
				options.autoreconnect = true;

				if (!conn_.open(options)) {
					return false;
				}

				return true;
			}

			bool CheckStructure()
			{
				try {
					conn_.query("CREATE TABLE IF NOT EXISTS `ProtPlayers` ("
						"`SteamId` BIGINT(11) NOT NULL, "
						"`TeamId` BIGINT(11) NOT NULL DEFAULT 0, "
						"`DefaultBlackListed` TINYINT(1) DEFAULT 0, "
						"UNIQUE INDEX SteamId_UNIQUE (`SteamId` ASC));");

					conn_.query("CREATE TABLE IF NOT EXISTS `ProtTeams` ("
						"`TeamId` BIGINT(11) NOT NULL DEFAULT 0, "
						"`ORPActivationTime` BIGINT(11) NOT NULL DEFAULT 0, "
						"`ORPDeactivationTime` BIGINT(11) NOT NULL DEFAULT 0, "
						"`APActivationTime` BIGINT(11) NOT NULL DEFAULT 0, "
						"`APDeactivationTime` BIGINT(11) NOT NULL DEFAULT 0, "
						"`LastCombatTime` BIGINT(11) NOT NULL DEFAULT 0, "
						"`ORPZones` VARCHAR(512) NOT NULL DEFAULT '[]', "
						"`BlackListed` TINYINT(1) DEFAULT 0, "
						"UNIQUE INDEX SteamId_UNIQUE (`TeamId` ASC));");
				}
				catch (const std::exception & e) {
					Log::GetLog()->error("[{}:{}] Database error {}", __FILE__, __FUNCTION__, e.what());
					return false;
				}

				return true;
			}

			void LoadPlayers(std::vector<std::shared_ptr<PlayerData>>& out)
			{
				auto select_cbk = [&out](unsigned long long steam_id, int team_id, int default_blacklisted)
				{
					std::shared_ptr<PlayerData> player_data = std::make_shared<PlayerData>(steam_id, team_id, default_blacklisted, false);
					out.push_back(player_data);

					return true;
				};

				try {
					conn_.query("SELECT SteamId, TeamId, DefaultBlackListed FROM ProtPlayers;").each(select_cbk);
				}
				catch (const std::exception & e) {
					Log::GetLog()->error("[{}:{}] Database error {}", __FILE__, __FUNCTION__, e.what());
				}
			}

			void LoadTeams(std::vector<std::shared_ptr<TeamData>>& out)
			{
				auto orp_parse_cbk = &DatabaseImpl::ParseORPZones;

				auto select_cbk = [&out, orp_parse_cbk, this](int team_id, unsigned long long orp_activation_time, unsigned long long orp_deactivation_time,
					unsigned long long ap_activation_time, unsigned long long ap_deactivation_time, unsigned long long last_combat_time, const std::string orp_zones, int blacklisted)
				{
					std::shared_ptr<TeamData> team = std::make_shared<TeamData>(team_id, orp_activation_time, orp_deactivation_time,
						ap_activation_time, ap_deactivation_time, last_combat_time, ((*this).*orp_parse_cbk)(orp_zones), blacklisted, false);
					out.push_back(team);

					return true;
				};

				try {
					conn_.query("SELECT TeamId, ORPActivationTime, ORPDeactivationTime, APActivationTime, APDeactivationTime, LastCombatTime, ORPZones, BlackListed FROM ProtTeams;").each(select_cbk);
				}
				catch (const std::exception & e) {
					Log::GetLog()->error("[{}:{}] Database error {}", __FILE__, __FUNCTION__, e.what());
				}
			}

			void SavePlayers(const std::vector<std::shared_ptr<PlayerData>>& data)
			{
				try {
					for (const auto player : data) {
						const std::string qstr = fmt::format("INSERT INTO ProtPlayers (SteamId, TeamId, DefaultBlackListed) VALUES ({}, {}, {}) "
							"ON DUPLICATE KEY UPDATE "
							"SteamId=VALUES(SteamId), TeamId=VALUES(TeamId), DefaultBlackListed=VALUES(DefaultBlackListed);",
							player->GetSteamId(), player->GetTeamId(), (int)player->GetBlackListFlag());
						conn_.query(qstr);
					}
				}
				catch (const std::exception & e) {
					Log::GetLog()->error("[{}:{}] Database error {}", __FILE__, __FUNCTION__, e.what());
				}
			}

			void SaveTeams(const std::vector<std::shared_ptr<TeamData>>& data)
			{
				try {
					for (const auto team : data) {
						const std::string qstr = fmt::format("INSERT INTO ProtTeams (TeamId, ORPActivationTime, ORPDeactivationTime, APActivationTime, "
							" APDeactivationTime, LastCombatTime, ORPZones, BlackListed) "
							"VALUES({}, {}, {}, {}, {}, {}, '{}', {}) "
							"ON DUPLICATE KEY UPDATE "
							"TeamId=VALUES(TeamId), ORPActivationTime=VALUES(ORPActivationTime), ORPDeactivationTime=VALUES(ORPDeactivationTime), APActivationTime=VALUES(APActivationTime), "
							"APDeactivationTime=VALUES(APDeactivationTime), LastCombatTime=VALUES(LastCombatTime), ORPZones=VALUES(ORPZones), BlackListed=VALUES(BlackListed);",
							team->GetTeamId(), team->GetORPActivationTime(), team->GetORPDeactivationTime(), team->GetAPActivationTime(), team->GetAPDeactivationTime(),
							team->GetLastCombatTime(), DumpORPZones(team->GetORPZones()), (int)team->GetBlackListFlag());
						conn_.query(qstr);
					}
				}
				catch (const std::exception & e) {
					Log::GetLog()->error("[{}:{}] Database error {}", __FILE__, __FUNCTION__, e.what());
				}
			}

			void CleanPlayers()
			{
				try {
					conn_.query("DELETE FROM ProtPlayers;");
				}
				catch (const std::exception& e) {
					Log::GetLog()->error("[{}:{}] Database error {}", __FILE__, __FUNCTION__, e.what());
				}
			}

			void CleanTeams()
			{
				try {
					conn_.query("DELETE FROM ProtTeams;");
				}
				catch (const std::exception & e) {
					Log::GetLog()->error("[{}:{}] Database error {}", __FILE__, __FUNCTION__, e.what());
				}
			}

		private:
			std::string DumpORPZones(const std::vector<ORPZone>& areas)
			{
				std::string out{};
				nlohmann::json json{};

				try {
					for (const auto& area : areas) {
						nlohmann::json zone_data
						{
							{"id", area.id},
							{"x", area.x},
							{"y", area.y},
							{"z", area.z}
						};
						json.push_back(zone_data);
					}
					out = json.dump();
				}
				catch (const std::exception & e) {
					Log::GetLog()->error("[{}:{}] ORP zones dumping error {}", __FILE__, __FUNCTION__, e.what());
				}

				return out;
			}

			std::vector<ORPZone> ParseORPZones(const std::string& dump)
			{
				std::vector<ORPZone> out{};
				nlohmann::basic_json<> json{};

				try {
					json = nlohmann::json::parse(dump);

					for (const auto& item : json)
						out.push_back(ORPZone{ item["id"], item["x"], item["y"], item["z"] });
				}
				catch (const std::exception & e) {
					Log::GetLog()->error("[{}:{}] ORP zones parsing error {}", __FILE__, __FUNCTION__, e.what());
				}

				return out;
			}

			daotk::mysql::connection conn_;
		};
	}
}

#pragma warning(pop)

#endif // !_GOG_PROT_DATABASE_IMPL_H_
