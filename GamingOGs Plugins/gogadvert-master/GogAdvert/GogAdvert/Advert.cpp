#include "Advert.h"

#include "ApiProxy.h"

namespace GogAdvert
{
	Advert::Advert(const nlohmann::basic_json<>& j, const FString& sender_name)
		: sender_name_(sender_name)
	{
		message_ = FString(ArkApi::Tools::Utf8Decode(j["Message"]));
		display_scale_ = j["DisplayScale"];
		display_time_ = j["DisplayTime"];
		const auto color = j["Color"];
		color_ = FLinearColor{ color[0], color[1], color[2], color[3] };

		const std::string type = j["Type"];
		if (type == "ClientChat")
		{
			type_ = Chat;
		}
		else if (type == "ServerChat")
		{
			type_ = ServerChat;
		}
		else
		{
			type_ = Notification;
		}
	}

	void Advert::Send()
	{
		if (type_ == Chat)
		{
			ArkApi::GetApiUtils().SendChatMessageToAll(sender_name_, *message_);
		}
		else if (type_ == ServerChat)
		{
			ArkApi::GetApiUtils().SendServerMessageToAll(color_, *message_);
		}
		else
		{
			ArkApi::GetApiUtils().SendNotificationToAll(color_, display_scale_, display_time_, nullptr, *message_);
		}
	}

	void Advert::Send(AShooterPlayerController* controller)
	{
		if (!controller)
		{
			return;
		}

		if (type_ == Chat)
		{
			ArkApi::GetApiUtils().SendChatMessage(controller, sender_name_, *message_);
		}
		else if (type_ == ServerChat)
		{
			ArkApi::GetApiUtils().SendServerMessage(controller, color_, *message_);
		}
		else
		{
			ArkApi::GetApiUtils().SendNotification(controller, color_, display_scale_, display_time_, nullptr, *message_);
		}
	}
}