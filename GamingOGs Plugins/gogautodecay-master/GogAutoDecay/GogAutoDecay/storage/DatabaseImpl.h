#pragma once

#include <string>
#include <vector>

#pragma warning(push, 0)
	#include <mysql++11/mysql++11.h>
#pragma warning(pop)

#include <json.hpp>
#include <Logger/Logger.h>

#pragma warning(push)
#pragma warning(disable : 26444)

#include "PlayerProps.h"
#include "TeamProps.h"

namespace AutoDecay
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

			if (!conn_.open(options))
			{
				return false;
			}

			return true;
		}

		bool CheckStructure()
		{
			try
			{
				conn_.query("CREATE TABLE IF NOT EXISTS `players` ("
					"`SteamId` BIGINT(11) NOT NULL, "
					"`TeamId` BIGINT(11) NOT NULL DEFAULT 0, "
					"`PointsOrdered` BIGINT(11) DEFAULT 0, "
					"`BoxesOrdered` BIGINT(11) DEFAULT 0, "
					"UNIQUE INDEX SteamId_UNIQUE (`SteamId` ASC));");

				conn_.query("CREATE TABLE IF NOT EXISTS `teams` ("
					"`TeamId` BIGINT(11) NOT NULL DEFAULT 0, "
					"`LastSeenTime` BIGINT(11) NOT NULL DEFAULT 0, "
					"UNIQUE INDEX SteamId_UNIQUE (`TeamId` ASC));");
			}
			catch (const std::exception & e)
			{
				Log::GetLog()->error("[{}:{}] Database error {}", __FILE__, __FUNCTION__, e.what());
				return false;
			}

			return true;
		}

		void LoadPlayers(std::vector<Player>& out)
		{
			auto select_cbk = [&out](unsigned long long steam_id, int team_id, int pts_ordered, int boxes_ordered)
			{
				Player player_data = std::make_shared<PlayerProps>();
				player_data->SetSteamId(steam_id);
				player_data->SetTeamId(team_id);
				player_data->SetPointsOrdered(pts_ordered);
				player_data->SetBoxesOrdered(boxes_ordered);
				out.push_back(player_data);
				return true;
			};

			try
			{
				conn_.query("SELECT SteamId, TeamId, PointsOrdered, BoxesOrdered FROM players;").each(select_cbk);
			}
			catch (const std::exception & e)
			{
				Log::GetLog()->error("[{}:{}] Database error {}", __FILE__, __FUNCTION__, e.what());
			}
		}

		void LoadTeams(std::vector<Team>& out)
		{
			auto select_cbk = [&out](int team_id, __int64 last_seen_time)
			{
				Team team_data = std::make_shared<TeamProps>();
				team_data->SetTeamId(team_id);
				team_data->SetLastSeenTime(last_seen_time);
				out.push_back(team_data);

				return true;
			};

			try
			{
				conn_.query("SELECT TeamId, LastSeenTime FROM teams;").each(select_cbk);
			}
			catch (const std::exception & e)
			{
				Log::GetLog()->error("[{}:{}] Database error {}", __FILE__, __FUNCTION__, e.what());
			}
		}

		void SavePlayers(const std::vector<Player>& data)
		{
			try
			{
				for (const auto player : data)
				{
					const std::string qstr = fmt::format("INSERT INTO players (SteamId, TeamId, PointsOrdered, BoxesOrdered) VALUES ({}, {}, {}, {}) "
						"ON DUPLICATE KEY UPDATE "
						"SteamId=VALUES(SteamId), TeamId=VALUES(TeamId), PointsOrdered=VALUES(PointsOrdered), BoxesOrdered=VALUES(BoxesOrdered);",
						player->GetSteamId(), player->GetTeamId(), player->GetPointsOrdered(), player->GetBoxesOrdered());
					conn_.query(qstr);
				}
			}
			catch (const std::exception & e)
			{
				Log::GetLog()->error("[{}:{}] Database error {}", __FILE__, __FUNCTION__, e.what());
			}
		}

		void SaveTeams(const std::vector<Team>& data)
		{
			try
			{
				for (const auto team : data)
				{
					const std::string qstr = fmt::format("INSERT INTO teams (TeamId, LastSeenTime) "
						"VALUES({}, {}) "
						"ON DUPLICATE KEY UPDATE TeamId=VALUES(TeamId), LastSeenTime=VALUES(LastSeenTime);",
						team->GetTeamId(), team->GetLastSeenTime());
					conn_.query(qstr);
				}
			}
			catch (const std::exception & e)
			{
				Log::GetLog()->error("[{}:{}] Database error {}", __FILE__, __FUNCTION__, e.what());
			}
		}

	private:
		/*std::string DumpPointsPenalties(const std::vector<TeamProps::PointsPenalty>& in)
		{
			std::string out{};
			nlohmann::basic_json<> json{};

			try
			{
				for (const auto& penalty : in)
				{
					nlohmann::json penalty_data
					{
						{ "day", penalty.day },
						{ "spent", penalty.spent },
						{ "remaining", penalty.remaining }
					};
					json.push_back(penalty_data);
				}
				out = json.dump();
			}
			catch (const std::exception & e)
			{
				Log::GetLog()->error("[{}:{}] Penalties dumping error {}", __FILE__, __FUNCTION__, e.what());
			}

			return std::move(out);
		}

		std::string DumpBoxPenalties(const std::vector<TeamProps::BoxPenalty>& in)
		{
			std::string out{};
			nlohmann::basic_json<> json{};

			try
			{
				for (const auto& penalty : in)
				{
					nlohmann::json penalty_data
					{
						{ "day", penalty.day },
						{ "spent", penalty.spent },
						{ "remaining", penalty.remaining }
					};
					json.push_back(penalty_data);
				}
				out = json.dump();
			}
			catch (const std::exception & e)
			{
				Log::GetLog()->error("[{}:{}] Penalties dumping error {}", __FILE__, __FUNCTION__, e.what());
			}

			return std::move(out);
		}

		std::vector<TeamProps::PointsPenalty> ParsePointsPenalties(std::string dump)
		{
			std::vector<TeamProps::PointsPenalty> out{};
			nlohmann::basic_json<> json{};

			try
			{
				json = nlohmann::json::parse(std::move(dump));

				for (const auto& item : json)
				{
					out.push_back(TeamProps::PointsPenalty{ item["day"], item["spent"], item["remaining"] });
				}
			}
			catch (const std::exception & e)
			{
				Log::GetLog()->error("[{}:{}] Penalties parsing error {}", __FILE__, __FUNCTION__, e.what());
			}

			return std::move(out);
		}

		std::vector<TeamProps::BoxPenalty> ParseBoxPenalties(std::string dump)
		{
			std::vector<TeamProps::BoxPenalty> out{};
			nlohmann::basic_json<> json{};

			try
			{
				json = nlohmann::json::parse(std::move(dump));

				for (const auto& item : json)
				{
					out.push_back(TeamProps::BoxPenalty{ item["day"], item["spent"], item["remaining"] });
				}
			}
			catch (const std::exception & e)
			{
				Log::GetLog()->error("[{}:{}] Penalties parsing error {}", __FILE__, __FUNCTION__, e.what());
			}

			return std::move(out);
		}*/

		daotk::mysql::connection conn_;
	};
}