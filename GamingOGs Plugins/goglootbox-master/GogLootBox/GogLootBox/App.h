#ifndef _LOOTBOX_APP_H_
#define _LOOTBOX_APP_H_

#ifdef GOGLOOTBOX_EXPORTS
#define GOGLOOTBOX_API __declspec(dllexport)
#else
#define GOGLOOTBOX_API __declspec(dllimport)
#endif

#include <vector>
#include <memory>

#include "Database.h"
#include "Types.h"
#include "Utils.h"

namespace LootBox
{

	class Box;
	class Player;

	class App
	{
	public:
		static App& Get()
		{
			static App instance;
			return instance;
		}

		void Initialize();
		void Shutdown();
		void OpenBoxChat(AShooterPlayerController* playerController, FString* message, EChatSendMode::Type /*unused*/);
		void ShowBoxesChat(AShooterPlayerController* playerController, FString* message, EChatSendMode::Type /*unused*/);
		void GambleChat(AShooterPlayerController* playerController, FString* message, EChatSendMode::Type /*unused*/);
		void TradeChat(AShooterPlayerController* playerController, FString* message, EChatSendMode::Type /*unused*/);
		void TraderIdChat(AShooterPlayerController* playerController, FString* message, EChatSendMode::Type /*unused*/);
		void AddBoxesAdmin(APlayerController* controller, FString* cmd, bool /*unused*/);
		void AddBoxesRcon(RCONClientConnection* rconConnection, RCONPacket* rconPacket, UWorld*);
		void SetBoxesAdmin(APlayerController* controller, FString* cmd, bool /*unused*/);
		void SetBoxesRcon(RCONClientConnection* rconConnection, RCONPacket* rconPacket, UWorld*);
		void RemoveBoxesAdmin(APlayerController* controller, FString* cmd, bool /*unused*/);
		void RemoveBoxesRcon(RCONClientConnection* rconConnection, RCONPacket* rconPacket, UWorld*);
		void RandBoxesAdmin(APlayerController* controller, FString* cmd, bool /*unused*/);
		void RandBoxesRcon(RCONClientConnection* rconConnection, RCONPacket* rconPacket, UWorld*);
		void AddBoxesAllAdmin(APlayerController* controller, FString* cmd, bool /*unused*/);
		void AddBoxesAllRcon(RCONClientConnection* rconConnection, RCONPacket* rconPacket, UWorld*);
		void SetBoxesAllAdmin(APlayerController* controller, FString* cmd, bool /*unused*/);
		void SetBoxesAllRcon(RCONClientConnection* rconConnection, RCONPacket* rconPacket, UWorld*);
		void RemoveBoxesAllAdmin(APlayerController* controller, FString* cmd, bool /*unused*/);
		void RemoveBoxesAllRcon(RCONClientConnection* rconConnection, RCONPacket* rconPacket, UWorld*);
		void RandBoxesAllAdmin(APlayerController* controller, FString* cmd, bool /*unused*/);
		void RandBoxesAllRcon(RCONClientConnection* rconConnection, RCONPacket* rconPacket, UWorld*);
		void EnableBoxesAdmin(APlayerController* controller, FString* cmd, bool /*unused*/);
		void EnableBoxesRcon(RCONClientConnection* rconConnection, RCONPacket* rconPacket, UWorld*);
		void DisableBoxesAdmin(APlayerController* controller, FString* cmd, bool /*unused*/);
		void DisableBoxesRcon(RCONClientConnection* rconConnection, RCONPacket* rconPacket, UWorld*);

		void OnLogin(AShooterPlayerController* controller);
		void OnLogout(AShooterPlayerController* controller);

		bool LootBoxExternal_IsEnabled() { return m_IsBoxesEnabled(); }
		std::string LootBoxExternal_GiveBoxes(uint64 steamId, const std::string& boxName, int amount, const std::string& message, const std::string& sender);
		Json LootBoxExternal_GetPlayerBoxes(uint64 steamId);
		std::string LootBoxExternal_RemoveBoxes(uint64 steamId, const std::string& boxName, int amount);

	private:
		void m_SetCommands();
		void m_RemoveCommands();

		std::shared_ptr<Player> m_GetOrLoadPlayer(uint64 steamId);
		std::shared_ptr<Player> m_GetOrLoadPlayerByTraderId(uint64 traderId);
		void m_RemovePlayer(uint64 steamId);
		bool m_IsBoxesEnabled() const;
		std::string m_BoxesDisabledFor() const;
		void m_OpenBox(const std::shared_ptr<Box>& box, const std::shared_ptr<Player>& player, AShooterPlayerController* controller);

		Message m_AddBoxesImpl(FString* cmd, const FString& commandDescriptiveName);
		Message m_SetBoxesImpl(FString* cmd, const FString& commandDescriptiveName);
		Message m_RemoveBoxesImpl(FString* cmd, const FString& commandDescriptiveName);
		Message m_RandBoxesImpl(FString* cmd, const FString& commandDescriptiveName);
		Message m_AddBoxesAllImpl(FString* cmd, const FString& commandDescriptiveName);
		Message m_SetBoxesAllImpl(FString* cmd, const FString& commandDescriptiveName);
		Message m_RemoveBoxesAllImpl(FString* cmd, const FString& commandDescriptiveName);
		Message m_RandBoxesAllImpl(FString* cmd, const FString& commandDescriptiveName);
		Message m_EnableBoxesImpl(FString* cmd, const FString& commandDescriptiveName);
		Message m_DisableBoxesImpl(FString* cmd, const FString& commandDescriptiveName);

		Message m_AddBoxes(uint64 steamId, const std::shared_ptr<Box>& box, int amount, const FString& commandDescriptiveName);
		Message m_SetBoxes(uint64 steamId, const std::shared_ptr<Box>& box, int amount, const FString& commandDescriptiveName);
		Message m_RemoveBoxes(uint64 steamId, const std::shared_ptr<Box>& box, int amount, const FString& commandDescriptiveName);
		Message m_RandBoxes(const BoxesList& randomList, uint64 steamId, int amount, const FString& commandDescriptiveName);

		PlayersList m_activePlayers;
		Database::Database m_db;
		std::optional<std::time_t> m_boxesDisabledUntil;
	};

}

#endif // !_LOOTBOX_APP_H_
