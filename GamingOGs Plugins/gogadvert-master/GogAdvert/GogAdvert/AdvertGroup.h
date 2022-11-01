#pragma once

#include <json.hpp>
#include <vector>
#include <memory>
#include <chrono>

#include "Advert.h"

namespace GogAdvert
{
	class AdvertGroup
	{
	public:
		AdvertGroup(const nlohmann::basic_json<>& j, const FString& sender_name);
		void Update();

	private:
		bool IsReady() const;
		void OnSent();

		int interval_;
		std::chrono::system_clock::time_point next_time_;
		std::vector<std::unique_ptr<Advert>> adverts_;
	};
}