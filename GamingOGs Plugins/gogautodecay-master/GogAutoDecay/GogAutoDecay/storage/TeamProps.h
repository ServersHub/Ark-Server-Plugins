#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../utils/Types.h"

namespace AutoDecay
{
	class TeamProps
	{
	public:
		TeamProps()
			: team_id_(0), last_seen_time_(0L)
		{
		}

		int GetKeyField() const
		{
			return team_id_;
		}

		int GetTeamId() const
		{
			return team_id_;
		}

		__int64 GetLastSeenTime() const
		{
			return last_seen_time_;
		}

		void SetTeamId(int val)
		{
			team_id_ = val;
		}

		void SetLastSeenTime(__int64 val)
		{
			last_seen_time_ = val;
		}

	private:
		int team_id_;
		__int64 last_seen_time_;
	};

	using Team = RefPtr<TeamProps>;
}