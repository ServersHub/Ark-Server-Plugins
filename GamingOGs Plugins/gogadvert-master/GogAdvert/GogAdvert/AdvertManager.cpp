#include "AdvertManager.h"

#include <fstream>

#include "ApiProxy.h"
#include "Tools.h"
#include "AdvertGroup.h"

namespace GogAdvert
{
	AdvertManager::AdvertManager()
	{
	}

	AdvertManager::~AdvertManager()
	{
	}

	bool AdvertManager::Initialize()
	{
		if (!LoadConfiguration())
		{
			return false;
		}

		welcome_message_ = std::make_unique<WelcomeMessage>(config_);

		if (!ParseGroups())
		{
			return false;
		}

		return true;
	}

	void AdvertManager::Update()
	{
		for (auto& group : advert_groups_)
		{
			group->Update();
		}
	}

	bool AdvertManager::Reload()
	{
		return Initialize();
	}

	void AdvertManager::OnNewPlayer(unsigned long long steam_id)
	{
		if (welcome_message_->Enabled())
		{
			auto f = [steam_id](WelcomeMessage* welcome_message)
			{
				AShooterPlayerController* controller = ArkApi::GetApiUtils().FindPlayerFromSteamId(steam_id);

				if (controller)
				{
					welcome_message->Send(controller);
				}
			};

			Timer(5000, true, std::move(f), welcome_message_.get());
		}
	}

	bool AdvertManager::LoadConfiguration()
	{
		config_ = {};

		std::ifstream f(CONFIG_PATH);

		if (!f.is_open())
		{
			Log::GetLog()->error("Can't open config.json");
			return false;
		}

		try
		{
			f >> config_;
		}
		catch (const std::exception&)
		{
			Log::GetLog()->error("Bad JSON");
			return false;
		}
		f.close();

		return true;
	}

	bool AdvertManager::ParseGroups()
	{
		advert_groups_.clear();

		try
		{
			const FString sender = FString(static_cast<std::string>(config_["SenderName"]));

			for (const auto& group : config_["AdvertGroups"])
			{
				auto group_ptr = std::make_unique<AdvertGroup>(group, sender);
				advert_groups_.push_back(std::move(group_ptr));
			}
			return true;
		}
		catch (const std::exception&)
		{
			Log::GetLog()->error("Can't parse Advert Groups");
			return false;
		}
	}
}