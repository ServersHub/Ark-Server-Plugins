#ifndef _GOG_PROT_ONLINE_MONITOR_H_
#define _GOG_PROT_ONLINE_MONITOR_H_

#include "../Storage/Storage.h"

namespace Protection
{
	class OnlineMonitor
	{
	public:
		OnlineMonitor(Storage* storage);
		~OnlineMonitor();

		bool IsPlayerOnline(const PlayerItemType& player);
		bool IsTeamOnline(const TeamItemType& team);

		void PlayerOnline(const PlayerItemType& player);
		void PlayerOffline(const PlayerItemType& player);

	private:
		Storage* storage_;
	};
}

#endif // !_GOG_PROT_ONLINE_MONITOR_H_
