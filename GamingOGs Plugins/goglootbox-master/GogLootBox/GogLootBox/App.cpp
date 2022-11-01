#include "App.h"

#include <API/ARK/Ark.h>
#include <Timer.h>
#include <Permissions/ArkPermissions.h>
#include <ArkShop/Points.h>
#include "Database.h"
#include "MetricsInterface.h"
#include "Configuration.h"
#include "Utils.h"
#include "Player.h"

namespace LootBox
{

	void App::Initialize()
	{
		auto& conf = Configuration::Get();
		conf.Load();

		m_db.Initialize(conf.MysqlHost(), conf.MysqlUser(), conf.MysqlPass(), conf.MysqlDB());

		if (conf.EnableMetrics())
		{
			Metrics::MetricsInterface::Get().Enable();
		}

		m_SetCommands();
	}

	void App::Shutdown()
	{
		m_RemoveCommands();
	}

	void App::OpenBoxChat(AShooterPlayerController* playerController, FString* message, EChatSendMode::Type)
	{
		if (ArkApi::IApiUtils::IsPlayerDead(playerController))
		{
			return;
		}

		if (!m_IsBoxesEnabled())
		{
			SendChatMessage(playerController, Configuration::Get().GetText("BoxesDisabled"), m_BoxesDisabledFor());
			return;
		}

		const auto steamId = ArkApi::GetApiUtils().GetSteamIdFromController(playerController);
		auto player = m_GetOrLoadPlayer(steamId);
		if (player)
		{
			try
			{
				auto [boxName] = CommandParser<std::string>(*message).Parse();
				const auto box = Configuration::Get().GetBox(boxName);
				if (box)
				{
					if (!player->HasBoxes(box->Name(), 1))
					{
						SendChatMessage(playerController, Configuration::Get().GetText("NoBoxesLeft"));
					}
					else if(const FString permissionRequired = box->Permission().c_str(); !permissionRequired.IsEmpty() && !Permissions::IsPlayerHasPermission(player->SteamId(), permissionRequired))
					{
						SendChatMessage(playerController, Configuration::Get().GetText("NoPermission"));
					}
					else
					{
						m_OpenBox(box, player, playerController);
					}
				}
				else
				{
					SendChatMessage(playerController, Configuration::Get().GetText("BoxNotExist"));
				}
			}
			catch (const std::exception&)
			{
				SendChatMessage(playerController, FString::Format("Correct syntax: '{} <box name>'", Configuration::Get().GetChatCommand("OpenBox").ToString()));
			}
		}
		else
		{
			const char* out = "Command: {}, Error: Open box was requested for non-existing player, Requested open box for SteamID: {}";
			Log::GetLog()->error(out, Configuration::Get().GetChatCommand("OpenBox").ToString(), steamId);
			SendChatMessage(playerController, Configuration::Get().GetText("ContactAdmins"));
		}
	}

	void App::ShowBoxesChat(AShooterPlayerController* playerController, FString* message, EChatSendMode::Type)
	{
		if (ArkApi::IApiUtils::IsPlayerDead(playerController))
		{
			return;
		}

		if (!m_IsBoxesEnabled())
		{
			SendChatMessage(playerController, Configuration::Get().GetText("BoxesDisabled"), m_BoxesDisabledFor());
			return;
		}

		const auto steamId = ArkApi::GetApiUtils().GetSteamIdFromController(playerController);
		const auto player = m_GetOrLoadPlayer(steamId);
		if (player)
		{
			std::string boxes{};
			for (const auto& box : player->InventoryBoxes().items())
			{
				std::string boxString = fmt::format("Box: {}, Amount: {}\n", box.key(), box.value().get<int>());
				boxes.append(boxString);
			}
			if (!boxes.empty())
			{
				boxes.erase(boxes.size() - 1, 1);
				SendChatMessage(playerController, Configuration::Get().GetText("ShowBoxes"), boxes);
			}
			else
			{
				SendChatMessage(playerController, Configuration::Get().GetText("ShowBoxesEmpty"));
			}
		}
		else
		{
			const char* out = "Command: {}, Error: Boxes info was requested for non-existing player, Requested boxes info for SteamID: {}";
			Log::GetLog()->error(out, Configuration::Get().GetChatCommand("ShowBoxes").ToString(), steamId);
			SendChatMessage(playerController, Configuration::Get().GetText("ContactAdmins"));
		}
	}

	void App::GambleChat(AShooterPlayerController* playerController, FString* message, EChatSendMode::Type)
	{
		if (ArkApi::IApiUtils::IsPlayerDead(playerController))
		{
			return;
		}

		if (!m_IsBoxesEnabled())
		{
			SendChatMessage(playerController, Configuration::Get().GetText("BoxesDisabled"), m_BoxesDisabledFor());
			return;
		}

		const auto steamId = ArkApi::GetApiUtils().GetSteamIdFromController(playerController);
		const auto player = m_GetOrLoadPlayer(steamId);
		if (player)
		{
			try
			{
				auto [boxName] = CommandParser<std::string>(*message).Parse();
				const auto box = Configuration::Get().GetBox(boxName);

				if (box)
				{
					if (box->GamblePrice() == 0)
					{
						SendChatMessage(playerController, Configuration::Get().GetText("CantGamble"));
						return;
					}

					const FString permissionRequired = FString{ box->Permission().c_str() };
					if (!permissionRequired.IsEmpty() && !Permissions::IsPlayerHasPermission(player->SteamId(), permissionRequired))
					{
						SendChatMessage(playerController, Configuration::Get().GetText("NoPermission"));
						return;
					}
					
					if (ArkShop::Points::GetPoints(player->SteamId()) < box->GamblePrice())
					{
						SendChatMessage(playerController, Configuration::Get().GetText("NoPoints"));
						return;
					}

					const auto num = RandFromRange(1, 100);
					if (num > box->GamblePercent())
					{
						SendChatMessageToAll(Configuration::Get().GetText("Unlucky"), player->CharacterName(), box->GamblePrice(), box->Name());
					}
					else
					{
						player->AddBoxes(box->Name(), 1);
						SendChatMessageToAll(Configuration::Get().GetText("WonBox"), player->CharacterName(), box->Name());
					}
					ArkShop::Points::SpendPoints(box->GamblePrice(), player->SteamId());
				}
				else
				{
					SendChatMessage(playerController, Configuration::Get().GetText("BoxNotExist"));
				}
			}
			catch (const std::exception&)
			{
				SendChatMessage(playerController, FString::Format("Correct syntax: '{} <box name>'", Configuration::Get().GetChatCommand("Gamble").ToString()));
			}
		}
		else
		{
			const char* out = "Command: {}, Error: Gamble operation was requested for non-existing player, Requested gamble operation for SteamID: {}";
			Log::GetLog()->error(out, Configuration::Get().GetChatCommand("Gamble").ToString(), steamId);
			SendChatMessage(playerController, Configuration::Get().GetText("ContactAdmins"));
		}
	}

	void App::TradeChat(AShooterPlayerController* playerController, FString* message, EChatSendMode::Type)
	{
		if (ArkApi::IApiUtils::IsPlayerDead(playerController))
		{
			return;
		}

		if (!m_IsBoxesEnabled())
		{
			SendChatMessage(playerController, Configuration::Get().GetText("BoxesDisabled"), m_BoxesDisabledFor());
			return;
		}

		const auto steamId = ArkApi::GetApiUtils().GetSteamIdFromController(playerController);
		const auto trader = m_GetOrLoadPlayer(steamId);
		if (trader)
		{
			try
			{
				auto [boxName, amount, traderId] = CommandParser<std::string, int, uint64>(*message).Parse();

				const auto box = Configuration::Get().GetBox(boxName);
				if (!box)
				{
					SendChatMessage(playerController, Configuration::Get().GetText("BoxNotExist"));
					return;
				}

				auto receiver = m_GetOrLoadPlayerByTraderId(traderId);
				if (!receiver)
				{
					SendChatMessage(playerController, Configuration::Get().GetText("WrongTraderId"));
					return;
				}

				if (!trader->HasBoxes(boxName, amount))
				{
					SendChatMessage(playerController, Configuration::Get().GetText("NoBoxesLeft"));
					return;
				}

				trader->RemoveBoxes(boxName, amount);
				receiver->AddBoxes(boxName, amount);

				AShooterPlayerController* receiverPlayerController = ArkApi::GetApiUtils().FindPlayerFromSteamId(receiver->SteamId());
				if (receiverPlayerController)
				{
					SendChatMessage(receiverPlayerController, Configuration::Get().GetText("TradedBoxes"), amount, boxName, trader->CharacterName());
				}
				SendChatMessageToAll(Configuration::Get().GetText("TradeBoxesBroadcast"), trader->CharacterName(), amount, receiver->CharacterName());
				SendChatMessage(playerController, Configuration::Get().GetText("TradedBoxesSuccess"));
				Metrics::MetricsInterface::Get().LogTradeBoxes(box, amount, trader);
			}
			catch (const std::exception&)
			{
				SendChatMessage(playerController, FString::Format("Correct syntax: '{} <box name> <boxes amount> <player trader id>'", Configuration::Get().GetChatCommand("TradeBox").ToString()));
			}
		}
		else
		{
			const char* out = "Command: {}, Error: Trade boxes operation was requested for non-existing player, Requested operation for SteamID: {}";
			Log::GetLog()->error(out, Configuration::Get().GetChatCommand("TradeBox").ToString(), steamId);
			SendChatMessage(playerController, Configuration::Get().GetText("ContactAdmins"));
		}
	}

	void App::TraderIdChat(AShooterPlayerController* playerController, FString* message, EChatSendMode::Type)
	{
		if (ArkApi::IApiUtils::IsPlayerDead(playerController))
		{
			return;
		}

		if (!m_IsBoxesEnabled())
		{
			SendChatMessage(playerController, Configuration::Get().GetText("BoxesDisabled"), m_BoxesDisabledFor());
			return;
		}

		const auto steamId = ArkApi::GetApiUtils().GetSteamIdFromController(playerController);
		const auto player = m_GetOrLoadPlayer(steamId);
		if (player)
		{
			SendChatMessage(playerController, Configuration::Get().GetText("TraderId"), player->TraderId());
		}
		else
		{
			const char* out = "Command: {}, Error: Trader info was requested for non-existing player, Requested trader info for SteamID: {}";
			Log::GetLog()->error(out, Configuration::Get().GetChatCommand("TraderId").ToString(), steamId);
			SendChatMessage(playerController, Configuration::Get().GetText("ContactAdmins"));
		}
	}

	void App::AddBoxesAdmin(APlayerController* controller, FString* cmd, bool)
	{
		auto msg = m_AddBoxesImpl(cmd, Configuration::Get().GetConsoleCommand("GiveBox"));
		SendServerMessage(controller, std::move(msg));
	}

	void App::AddBoxesRcon(RCONClientConnection* rconConnection, RCONPacket* rconPacket, UWorld*)
	{
		auto msg = m_AddBoxesImpl(&rconPacket->Body, Configuration::Get().GetRconCommand("GiveBox"));
		SendRconMessage(rconConnection, rconPacket, std::move(msg));
	}

	void App::SetBoxesAdmin(APlayerController* controller, FString* cmd, bool)
	{
		auto msg = m_SetBoxesImpl(cmd, Configuration::Get().GetConsoleCommand("SetBox"));
		SendServerMessage(controller, std::move(msg));
	}

	void App::SetBoxesRcon(RCONClientConnection* rconConnection, RCONPacket* rconPacket, UWorld*)
	{
		auto msg = m_SetBoxesImpl(&rconPacket->Body, Configuration::Get().GetRconCommand("SetBox"));
		SendRconMessage(rconConnection, rconPacket, std::move(msg));
	}

	void App::RemoveBoxesAdmin(APlayerController* controller, FString* cmd, bool)
	{
		auto msg = m_RemoveBoxesImpl(cmd, Configuration::Get().GetConsoleCommand("RemoveBox"));
		SendServerMessage(controller, std::move(msg));
	}

	void App::RemoveBoxesRcon(RCONClientConnection* rconConnection, RCONPacket* rconPacket, UWorld*)
	{
		auto msg = m_RemoveBoxesImpl(&rconPacket->Body, Configuration::Get().GetRconCommand("RemoveBox"));
		SendRconMessage(rconConnection, rconPacket, std::move(msg));
	}

	void App::RandBoxesAdmin(APlayerController* controller, FString* cmd, bool)
	{
		auto msg = m_RandBoxesImpl(cmd, Configuration::Get().GetConsoleCommand("RandBox"));
		SendServerMessage(controller, std::move(msg));
	}

	void App::RandBoxesRcon(RCONClientConnection* rconConnection, RCONPacket* rconPacket, UWorld*)
	{
		auto msg = m_RandBoxesImpl(&rconPacket->Body, Configuration::Get().GetRconCommand("RandBox"));
		SendRconMessage(rconConnection, rconPacket, std::move(msg));
	}

	void App::AddBoxesAllAdmin(APlayerController* controller, FString* cmd, bool)
	{
		auto msg = m_AddBoxesAllImpl(cmd, Configuration::Get().GetConsoleCommand("GiveBoxAll"));
		SendServerMessage(controller, std::move(msg));
	}

	void App::AddBoxesAllRcon(RCONClientConnection* rconConnection, RCONPacket* rconPacket, UWorld*)
	{
		auto msg = m_AddBoxesAllImpl(&rconPacket->Body, Configuration::Get().GetRconCommand("GiveBoxAll"));
		SendRconMessage(rconConnection, rconPacket, std::move(msg));
	}

	void App::SetBoxesAllAdmin(APlayerController* controller, FString* cmd, bool)
	{
		auto msg = m_SetBoxesAllImpl(cmd, Configuration::Get().GetConsoleCommand("SetBoxAll"));
		SendServerMessage(controller, std::move(msg));
	}

	void App::SetBoxesAllRcon(RCONClientConnection* rconConnection, RCONPacket* rconPacket, UWorld*)
	{
		auto msg = m_SetBoxesAllImpl(&rconPacket->Body, Configuration::Get().GetRconCommand("SetBoxAll"));
		SendRconMessage(rconConnection, rconPacket, std::move(msg));
	}

	void App::RemoveBoxesAllAdmin(APlayerController* controller, FString* cmd, bool)
	{
		auto msg = m_RemoveBoxesAllImpl(cmd, Configuration::Get().GetConsoleCommand("RemoveBoxAll"));
		SendServerMessage(controller, std::move(msg));
	}

	void App::RemoveBoxesAllRcon(RCONClientConnection* rconConnection, RCONPacket* rconPacket, UWorld*)
	{
		auto msg = m_RemoveBoxesAllImpl(&rconPacket->Body, Configuration::Get().GetRconCommand("RemoveBoxAll"));
		SendRconMessage(rconConnection, rconPacket, std::move(msg));
	}

	void App::RandBoxesAllAdmin(APlayerController* controller, FString* cmd, bool)
	{
		auto msg = m_RandBoxesAllImpl(cmd, Configuration::Get().GetConsoleCommand("RandBoxAll"));
		SendServerMessage(controller, std::move(msg));
	}

	void App::RandBoxesAllRcon(RCONClientConnection* rconConnection, RCONPacket* rconPacket, UWorld*)
	{
		auto msg = m_RandBoxesAllImpl(&rconPacket->Body, Configuration::Get().GetRconCommand("RandBoxAll"));
		SendRconMessage(rconConnection, rconPacket, std::move(msg));
	}

	void App::EnableBoxesAdmin(APlayerController* controller, FString* cmd, bool)
	{
		auto msg = m_EnableBoxesImpl(cmd, Configuration::Get().GetConsoleCommand("EnableBoxes"));
		SendServerMessage(controller, std::move(msg));
	}

	void App::EnableBoxesRcon(RCONClientConnection* rconConnection, RCONPacket* rconPacket, UWorld*)
	{
		auto msg = m_EnableBoxesImpl(&rconPacket->Body, Configuration::Get().GetRconCommand("EnableBoxes"));
		SendRconMessage(rconConnection, rconPacket, std::move(msg));
	}

	void App::DisableBoxesAdmin(APlayerController* controller, FString* cmd, bool)
	{
		auto msg = m_DisableBoxesImpl(cmd, Configuration::Get().GetConsoleCommand("DisableBoxes"));
		SendServerMessage(controller, std::move(msg));
	}

	void App::DisableBoxesRcon(RCONClientConnection* rconConnection, RCONPacket* rconPacket, UWorld*)
	{
		auto msg = m_DisableBoxesImpl(&rconPacket->Body, Configuration::Get().GetRconCommand("DisableBoxes"));
		SendRconMessage(rconConnection, rconPacket, std::move(msg));
	}

	void App::OnLogin(AShooterPlayerController* controller)
	{
		FString mapName;
		ArkApi::GetApiUtils().GetShooterGameMode()->GetMapName(&mapName);
		Configuration::Get().CheckMapCompatibility(mapName.ToString());

		const uint64 playerId = ArkApi::GetApiUtils().GetPlayerID(controller);
		if (playerId > 0ULL)
		{
			const uint64 steamId = ArkApi::GetApiUtils().GetSteamIdFromController(controller);
			std::shared_ptr<Player> player = nullptr;
			if (m_db.IsPlayerExist(steamId))
			{
				player = m_db.LoadPlayer(steamId);
				if (!player)
				{
					return;
				}
				m_activePlayers.push_back(player);
			}
			else
			{
				const uint64 traderId = m_db.LoadLastTraderId() + 1;
				const FString steamName = ArkApi::GetApiUtils().GetSteamName(controller);
				player = m_db.AddPlayer(steamId, traderId, "", steamName.ToString());
				m_activePlayers.push_back(player);
			}

			API::Timer::Get().DelayExecute([player]() {
				if (player)
				{
					auto controller = ArkApi::GetApiUtils().FindPlayerFromSteamId(player->SteamId());
					if (controller)
					{
						const FString characterName = ArkApi::GetApiUtils().GetCharacterName(controller);
						player->Rename(characterName.ToString());
					}
				}
			}, 60);
		}
	}

	void App::OnLogout(AShooterPlayerController* controller)
	{
		const uint64 steamId = ArkApi::GetApiUtils().GetSteamIdFromController(controller);
		m_RemovePlayer(steamId);
	}

	std::string App::LootBoxExternal_GiveBoxes(uint64 steamId, const std::string& boxName, int amount, const std::string& message, const std::string& sender)
	{
		if (!m_IsBoxesEnabled())
		{
			return std::string{ "Boxes are disabled" };
		}

		auto player = m_GetOrLoadPlayer(steamId);
		if (!player)
		{
			Log::GetLog()->error("External plugin requests non-existing player. SteamID: {}", steamId);
			return std::string{ "Player is not exist" };
		}

		const auto box = Configuration::Get().GetBox(boxName);
		if (!box)
		{
			Log::GetLog()->error("External plugin requests non-existing box. Box name: {}", boxName);
			return std::string{ "Box is not exist" };
		}

		player->AddBoxes(boxName, amount);
		
		auto controller = ArkApi::GetApiUtils().FindPlayerFromSteamId(steamId);
		if (controller)
		{
			ArkApi::GetApiUtils().SendChatMessage(controller, FString{ sender.c_str() }, message.c_str());
		}

		return std::string{ "" };
	}

	Json App::LootBoxExternal_GetPlayerBoxes(uint64 steamId)
	{
		if (!m_IsBoxesEnabled())
		{
			return std::string{ "Boxes are disabled" };
		}

		const auto player = m_GetOrLoadPlayer(steamId);
		if (player)
		{
			return player->InventoryBoxes();
		}
		else
		{
			Log::GetLog()->error("External plugin requests non-existing player. SteamID: {}", steamId);
			return Json::object();
		}
	}

	std::string App::LootBoxExternal_RemoveBoxes(uint64 steamId, const std::string& boxName, int amount)
	{
		if (!m_IsBoxesEnabled())
		{
			return std::string{ "Boxes are disabled" };
		}

		auto player = m_GetOrLoadPlayer(steamId);
		if (!player)
		{
			Log::GetLog()->error("External plugin requests non-existing player. SteamID: {}", steamId);
			return std::string{ "Player is not exist" };
		}

		const auto box = Configuration::Get().GetBox(boxName);
		if (!box)
		{
			Log::GetLog()->error("External plugin requests non-existing box. Box name: {}", boxName);
			return std::string{ "Box is not exist" };
		}

		player->RemoveBoxes(boxName, amount);
		return std::string{ "" };
	}

	void App::m_SetCommands()
	{
		auto& commands = ArkApi::GetCommands();
		auto& conf = Configuration::Get();

		commands.AddChatCommand(conf.GetChatCommand("OpenBox"), [this](AShooterPlayerController* playerController, FString* message, EChatSendMode::Type type) { this->OpenBoxChat(playerController, message, type); });
		commands.AddChatCommand(conf.GetChatCommand("ShowBoxes"), [this](AShooterPlayerController* playerController, FString* message, EChatSendMode::Type type) { this->ShowBoxesChat(playerController, message, type); });
		commands.AddChatCommand(conf.GetChatCommand("Gamble"), [this](AShooterPlayerController* playerController, FString* message, EChatSendMode::Type type) { this->GambleChat(playerController, message, type); });
		commands.AddChatCommand(conf.GetChatCommand("TradeBox"), [this](AShooterPlayerController* playerController, FString* message, EChatSendMode::Type type) { this->TradeChat(playerController, message, type); });
		commands.AddChatCommand(conf.GetChatCommand("TraderId"), [this](AShooterPlayerController* playerController, FString* message, EChatSendMode::Type type) { this->TraderIdChat(playerController, message, type); });

		commands.AddConsoleCommand(conf.GetConsoleCommand("GiveBox"), [this](APlayerController* controller, FString* cmd, bool doLog) { this->AddBoxesAdmin(controller, cmd, doLog); });
		commands.AddConsoleCommand(conf.GetConsoleCommand("SetBox"), [this](APlayerController* controller, FString* cmd, bool doLog) { this->SetBoxesAdmin(controller, cmd, doLog); });
		commands.AddConsoleCommand(conf.GetConsoleCommand("RemoveBox"), [this](APlayerController* controller, FString* cmd, bool doLog) { this->RemoveBoxesAdmin(controller, cmd, doLog); });
		commands.AddConsoleCommand(conf.GetConsoleCommand("RandBox"), [this](APlayerController* controller, FString* cmd, bool doLog) { this->RandBoxesAdmin(controller, cmd, doLog); });

		commands.AddConsoleCommand(conf.GetConsoleCommand("GiveBoxAll"), [this](APlayerController* controller, FString* cmd, bool doLog) { this->AddBoxesAllAdmin(controller, cmd, doLog); });
		commands.AddConsoleCommand(conf.GetConsoleCommand("SetBoxAll"), [this](APlayerController* controller, FString* cmd, bool doLog) { this->SetBoxesAllAdmin(controller, cmd, doLog); });
		commands.AddConsoleCommand(conf.GetConsoleCommand("RemoveBoxAll"), [this](APlayerController* controller, FString* cmd, bool doLog) { this->RemoveBoxesAllAdmin(controller, cmd, doLog); });
		commands.AddConsoleCommand(conf.GetConsoleCommand("RandBoxAll"), [this](APlayerController* controller, FString* cmd, bool doLog) { this->RandBoxesAllAdmin(controller, cmd, doLog); });

		commands.AddConsoleCommand(conf.GetConsoleCommand("EnableBoxes"), [this](APlayerController* controller, FString* cmd, bool doLog) { this->EnableBoxesAdmin(controller, cmd, doLog); });
		commands.AddConsoleCommand(conf.GetConsoleCommand("DisableBoxes"), [this](APlayerController* controller, FString* cmd, bool doLog) { this->DisableBoxesAdmin(controller, cmd, doLog); });

		commands.AddRconCommand(conf.GetRconCommand("GiveBox"), [this](RCONClientConnection* rconConnection, RCONPacket* rconPacket, UWorld* world) { this->AddBoxesRcon(rconConnection, rconPacket, world); });
		commands.AddRconCommand(conf.GetRconCommand("SetBox"), [this](RCONClientConnection* rconConnection, RCONPacket* rconPacket, UWorld* world) { this->SetBoxesRcon(rconConnection, rconPacket, world); });
		commands.AddRconCommand(conf.GetRconCommand("RemoveBox"), [this](RCONClientConnection* rconConnection, RCONPacket* rconPacket, UWorld* world) { this->RemoveBoxesRcon(rconConnection, rconPacket, world); });
		commands.AddRconCommand(conf.GetRconCommand("RandBox"), [this](RCONClientConnection* rconConnection, RCONPacket* rconPacket, UWorld* world) { this->RandBoxesRcon(rconConnection, rconPacket, world); });

		commands.AddRconCommand(conf.GetRconCommand("GiveBoxAll"), [this](RCONClientConnection* rconConnection, RCONPacket* rconPacket, UWorld* world) { this->AddBoxesAllRcon(rconConnection, rconPacket, world); });
		commands.AddRconCommand(conf.GetRconCommand("SetBoxAll"), [this](RCONClientConnection* rconConnection, RCONPacket* rconPacket, UWorld* world) { this->SetBoxesAllRcon(rconConnection, rconPacket, world); });
		commands.AddRconCommand(conf.GetRconCommand("RemoveBoxAll"), [this](RCONClientConnection* rconConnection, RCONPacket* rconPacket, UWorld* world) { this->RemoveBoxesAllRcon(rconConnection, rconPacket, world); });
		commands.AddRconCommand(conf.GetRconCommand("RandBoxAll"), [this](RCONClientConnection* rconConnection, RCONPacket* rconPacket, UWorld* world) { this->RandBoxesAllRcon(rconConnection, rconPacket, world); });

		commands.AddRconCommand(conf.GetRconCommand("EnableBoxes"), [this](RCONClientConnection* rconConnection, RCONPacket* rconPacket, UWorld* world) { this->EnableBoxesRcon(rconConnection, rconPacket, world); });
		commands.AddRconCommand(conf.GetRconCommand("DisableBoxes"), [this](RCONClientConnection* rconConnection, RCONPacket* rconPacket, UWorld* world) { this->DisableBoxesRcon(rconConnection, rconPacket, world); });
	}

	void App::m_RemoveCommands()
	{
		auto& commands = ArkApi::GetCommands();
		auto& conf = Configuration::Get();

		commands.RemoveChatCommand(conf.GetChatCommand("OpenBox"));
		commands.RemoveChatCommand(conf.GetChatCommand("ShowBoxes"));
		commands.RemoveChatCommand(conf.GetChatCommand("Gamble"));
		commands.RemoveChatCommand(conf.GetChatCommand("TradeBox"));
		commands.RemoveChatCommand(conf.GetChatCommand("TraderId"));

		commands.RemoveConsoleCommand(conf.GetConsoleCommand("GiveBox"));
		commands.RemoveConsoleCommand(conf.GetConsoleCommand("SetBox"));
		commands.RemoveConsoleCommand(conf.GetConsoleCommand("RemoveBox"));
		commands.RemoveConsoleCommand(conf.GetConsoleCommand("RandBox"));

		commands.RemoveConsoleCommand(conf.GetConsoleCommand("GiveBoxAll"));
		commands.RemoveConsoleCommand(conf.GetConsoleCommand("SetBoxAll"));
		commands.RemoveConsoleCommand(conf.GetConsoleCommand("RemoveBoxAll"));
		commands.RemoveConsoleCommand(conf.GetConsoleCommand("RandBoxAll"));

		commands.RemoveConsoleCommand(conf.GetConsoleCommand("EnableBoxes"));
		commands.RemoveConsoleCommand(conf.GetConsoleCommand("DisableBoxes"));

		commands.RemoveRconCommand(conf.GetRconCommand("GiveBox"));
		commands.RemoveRconCommand(conf.GetRconCommand("SetBox"));
		commands.RemoveRconCommand(conf.GetRconCommand("RemoveBox"));
		commands.RemoveRconCommand(conf.GetRconCommand("RandBox"));

		commands.RemoveRconCommand(conf.GetRconCommand("GiveBoxAll"));
		commands.RemoveRconCommand(conf.GetRconCommand("SetBoxAll"));
		commands.RemoveRconCommand(conf.GetRconCommand("RemoveBoxAll"));
		commands.RemoveRconCommand(conf.GetRconCommand("RandBoxAll"));

		commands.RemoveRconCommand(conf.GetRconCommand("EnableBoxes"));
		commands.RemoveRconCommand(conf.GetRconCommand("DisableBoxes"));
	}

	std::shared_ptr<Player> App::m_GetOrLoadPlayer(uint64 steamId)
	{
		const auto it = std::find_if(m_activePlayers.begin(), m_activePlayers.end(), [steamId](const std::shared_ptr<Player>& player) {
			return player->SteamId() == steamId;
			});
		if (it != m_activePlayers.end())
		{
			return *it;
		}
		else
		{
			return m_db.LoadPlayer(steamId);
		}
	}

	std::shared_ptr<Player> App::m_GetOrLoadPlayerByTraderId(uint64 traderId)
	{
		const auto it = std::find_if(m_activePlayers.begin(), m_activePlayers.end(), [traderId](const std::shared_ptr<Player>& player) {
			return player->TraderId() == traderId;
			});
		if (it != m_activePlayers.end())
		{
			return *it;
		}
		else
		{
			return m_db.LoadPlayerByTraderId(traderId);
		}
	}

	void App::m_RemovePlayer(uint64 steamId)
	{
		const auto it = std::find_if(m_activePlayers.begin(), m_activePlayers.end(), [steamId](const std::shared_ptr<Player>& player) {
			return player->SteamId() == steamId;
			});
		if (it != m_activePlayers.end())
		{
			m_activePlayers.erase(it);
		}
	}

	bool App::m_IsBoxesEnabled() const
	{
		return !m_boxesDisabledUntil.has_value();
	}

	std::string App::m_BoxesDisabledFor() const
	{
		if (m_IsBoxesEnabled())
		{
			return "Boxes enabled";
		}

		unsigned long long seconds = m_boxesDisabledUntil.value() - time(nullptr);

		if (seconds < 60)
			return "0 minutes";

		seconds = seconds / 60;

		const uint64 MINUTE = 1;
		const uint64 HOUR = MINUTE * 60;
		const uint64 DAY = HOUR * 24;

		uint64 rDays = 0;
		uint64 rHours = 0;
		uint64 rMinutes = 0;

		if (seconds >= DAY) {
			rDays = seconds / DAY;
			seconds -= rDays * DAY;
		}
		if (seconds >= HOUR) {
			rHours = seconds / HOUR;
			seconds -= rHours * HOUR;
		}
		if (seconds >= MINUTE) {
			rMinutes = seconds;
		}

		std::string reply{};
		if (rDays > 0)
			reply += fmt::format("{} days ", rDays);
		if (rHours > 0)
			reply += fmt::format("{} hours ", rHours);
		if (rMinutes > 0)
			reply += fmt::format("{} minutes", rMinutes);


		if (!reply.empty() && reply[reply.size() - 1] == ' ')
		{
			reply.erase(reply.size() - 1, 1);
		}

		return reply;
	}

	void App::m_OpenBox(const std::shared_ptr<Box>& box, const std::shared_ptr<Player>& player, AShooterPlayerController* controller)
	{
		if (box->Elements().empty())
		{
			SendChatMessage(controller, Configuration::Get().GetText("IncompatibleMap"));
			return;
		}

		const auto amount = RandFromRange(box->MinAmount(), box->MaxAmount());
		bool globalSuccess = false;
		for (int i = 0; i < amount; i++)
		{
			std::vector<int> probabilities{};
			for (const auto& element : box->Elements())
			{
				probabilities.push_back(element->Probability());
			}
			const auto element = box->Elements()[RandWithProbability(std::move(probabilities))];

			bool success;
			switch (element->Type())
			{
				case BoxElementType::Item:
				{
					const auto itemsElement = std::static_pointer_cast<BoxElementItem>(element);
					for (const auto& item : itemsElement->Items())
					{
						TArray<UPrimalItem*> out;
						FString blueprint = item->Blueprint().c_str();
						success |= controller->GiveItem(
							&out,
							&blueprint,
							item->Amount(),
							static_cast<float>(RandFromRange(item->MinQuality(), item->MaxQuality())),
							RandWithProbability(item->BlueprintChance()),
							false,
							0);
					}
					break;
				}
				case BoxElementType::Dino:
				{
					const auto dinosElement = std::static_pointer_cast<BoxElementDino>(element);
					for (const auto& dino : dinosElement->Dinos())
					{
						auto spawnedDino = ArkApi::GetApiUtils().SpawnDino(
							controller,
							FString{ dino->Blueprint().c_str() },
							nullptr,
							dino->Level(),
							true,
							dino->IsNeutered().has_value() ? dino->IsNeutered().value() : false
						);

						if (spawnedDino && dino->IsBaby())
						{
							spawnedDino->SetBabyAge(0.0);
							spawnedDino->RefreshBabyScaling();
						}

						if (spawnedDino && dino->ImprintAmount() > 0.0f)
						{
							auto player_character = static_cast<AShooterCharacter*>(controller->CharacterField());
							if (player_character)
							{
								auto characterName = ArkApi::GetApiUtils().GetCharacterName(controller);
								auto primal_data = player_character->GetPlayerData();
								if (primal_data)
								{
									auto fprimal_data = primal_data->MyDataField();
									if (fprimal_data)
									{
										auto data_id = fprimal_data->PlayerDataIDField();
										if (data_id > 0)
										{
											spawnedDino->UpdateImprintingDetails(&characterName, data_id);
											spawnedDino->UpdateImprintingQuality(dino->ImprintAmount() / 100.0f);
										}
									}
								}
							}
						}

						success |= spawnedDino != nullptr;

						if (dino->SaddleBlueprint().has_value()) {
							FString saddleBlueprint = dino->SaddleBlueprint()->c_str();
							TArray<UPrimalItem*> out;
							success |= controller->GiveItem(
								&out,
								&saddleBlueprint,
								1,
								static_cast<float>(dino->SaddleQuality().has_value() ? dino->SaddleQuality().value() : 1),
								false,
								false,
								0);
						}
					}
					break;
				}
				case BoxElementType::Beacon:
				{
					const auto beaconElement = std::static_pointer_cast<BoxElementBeacon>(element);
					auto* cheatManager = static_cast<UShooterCheatManager*>(controller->CheatManagerField());
					FString className = beaconElement->ClassName().c_str();
					cheatManager->Summon(&className);
					success = true;
					break;
				}
				case BoxElementType::Command:
				{
					const auto commandElement = std::static_pointer_cast<BoxElementCommand>(element);
					FString result;
					FString command = fmt::format(commandElement->Command(), player->SteamId()).c_str();
					controller->ConsoleCommand(&result, &command, false);
					success = true;
					break;
				}
				case BoxElementType::Points:
				{
					const auto pointsElement = std::static_pointer_cast<BoxElementPoints>(element);
					const auto amount = RandFromRange(pointsElement->MinAmount(), pointsElement->MaxAmount());
					ArkShop::Points::AddPoints(amount, player->SteamId());
					success = true;
					break;
				}
				default:
					;
			}

			if (success)
			{
				SendChatMessage(controller, Configuration::Get().GetText("WonItem"), element->Description());
				Metrics::MetricsInterface::Get().LogOpenBox(box, element, player);
			}
			else
			{
				if (element->Type() == BoxElementType::Item)
				{
					SendChatMessage(controller, Configuration::Get().GetText("CannotGiveItem"));
				}
				else if (element->Type() == BoxElementType::Dino)
				{
					SendChatMessage(controller, Configuration::Get().GetText("CannotGiveDino"));
				}
			}

			globalSuccess |= success;
		}

		if (globalSuccess)
		{
			player->RemoveBoxes(box->Name(), 1);
		}
	}

	Message App::m_AddBoxesImpl(FString* cmd, const FString& commandDescriptiveName)
	{
		if (!m_IsBoxesEnabled())
		{
			return Message{ fmt::format("Boxes are disabled for {}", m_BoxesDisabledFor()), false };
		}

		try
		{
			auto [steamId, boxName, amount] = CommandParser<uint64, std::string, int>(*cmd).Parse();
			const auto box = Configuration::Get().GetBox(boxName);
			if (!box)
			{
				const char* out = "Command: {}, Reason: Box is not exist, Details: Box name {}";
				Log::GetLog()->error(out, commandDescriptiveName.ToString(), boxName);
				return Message{ "Wrong box name", false };
			}
			return m_AddBoxes(steamId, box, amount, commandDescriptiveName);
		}
		catch (const std::exception& e)
		{
			const char* out = "Command: {}, Reason: Wrong syntax '{}', Details: {}";
			Log::GetLog()->error(out, commandDescriptiveName.ToString(), cmd->ToString(), e.what());
			return Message{ fmt::format("Correct syntax '{} <steam_id> <box_name> <amount>'", commandDescriptiveName.ToString()), false };
		}
	}

	Message App::m_SetBoxesImpl(FString* cmd, const FString& commandDescriptiveName)
	{
		if (!m_IsBoxesEnabled())
		{
			return Message{ fmt::format("Boxes are disabled for {}", m_BoxesDisabledFor()), false };
		}

		try
		{
			auto [steamId, boxName, amount] = CommandParser<uint64, std::string, int>(*cmd).Parse();
			const auto box = Configuration::Get().GetBox(boxName);
			if (!box)
			{
				const char* out = "Command: {}, Reason: Box is not exist, Details: Box name {}";
				Log::GetLog()->error(out, commandDescriptiveName.ToString(), boxName);
				return Message{ "Wrong box name", false };
			}
			return m_SetBoxes(steamId, box, amount, commandDescriptiveName);
		}
		catch (const std::exception& e)
		{
			const char* out = "Command: {}, Reason: Wrong syntax '{}', Details: {}";
			Log::GetLog()->error(out, commandDescriptiveName.ToString(), cmd->ToString(), e.what());
			return Message{ fmt::format("Correct syntax '{} <steam_id> <box_name> <amount>'", commandDescriptiveName.ToString()), false };
		}
	}

	Message App::m_RemoveBoxesImpl(FString* cmd, const FString& commandDescriptiveName)
	{
		if (!m_IsBoxesEnabled())
		{
			return Message{ fmt::format("Boxes are disabled for {}", m_BoxesDisabledFor()), false };
		}

		try
		{
			auto [steamId, boxName, amount] = CommandParser<uint64, std::string, int>(*cmd).Parse();
			const auto box = Configuration::Get().GetBox(boxName);
			if (!box)
			{
				const char* out = "Command: {}, Reason: Box is not exist, Details: Box name {}";
				Log::GetLog()->error(out, commandDescriptiveName.ToString(), boxName);
				return Message{ "Wrong box name", false };
			}
			return m_RemoveBoxes(steamId, box, amount, commandDescriptiveName);
		}
		catch (const std::exception& e)
		{
			const char* out = "Command: {}, Reason: Wrong syntax '{}', Details: {}";
			Log::GetLog()->error(out, commandDescriptiveName.ToString(), cmd->ToString(), e.what());
			return Message{ fmt::format("Correct syntax '{} <steam_id> <box_name> <amount>'", commandDescriptiveName.ToString()), false };
		}
	}

	Message App::m_RandBoxesImpl(FString* cmd, const FString& commandDescriptiveName)
	{
		if (!m_IsBoxesEnabled())
		{
			return Message{ fmt::format("Boxes are disabled for {}", m_BoxesDisabledFor()), false };
		}

		try
		{
			auto [steamId, amount] = CommandParser<uint64, int>(*cmd).Parse();
			BoxesList randList{};
			for (const auto& box : Configuration::Get().GetBoxesList())
			{
				if (Configuration::Get().RandomBoxesContainsBox(box->Name()))
				{
					randList.push_back(box);
				}
			}

			if (randList.empty())
			{
				const char* out = "Command: {}, Reason: There are no boxes configured for random";
				Log::GetLog()->error(out, commandDescriptiveName.ToString());
				return Message{ "There are no boxes configured for random", false };
			}

			return m_RandBoxes(randList, steamId, amount, commandDescriptiveName);
		}
		catch (const std::exception& e)
		{
			const char* out = "Command: {}, Reason: Wrong syntax '{}', Details: {}";
			Log::GetLog()->error(out, commandDescriptiveName.ToString(), cmd->ToString(), e.what());
			return Message{ fmt::format("Correct syntax '{} <steam_id> <amount>'", commandDescriptiveName.ToString()), false };
		}
	}

	Message App::m_AddBoxesAllImpl(FString* cmd, const FString& commandDescriptiveName)
	{
		if (!m_IsBoxesEnabled())
		{
			return Message{ fmt::format("Boxes are disabled for {}", m_BoxesDisabledFor()), false };
		}

		try
		{
			auto [boxName, amount] = CommandParser<std::string, int>(*cmd).Parse();
			const auto box = Configuration::Get().GetBox(boxName);
			if (!box)
			{
				const char* out = "Command: {}, Reason: Box is not exist, Details: Box name {}";
				Log::GetLog()->error(out, commandDescriptiveName.ToString(), boxName);
				return Message{ "Wrong box name", false };
			}
			
			auto controllers = ArkApi::GetApiUtils().GetWorld()->PlayerControllerListField();
			int counter = 0;
			for (auto& controller : controllers)
			{
				if (controller.Get())
				{
					const uint64 steamId = ArkApi::GetApiUtils().GetSteamIdFromController(controller.Get());
					auto result = m_AddBoxes(steamId, box, amount, commandDescriptiveName);
					if (result.IsOk())
					{
						counter++;
					}
				}
			}

			return Message{ fmt::format("Players processed: {}", counter), true };
		}
		catch (const std::exception& e)
		{
			const char* out = "Command: {}, Reason: Wrong syntax '{}', Details: {}";
			Log::GetLog()->error(out, commandDescriptiveName.ToString(), cmd->ToString(), e.what());
			return Message{ fmt::format("Correct syntax '{} <box_name> <amount>'", commandDescriptiveName.ToString()), false };
		}
	}

	Message App::m_SetBoxesAllImpl(FString* cmd, const FString& commandDescriptiveName)
	{
		if (!m_IsBoxesEnabled())
		{
			return Message{ fmt::format("Boxes are disabled for {}", m_BoxesDisabledFor()), false };
		}

		try
		{
			auto [boxName, amount] = CommandParser<std::string, int>(*cmd).Parse();
			const auto box = Configuration::Get().GetBox(boxName);
			if (!box)
			{
				const char* out = "Command: {}, Reason: Box is not exist, Details: Box name {}";
				Log::GetLog()->error(out, commandDescriptiveName.ToString(), boxName);
				return Message{ "Wrong box name", false };
			}

			auto controllers = ArkApi::GetApiUtils().GetWorld()->PlayerControllerListField();
			int counter = 0;
			for (auto& controller : controllers)
			{
				if (controller.Get())
				{
					const uint64 steamId = ArkApi::GetApiUtils().GetSteamIdFromController(controller.Get());
					auto result = m_SetBoxes(steamId, box, amount, commandDescriptiveName);
					if (result.IsOk())
					{
						counter++;
					}
				}
			}

			return Message{ fmt::format("Players processed: {}", counter), true };
		}
		catch (const std::exception& e)
		{
			const char* out = "Command: {}, Reason: Wrong syntax '{}', Details: {}";
			Log::GetLog()->error(out, commandDescriptiveName.ToString(), cmd->ToString(), e.what());
			return Message{ fmt::format("Correct syntax '{} <box_name> <amount>'", commandDescriptiveName.ToString()), false };
		}
	}

	Message App::m_RemoveBoxesAllImpl(FString* cmd, const FString& commandDescriptiveName)
	{
		if (!m_IsBoxesEnabled())
		{
			return Message{ fmt::format("Boxes are disabled for {}", m_BoxesDisabledFor()), false };
		}

		try
		{
			auto [boxName, amount] = CommandParser<std::string, int>(*cmd).Parse();
			const auto box = Configuration::Get().GetBox(boxName);
			if (!box)
			{
				const char* out = "Command: {}, Reason: Box is not exist, Details: Box name {}";
				Log::GetLog()->error(out, commandDescriptiveName.ToString(), boxName);
				return Message{ "Wrong box name", false };
			}

			auto controllers = ArkApi::GetApiUtils().GetWorld()->PlayerControllerListField();
			int counter = 0;
			for (auto& controller : controllers)
			{
				if (controller.Get())
				{
					const uint64 steamId = ArkApi::GetApiUtils().GetSteamIdFromController(controller.Get());
					auto result = m_RemoveBoxes(steamId, box, amount, commandDescriptiveName);
					if (result.IsOk())
					{
						counter++;
					}
				}
			}

			return Message{ fmt::format("Players processed: {}", counter), true };
		}
		catch (const std::exception& e)
		{
			const char* out = "Command: {}, Reason: Wrong syntax '{}', Details: {}";
			Log::GetLog()->error(out, commandDescriptiveName.ToString(), cmd->ToString(), e.what());
			return Message{ fmt::format("Correct syntax '{} <box_name> <amount>'", commandDescriptiveName.ToString()), false };
		}
	}

	Message App::m_RandBoxesAllImpl(FString* cmd, const FString& commandDescriptiveName)
	{
		if (!m_IsBoxesEnabled())
		{
			return Message{ fmt::format("Boxes are disabled for {}", m_BoxesDisabledFor()), false };
		}

		try
		{
			auto [amount] = CommandParser<int>(*cmd).Parse();
			BoxesList randList{};
			for (const auto& box : Configuration::Get().GetBoxesList())
			{
				if (Configuration::Get().RandomBoxesContainsBox(box->Name()))
				{
					randList.push_back(box);
				}
			}

			if (randList.empty())
			{
				const char* out = "Command: {}, Reason: There are no boxes configured for random";
				Log::GetLog()->error(out, commandDescriptiveName.ToString());
				return Message{ "There are no boxes configured for random", false };
			}

			auto controllers = ArkApi::GetApiUtils().GetWorld()->PlayerControllerListField();
			int counter = 0;
			for (auto& controller : controllers)
			{
				if (controller.Get())
				{
					const uint64 steamId = ArkApi::GetApiUtils().GetSteamIdFromController(controller.Get());
					auto result = m_RandBoxes(randList, steamId, amount, commandDescriptiveName);
					if (result.IsOk())
					{
						counter++;
					}
				}
			}

			return Message{ fmt::format("Players processed: {}", counter), true };
		}
		catch (const std::exception& e)
		{
			const char* out = "Command: {}, Reason: Wrong syntax '{}', Details: {}";
			Log::GetLog()->error(out, commandDescriptiveName.ToString(), cmd->ToString(), e.what());
			return Message{ fmt::format("Correct syntax '{} <amount>'", commandDescriptiveName.ToString()), false };
		}
	}

	Message App::m_EnableBoxesImpl(FString* cmd, const FString& commandDescriptiveName)
	{
		m_boxesDisabledUntil = std::nullopt;
		return Message{ "Boxes enabled", true };
	}

	Message App::m_DisableBoxesImpl(FString* cmd, const FString& commandDescriptiveName)
	{
		int seconds = 0;
		TArray<FString> parsed;
		cmd->ParseIntoArray(parsed, L" ");

		try
		{
			if (parsed.IsValidIndex(1)) 
			{
				int counter = 1;
				while (counter < parsed.Num()) {
					const auto str = parsed[counter].ToString();
					if (str.find("d") != std::string::npos)
					{
						auto sub = str.substr(0, str.find("d"));
						seconds += std::stoi(sub.c_str()) * 24 * 60 * 60;
					}
					else if (str.find("h") != std::string::npos)
					{
						auto sub = str.substr(0, str.find("h"));
						seconds += std::stoi(sub.c_str()) * 60 * 60;
					}
					else if (str.find("m") != std::string::npos)
					{
						auto sub = str.substr(0, str.find("m"));
						seconds += std::stoi(sub.c_str()) * 60;
					}

					counter++;
				}
			}
			else 
			{
				throw std::runtime_error("Bad command '" + cmd->ToString() + "'");
			}
		}
		catch (const std::exception& e)
		{
			const char* out = "Command: {}, Reason: Wrong syntax '{}', Details: {}";
			Log::GetLog()->error(out, commandDescriptiveName.ToString(), cmd->ToString(), e.what());
			return Message{ fmt::format("Correct syntax '{} 1d 1h 1m'", commandDescriptiveName.ToString()), false };
		}

		m_boxesDisabledUntil = std::make_optional<std::time_t>(std::time(nullptr) + seconds);
		return Message{ fmt::format("Boxes disabled for {}", m_BoxesDisabledFor()), true };
	}

	Message App::m_AddBoxes(uint64 steamId, const std::shared_ptr<Box>& box, int amount, const FString& commandDescriptiveName)
	{
		auto player = m_GetOrLoadPlayer(steamId);
		if (!player)
		{
			const char* out = "Command: {}, Reason: Player is not exist, Details: SteamID {}";
			Log::GetLog()->error(out, commandDescriptiveName.ToString(), steamId);
			return Message{ "Player is not exist", false };
		}

		player->AddBoxes(box->Name(), amount);
		Metrics::MetricsInterface::Get().LogReceiveBoxes(box, amount, player);
		return Message{ "Success", true };
	}

	Message App::m_SetBoxes(uint64 steamId, const std::shared_ptr<Box>& box, int amount, const FString& commandDescriptiveName)
	{
		auto player = m_GetOrLoadPlayer(steamId);
		if (!player)
		{
			const char* out = "Command: {}, Reason: Player is not exist, Details: SteamID {}";
			Log::GetLog()->error(out, commandDescriptiveName.ToString(), steamId);
			return Message{ "Player is not exist", false };
		}

		player->SetBoxes(box->Name(), amount);
		return Message{ "Success", true };
	}

	Message App::m_RemoveBoxes(uint64 steamId, const std::shared_ptr<Box>& box, int amount, const FString& commandDescriptiveName)
	{
		auto player = m_GetOrLoadPlayer(steamId);
		if (!player)
		{
			const char* out = "Command: {}, Reason: Player is not exist, Details: SteamID {}";
			Log::GetLog()->error(out, commandDescriptiveName.ToString(), steamId);
			return Message{ "Player is not exist", false };
		}

		if (player->HasBoxes(box->Name(), amount))
		{
			player->RemoveBoxes(box->Name(), amount);
			return Message{ "Success", true };
		}
		else
		{
			const char* out = "Command: {}, Reason: Player doesn't have boxes left, Details: SteamID {} Box name {}, Requested amount {}";
			Log::GetLog()->error(out, commandDescriptiveName.ToString(), steamId, box->Name(), amount);
			return Message{ "Player doesn't have boxes left", false };
		}

	}

	Message App::m_RandBoxes(const BoxesList& randomList, uint64 steamId, int amount, const FString& commandDescriptiveName)
	{
		auto player = m_GetOrLoadPlayer(steamId);
		if (!player)
		{
			const char* out = "Command: {}, Reason: Player is not exist, Details: SteamID {}";
			Log::GetLog()->error(out, commandDescriptiveName.ToString(), steamId);
			return Message{ "Player is not exist", false };
		}

		if (amount <= 0)
		{
			return Message{ "Bad amount", false };
		}

		for (int i = 0; i < amount; i++)
		{
			std::vector<std::string> allowedBoxes{};
			for (const auto& box : randomList)
			{
				if (!player->HasCooldown(box->Name()))
				{
					allowedBoxes.push_back(box->Name());
				}
			}

			if (allowedBoxes.empty())
			{
				player->DecrementCooldowns();
				continue;
			}

			const auto rnd = RandFromRange(0, static_cast<int>(allowedBoxes.size() - 1));
			player->AddBoxes(allowedBoxes[rnd], 1);
			player->DecrementCooldowns();
			player->SetCooldown(allowedBoxes[rnd], Configuration::Get().RandomBoxesRepeatInterval());
		}

		return Message{ "Success", true };
	}

}