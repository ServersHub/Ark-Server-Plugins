#include "Commands.h"

#include "../utils/ApiProxy.h"
#include "../utils/Config.h"
#include "../utils/Helpers.h"
#include "Application.h"

namespace AutoDecay
{
	namespace Commands
	{
		void CliCheckTribe(AShooterPlayerController* shooter_controller, FString* message, int mode)
		{
			AActor* aimed_actor = Helpers::GetAimedActor(shooter_controller);
			if (aimed_actor && aimed_actor->IsA(APrimalStructure::GetPrivateStaticClass()))
			{
				FString reply = app->CheckTribeCommandImpl(aimed_actor->TargetingTeamField());
				ArkApi::GetApiUtils().SendChatMessage(shooter_controller, GetMsg("Sender"), *reply);
			}
			else
			{
				ArkApi::GetApiUtils().SendChatMessage(shooter_controller, GetMsg("Sender"), *GetMsg("NoTarget"));
			}
		}

		void AdminGetBP(APlayerController* player_controller, FString* message, bool write_log)
		{
			AActor* aimed_actor = Helpers::GetAimedActor(player_controller);
			if (aimed_actor && aimed_actor->IsA(APrimalStructure::GetPrivateStaticClass()))
			{
				APrimalStructure* structure = reinterpret_cast<APrimalStructure*>(aimed_actor);
				FString bp = Helpers::GetStructureBlueprint(structure);
				ArkApi::GetApiUtils().SendChatMessage(reinterpret_cast<AShooterPlayerController*>(player_controller),
					GetMsg("Sender"), *bp);
			}
			else
			{
				ArkApi::GetApiUtils().SendChatMessage(reinterpret_cast<AShooterPlayerController*>(player_controller),
					GetMsg("Sender"), *GetMsg("NoTarget"));
			}
		}

		inline void TimerTick()
		{
			Application::Update();
		}

		void Initialize()
		{
			auto& commands = ArkApi::GetCommands();

			commands.AddOnTimerCallback(L"AutoDecay_UpdateEngine", &TimerTick);
			commands.AddChatCommand(GetCmd("CheckTribe"), &CliCheckTribe);
			commands.AddConsoleCommand(GetCmd("GetBlueprint"), &AdminGetBP);
		}

		void Deinitialize()
		{
			auto& commands = ArkApi::GetCommands();

			commands.RemoveOnTimerCallback(L"AutoDecay_UpdateEngine");
			commands.RemoveChatCommand(GetCmd("CheckTribe"));
			commands.RemoveConsoleCommand(GetCmd("GetBlueprint"));
		}
	}
}