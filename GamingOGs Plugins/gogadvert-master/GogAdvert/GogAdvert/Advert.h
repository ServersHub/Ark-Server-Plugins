#pragma once

#include <json.hpp>
#include <API/UE/Containers/FString.h>
#include <API/UE/Math/Color.h>

struct AShooterPlayerController;

namespace GogAdvert
{
	enum MessageType
	{
		Chat = 0,
		ServerChat = 1,
		Notification = 2,
	};

	class Advert
	{
	public:
		Advert(const nlohmann::basic_json<>& j, const FString& sender_name);
		void Send();
		void Send(AShooterPlayerController* controller);

	private:
		FString sender_name_;
		FString message_;
		FLinearColor color_;
		float display_scale_ = 0.0f;
		float display_time_ = 0.0f;
		MessageType type_ = Chat;
	};
}