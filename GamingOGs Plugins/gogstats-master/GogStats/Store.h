#ifndef _GOG_STATS_STORE_H_
#define _GOG_STATS_STORE_H_

#include <string>
#include <ctime>
#include "Config.h"

namespace daotk::mysql
{
	class connection;
}

namespace Stats
{
	struct PlayerData
	{
		unsigned long long steamId;
		bool needUpdate;
		std::string rankTag;
	};

	class Store
	{
	public:
		Store();
		~Store();

		static Store& Get()
		{
			static Store instance_;
			return instance_;
		}

		void Initialize(const DBConfig& config);

		bool IsPlayerExists(unsigned long long steamId, const std::string& serverId);
		bool IsTribeExists(int tribeId, const std::string& serverId);

		void LoadPlayersData(const std::string& serverId, std::vector<PlayerData>& out);
		void CreatePlayer(unsigned long long steamId, const std::string& serverId);
		void SetTribeId(unsigned long long steamId, int tribeId, const std::string& serverId);
		void SetTribeRankTag(unsigned long long steamId, const std::string& tag, const std::string& serverId);
		void PlayerOnline(unsigned long long steamId, const std::string& serverId);
		void PlayerOffline(unsigned long long steamId, const std::string& serverId);
		void UpdatePlayer(unsigned long long steamId, unsigned long long playerId, const std::string& playerName, const std::string& serverId);
		void PlayerUpdated(unsigned long long steamId, const std::string& serverId);
		void Death(unsigned long long steamId, const std::string& serverId);
		void KillPlayer(unsigned long long steamId, const std::string& serverId);
		void KillTamedDino(unsigned long long steamId, const std::string& serverId);
		void KillWildDino(unsigned long long steamId, const std::string& serverId);
		void KillRareDino(unsigned long long steamId, const std::string& fieldName, const std::string& serverId);
		
		void CreateTribe(int tribeId, const std::string& tribeName, const std::string& serverId);
		void RenameTribe(int tribeId, const std::string& newName, const std::string& serverId);
		void MaybeRemoveTribe(int tribeId, const std::string& serverId);
		void TribeKillPlayer(int tribeId, const std::string& serverId);
		void TribeKillTamedDino(int tribeId, const std::string& serverId);
		void TribeKillWildDino(int tribeId, const std::string& serverId);
		void TribePlayerDeath(int tribeId, const std::string& serverId);
		void TribeDinoDeath(int tribeId, const std::string& serverId);

	private:
		daotk::mysql::connection* m_pDB;
	};
}

#endif // !_GOG_STATS_STORE_H_
