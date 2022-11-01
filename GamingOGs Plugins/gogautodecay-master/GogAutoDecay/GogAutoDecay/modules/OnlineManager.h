#pragma once

#include "../storage/PlayerProps.h"
#include "../storage/TeamProps.h"

namespace AutoDecay
{
	class Storage;

	class OnlineManager
	{
	public:
		OnlineManager(Storage* storage);

		bool IsPlayerOnline(Player& player);
		bool IsTeamOnline(Team& team);
		void PlayerOnline(Player& player);
		void PlayerOffline(Player& player);

	private:
		Storage* storage_;
	};
}