#ifndef _GOG_PROT_BLACK_LIST_H_
#define _GOG_PROT_BLACK_LIST_H_

#include "../Storage/Storage.h"

namespace Protection
{
	class BlackList
	{
	public:
		BlackList(Storage* storage);
		~BlackList();

		bool IsTeamBlackListed(TeamItemType& team);
		void ForceAddTeam(TeamItemType& team);
		bool ForceRemoveTeam(TeamItemType& team);

	private:
		bool IsTeamHasBlackListedPlayers(const TeamItemType& team);

		Storage* storage_;
	};
}

#endif // !_GOG_PROT_BLACK_LIST_H_
