#include "AdvertGroup.h"

#include "Tools.h"

namespace GogAdvert
{
	AdvertGroup::AdvertGroup(const nlohmann::basic_json<>& j, const FString& sender_name)
	{
		interval_ = j["Interval"];

		for (const auto& message : j["Messages"])
		{
			auto advert = std::make_unique<Advert>(message, sender_name);
			adverts_.push_back(std::move(advert));
		}

		OnSent();
	}
	void AdvertGroup::Update()
	{
		if (adverts_.empty())
		{
			return;
		}

		if (!IsReady())
		{
			return;
		}

		int index = 0;

		if (adverts_.size() > 1)
		{
			index = GetRandomNumber(0, static_cast<int>(adverts_.size() - 1));
		}

		try
		{
			adverts_.at(static_cast<size_t>(index))->Send();
			OnSent();
		}
		catch (const std::exception&)
		{
			Log::GetLog()->error("Index out of range");
		}
	}

	bool AdvertGroup::IsReady() const
	{
		return next_time_ <= std::chrono::system_clock::now();
	}

	void AdvertGroup::OnSent()
	{
		next_time_ = std::chrono::system_clock::now() + std::chrono::minutes(interval_);
	}
}