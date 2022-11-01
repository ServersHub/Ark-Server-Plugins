#pragma once

#include <vector>
#include <memory>
#include <json.hpp>
#include <Logger/Logger.h>
#include <API/UE/Containers/FString.h>
#include <fstream>
#include <Tools.h>

#include "Advert.h"

struct AShooterPlayerController;

namespace GogAdvert
{
	class AdvertGroup;

	class AdvertManager
	{
		class WelcomeMessage
		{
		public:
			WelcomeMessage(const nlohmann::basic_json<>& j)
				: enabled_(false)
			{
				try
				{
					enabled_ = j["WelcomeMessageEnabled"];

					if (enabled_)
					{
						const FString sender = FString(static_cast<std::string>(j["SenderName"]));
						advert_ = std::make_unique<Advert>(j["WelcomeMessage"], sender);
					}
				}
				catch (const std::exception & e)
				{
					Log::GetLog()->error("Welcome message parsing error: {}", e.what());
				}
			}

			bool Enabled() const
			{
				return enabled_;
			}

			void Send(AShooterPlayerController* controller)
			{
				if (advert_)
				{
					advert_->Send(controller);
				}
			}

		private:
			bool enabled_;
			std::unique_ptr<Advert> advert_;
		};

	public:
		AdvertManager();
		~AdvertManager();

		static AdvertManager& Get()
		{
			static AdvertManager instance_;
			return instance_;
		}

		bool Initialize();
		void Update();
		bool Reload();
		void OnNewPlayer(unsigned long long steam_id);

	private:
		bool LoadConfiguration();
		bool ParseGroups();

		nlohmann::json config_;
		std::unique_ptr<WelcomeMessage> welcome_message_;
		std::vector<std::unique_ptr<AdvertGroup>> advert_groups_;
	};

	inline std::string GetLicenseKey()
	{
		const std::string configPath = ArkApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/Adverts/config.json";
		std::ifstream file{ configPath };

		if (!file.is_open())
		{
			throw std::runtime_error("Can't open config.json");
		}

		nlohmann::json j{};
		file >> j;
		file.close();

		if (!j.contains("/LicenseKey"_json_pointer))
		{
			throw std::runtime_error("Can't load license key");
		}

		return j["/LicenseKey"_json_pointer].get<std::string>();
	}
}