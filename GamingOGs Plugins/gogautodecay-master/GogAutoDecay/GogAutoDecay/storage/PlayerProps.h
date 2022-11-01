#pragma once

#include <memory>

#include "../utils/Types.h"

namespace AutoDecay
{
	class PlayerProps
	{
	public:
		PlayerProps()
			: steam_id_(0ULL), team_id_(0), points_ordered_(0), boxes_ordered_(0), is_online_(false)
		{
		}

		unsigned long long GetKeyField() const
		{
			return steam_id_;
		}

		unsigned long long GetSteamId() const
		{
			return steam_id_;
		}

		int GetTeamId() const
		{
			return team_id_;
		}

		int GetPointsOrdered() const
		{
			return points_ordered_;
		}

		int GetBoxesOrdered() const
		{
			return boxes_ordered_;
		}

		bool GetOnlineFlag() const
		{
			return is_online_;
		}

		void SetSteamId(unsigned long long val)
		{
			steam_id_ = val;
		}

		void SetTeamId(int val)
		{
			team_id_ = val;
		}

		void SetPointsOrdered(int val)
		{
			points_ordered_ = val;
		}

		void SetBoxesOrdered(int val)
		{
			boxes_ordered_ = val;
		}

		void SetOnlineFlag(bool val)
		{
			is_online_ = val;
		}

	private:
		unsigned long long steam_id_;
		int team_id_;
		int points_ordered_;
		int boxes_ordered_;
		bool is_online_;
	};

	using Player = RefPtr<PlayerProps>;
}