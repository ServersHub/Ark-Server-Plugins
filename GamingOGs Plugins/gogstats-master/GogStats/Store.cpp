#include "Store.h"

#pragma warning(push, 0)
    #include <mysql++11/mysql++11.h>
#pragma warning(pop)

#include <Logger/Logger.h>
#include <thread>

namespace Stats
{
    Store::Store()
    {
        m_pDB = new daotk::mysql::connection{};
    }

    Store::~Store()
    {
        if (m_pDB)
        {
            delete m_pDB;
        }
    }

    void Store::Initialize(const DBConfig& config)
    {
        daotk::mysql::connect_options options;
        options.server = config.host;
        options.username = config.user;
        options.password = config.password;
        options.dbname = config.database;
        options.autoreconnect = true;
        options.timeout = 30;

        bool result = m_pDB->open(options);
        if (!result)
        {
            throw std::runtime_error("Can't open MySQL connection");
        }

        std::string customFieldsPart{};
        for (const auto& field : config.customFields)
        {
            customFieldsPart.append(fmt::format("`{}` int NOT NULL DEFAULT 0,", field));
        }

        std::string query{
            "CREATE TABLE IF NOT EXISTS `personal_stats` ("
            "`pk`                int NOT NULL AUTO_INCREMENT,"
            "`steam_id`          bigint NOT NULL,"
            "`player_id`         bigint NOT NULL DEFAULT 0,"
            "`player_name`       varchar(128) NOT NULL DEFAULT '',"
            "`tribe_id`          int NULL DEFAULT NULL,"
            "`tribe_rank`        varchar(128) NOT NULL DEFAULT '',"
            "`last_login`        bigint NOT NULL DEFAULT 0,"
            "`play_time`         bigint NOT NULL DEFAULT 0,"
            "`online_status`     int NOT NULL DEFAULT 0,"
            "`deaths`            int NOT NULL DEFAULT 0,"
            "`players_kills`     int NOT NULL DEFAULT 0,"
            "`wild_dinos_kills`  int NOT NULL DEFAULT 0,"
            "`tamed_dinos_kills` int NOT NULL DEFAULT 0,"
        };

        if (!customFieldsPart.empty())
        {
            query.append(customFieldsPart);
        }

        query.append(
            "`need_handling`     int NOT NULL DEFAULT 1,"
            "`server_id`         varchar(256) NOT NULL DEFAULT '',"
            "PRIMARY KEY(`pk`));"
        );

        result = m_pDB->query(query);

        if (!result)
        {
            throw std::runtime_error(fmt::format("({} {}) Failed to create table!", __FILE__, __FUNCTION__));
        }

        result = result && m_pDB->query(
            "CREATE TABLE IF NOT EXISTS `tribe_stats` ("
            "`pk`                 int NOT NULL AUTO_INCREMENT,"
            "`tribe_id`           bigint NOT NULL,"
            "`tribe_name`         varchar(128) NOT NULL,"
            "`players_kills`      int NOT NULL DEFAULT 0,"
            "`tamed_dinos_kills`  int NOT NULL DEFAULT 0,"
            "`wild_dinos_kills`   int NOT NULL DEFAULT 0,"
            "`players_deaths`     int NOT NULL DEFAULT 0,"
            "`tamed_dinos_deaths` int NOT NULL DEFAULT 0,"
            "`server_id`          varchar(256) NOT NULL DEFAULT '',"
            "PRIMARY KEY(`pk`));"
        );

        if (!result)
        {
            throw std::runtime_error(fmt::format("({} {}) Failed to create table!", __FILE__, __FUNCTION__));
        }
    }

    bool Store::IsPlayerExists(unsigned long long steamId, const std::string& serverId)
    {
        try
        {
            return m_pDB->query(fmt::format("SELECT count(1) FROM personal_stats WHERE steam_id = {} AND server_id = '{}';", steamId, serverId)).get_value<int>() > 0;
        }
        catch (const std::exception& exception)
        {
            Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
            return false;
        }
    }

    bool Store::IsTribeExists(int tribeId, const std::string& serverId)
    {
        try
        {
            return m_pDB->query(fmt::format("SELECT count(1) FROM tribe_stats WHERE tribe_id = {} AND server_id = '{}';", tribeId, serverId)).get_value<int>() > 0;
        }
        catch (const std::exception& exception)
        {
            Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
            return false;
        }
    }

    void Store::LoadPlayersData(const std::string& serverId, std::vector<PlayerData>& out)
    {
        try
        {
            m_pDB->query(fmt::format("SELECT (steam_id, need_handling, tribe_rank) FROM personal_stats WHERE server_id = '{}';", serverId))
                .each([&](unsigned long long steamId, int needHandling, std::string tribeRank) {
                    out.push_back(PlayerData{ steamId, needHandling == 1 ? true : false, tribeRank });
                    return true;
                });

        }
        catch (const std::exception& exception)
        {
            Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
        }
    }

    void Store::CreatePlayer(unsigned long long steamId, const std::string& serverId)
    {
        try
        {
            m_pDB->query(fmt::format("INSERT INTO personal_stats (steam_id, server_id) VALUES ({}, '{}');", steamId, serverId));
        }
        catch (const std::exception& exception)
        {
            Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
        }
    }

    void Store::SetTribeId(unsigned long long steamId, int tribeId, const std::string& serverId)
    {
        try
        {
            if (tribeId == 0)
            {
                m_pDB->query(fmt::format("UPDATE personal_stats SET tribe_id = NULL WHERE steam_id = {} AND server_id = '{}';", steamId, serverId));
            }
            else
            {
                auto res = m_pDB->query(fmt::format("SELECT pk FROM tribe_stats WHERE tribe_id = {} AND server_id = '{}';", tribeId, serverId));
                if (res)
                {
                    const int pk = res.get_value<int>();
                    m_pDB->query(fmt::format("UPDATE personal_stats SET tribe_id = {} WHERE steam_id = {} AND server_id = '{}';", pk, steamId, serverId));
                }
            }
        }
        catch (const std::exception& exception)
        {
            Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
        }
    }

    void Store::SetTribeRankTag(unsigned long long steamId, const std::string& tag, const std::string& serverId)
    {
        try
        {
            m_pDB->query(fmt::format("UPDATE personal_stats SET tribe_rank = '{}' WHERE steam_id = {} AND server_id = '{}';", tag, steamId, serverId));
        }
        catch (const std::exception& exception)
        {
            Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
        }
    }

    void Store::PlayerOnline(unsigned long long steamId, const std::string& serverId)
    {
        try
        {
            m_pDB->query(fmt::format("UPDATE personal_stats SET online_status = 1, last_login = {} WHERE steam_id = {} AND server_id = '{}';", std::time(nullptr), steamId, serverId));
        }
        catch (const std::exception& exception)
        {
            Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
        }
    }

    void Store::PlayerOffline(unsigned long long steamId, const std::string& serverId)
    {
        try
        {
            m_pDB->query(fmt::format("UPDATE personal_stats SET online_status = 0, play_time = play_time + ({} - last_login) WHERE steam_id = {} AND server_id = '{}';", 
                std::time(nullptr), steamId, serverId));
            m_pDB->query("UPDATE personal_stats SET last_login = {} WHERE steam_id = {} AND server_id = '{}';", std::time(nullptr), steamId, serverId);
        }
        catch (const std::exception& exception)
        {
            Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
        }
    }
    void Store::UpdatePlayer(unsigned long long steamId, unsigned long long playerId, const std::string& playerName, const std::string& serverId)
    {
        try
        {
            m_pDB->query(fmt::format("UPDATE personal_stats SET player_name = '{}', player_id = {} WHERE steam_id = {} AND server_id = '{}';",
                playerName, playerId, steamId, serverId));
        }
        catch (const std::exception& exception)
        {
            Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
        }
    }

    void Store::PlayerUpdated(unsigned long long steamId, const std::string& serverId)
    {
        try
        {
            m_pDB->query(fmt::format("UPDATE personal_stats SET need_handling = 0 WHERE steam_id = {} AND server_id = '{}';", steamId, serverId));
        }
        catch (const std::exception& exception)
        {
            Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
        }
    }

    void Store::Death(unsigned long long steamId, const std::string& serverId)
    {
        try
        {
            m_pDB->query(fmt::format("UPDATE personal_stats SET deaths = deaths + 1 WHERE steam_id = {} AND server_id = '{}';", steamId, serverId));
        }
        catch (const std::exception& exception)
        {
            Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
        }
    }

    void Store::KillPlayer(unsigned long long steamId, const std::string& serverId)
    {
        try
        {
            m_pDB->query(fmt::format("UPDATE personal_stats SET players_kills = players_kills + 1 WHERE steam_id = {} AND server_id = '{}';", steamId, serverId));
        }
        catch (const std::exception& exception)
        {
            Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
        }
    }

    void Store::KillTamedDino(unsigned long long steamId, const std::string& serverId)
    {
        try
        {
            m_pDB->query(fmt::format("UPDATE personal_stats SET tamed_dinos_kills = tamed_dinos_kills + 1 WHERE steam_id = {} AND server_id = '{}';", steamId, serverId));
        }
        catch (const std::exception& exception)
        {
            Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
        }
    }

    void Store::KillWildDino(unsigned long long steamId, const std::string& serverId)
    {
        try
        {
            m_pDB->query(fmt::format("UPDATE personal_stats SET wild_dinos_kills = wild_dinos_kills + 1 WHERE steam_id = {} AND server_id = '{}';", steamId, serverId));
        }
        catch (const std::exception& exception)
        {
            Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
        }
    }

    void Store::KillRareDino(unsigned long long steamId, const std::string& fieldName, const std::string& serverId)
    {
        try
        {
            m_pDB->query(fmt::format("UPDATE personal_stats SET {} = {} + 1 WHERE steam_id = {} AND server_id = '{}';", fieldName, fieldName, steamId, serverId));
        }
        catch (const std::exception& exception)
        {
            Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
        }
    }

    void Store::CreateTribe(int tribeId, const std::string& tribeName, const std::string& serverId)
    {
        try
        {
            m_pDB->query(fmt::format("INSERT INTO tribe_stats (tribe_id, tribe_name, server_id) VALUES ({}, '{}', '{}');", tribeId, tribeName, serverId));
        }
        catch (const std::exception& exception)
        {
            Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
        }
    }

    void Store::RenameTribe(int tribeId, const std::string& newName, const std::string& serverId)
    {
        try
        {
            m_pDB->query(fmt::format("UPDATE tribe_stats SET tribe_name = '{}' WHERE tribe_id = {} AND server_id = '{}';", newName, tribeId, serverId));
        }
        catch (const std::exception& exception)
        {
            Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
        }
    }

    void Store::MaybeRemoveTribe(int tribeId, const std::string& serverId)
    {
        try
        {
            const int tribePK = m_pDB->query(fmt::format("SELECT pk FROM tribe_stats WHERE tribe_id = {} AND server_id = '{}';", tribeId, serverId)).get_value<int>();
            if (tribePK > 0)
            {
                const int playersCount = m_pDB->query(fmt::format("SELECT count(1) FROM personal_stats WHERE tribe_id = {} AND server_id = '{}';", tribePK, serverId)).get_value<int>();
                if (playersCount <= 0)
                {
                    auto res = m_pDB->query(fmt::format("DELETE FROM tribe_stats WHERE pk = {} AND server_id = '{}';", tribePK, serverId));
                }
            }
            else
            {
                throw std::runtime_error("Tribe ID is not found in database: " + std::to_string(tribePK));
            }
        }
        catch (const std::exception& exception)
        {
            Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
        }
    }

    void Store::TribeKillPlayer(int tribeId, const std::string& serverId)
    {
        try
        {
            m_pDB->query(fmt::format("UPDATE tribe_stats SET players_kills = players_kills + 1 WHERE tribe_id = {} AND server_id = '{}';", tribeId, serverId));
        }
        catch (const std::exception& exception)
        {
            Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
        }
    }

    void Store::TribeKillTamedDino(int tribeId, const std::string& serverId)
    {
        try
        {
            m_pDB->query(fmt::format("UPDATE tribe_stats SET tamed_dinos_kills = tamed_dinos_kills + 1 WHERE tribe_id = {} AND server_id = '{}';", tribeId, serverId));
        }
        catch (const std::exception& exception)
        {
            Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
        }
    }

    void Store::TribeKillWildDino(int tribeId, const std::string& serverId)
    {
        try
        {
            m_pDB->query(fmt::format("UPDATE tribe_stats SET wild_dinos_kills = wild_dinos_kills + 1 WHERE tribe_id = {} AND server_id = '{}';", tribeId, serverId));
        }
        catch (const std::exception& exception)
        {
            Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
        }
    }

    void Store::TribePlayerDeath(int tribeId, const std::string& serverId)
    {
        try
        {
            m_pDB->query(fmt::format("UPDATE tribe_stats SET players_deaths = players_deaths + 1 WHERE tribe_id = {} AND server_id = '{}';", tribeId, serverId));
        }
        catch (const std::exception& exception)
        {
            Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
        }
    }

    void Store::TribeDinoDeath(int tribeId, const std::string& serverId)
    {
        try
        {
            m_pDB->query(fmt::format("UPDATE tribe_stats SET tamed_dinos_deaths = tamed_dinos_deaths + 1 WHERE tribe_id = {} AND server_id = '{}';", tribeId, serverId));
        }
        catch (const std::exception& exception)
        {
            Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
        }
    }
}