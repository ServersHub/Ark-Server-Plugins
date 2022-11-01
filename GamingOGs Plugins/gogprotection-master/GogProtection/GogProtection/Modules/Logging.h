#ifndef _GOG_PROT_LOGGING_H_
#define _GOG_PROT_LOGGING_H_

#include <string>
#include <API/UE/Containers/FString.h>

#include "../Common/Config.h"

namespace Protection
{
	class DiscordLogger
	{
	public:
		DiscordLogger(const DiscordChannelData& chan_data);
		~DiscordLogger();

		void LogNewPlayer(const FString& character_name, const FString& initial_time, unsigned long long steam_id);

	private:
		void LogBase(const std::string& message);

		DiscordChannelData chan_data_;
	};
}

#endif // !_GOG_PROT_LOGGING_H_
