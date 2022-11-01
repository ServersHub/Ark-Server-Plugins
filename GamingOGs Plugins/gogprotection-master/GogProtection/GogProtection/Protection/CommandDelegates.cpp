#include "CommandDelegates.h"

#include "Application.h"
#include "../Storage/Storage.h"
#include "../Common/Config.h"
#include "../Common/Helpers.h"
#include "../Modules/BlackList.h"
#include "../Modules/TeamManager.h"
#include "../Modules/ProtManager.h"

#include <regex>

namespace Protection
{
    CliInfoCommandDelegate::CliInfoCommandDelegate(Application* app)
        : ICommandDelegate(app)
    {
    }

    CommandExecResultPtr CliInfoCommandDelegate::Exec()
    {
        const int protection_hours = static_cast<int>(GetConfig()["AP"]["General"]["DefaultDuration"]) / 60;

        return std::make_unique<CommandDelegateExecResult>(true, FString::Format(*GetText("ChatInfo"), protection_hours));
    }

	CliStatusCommandDelegate::CliStatusCommandDelegate(Application* app, int team_id)
		: ICommandDelegate(app), team_id_(team_id)
	{
	}

	CommandExecResultPtr CliStatusCommandDelegate::Exec()
	{
		TeamItemType team = app_->GetStorage()->Teams()->Get(team_id_);
		CommandExecResultPtr result = std::make_unique<CommandDelegateExecResult>();

		if(team) {
			const unsigned long long remaining_seconds = app_->GetProtManager()->GetProtectionRemainingTime(team, ProtectionType::AP);

			result->SetResult(true);

			if (remaining_seconds > 0)
				result->SetOutput(FString::Format(*GetText("RemainingProtectionTime"),
					Helpers::ToHumanReadableTime(remaining_seconds).ToString()));
			else
				result->SetOutput(GetText("ProtectionExpired"));
		}
		else {
			result->SetResult(false);
			result->SetOutput(L"Unknown");
		}

		return result;
	}

	CliDisableCommandDelegate::CliDisableCommandDelegate(Application* app, int team_id, bool invoked_by_team_admin)
		: ICommandDelegate(app), team_id_(team_id), invoked_by_team_admin_(invoked_by_team_admin)
	{
	}

	CommandExecResultPtr CliDisableCommandDelegate::Exec()
	{
		TeamItemType team = app_->GetStorage()->Teams()->Get(team_id_);
		CommandExecResultPtr result = std::make_unique<CommandDelegateExecResult>();

		if (team) {
			if (invoked_by_team_admin_) {
				app_->GetProtManager()->DisableAP(team);
				result->SetResult(true);
				result->SetOutput(GetText("ProtectionDisable"));
			}
			else {
				result->SetResult(false);
				result->SetOutput(GetText("HaveNotPermissions"));
			}
		}
		else {
			result->SetResult(false);
			result->SetOutput(L"Internal error");
		}

		return result;
	}

	CliSetZoneCommandDelegate::CliSetZoneCommandDelegate(Application* app, int team_id, const std::string& id, const FVector& position)
		: ICommandDelegate(app), team_id_(team_id), id_(id), position_(position)
	{
	}

	CommandExecResultPtr CliSetZoneCommandDelegate::Exec()
	{
		TeamItemType team = app_->GetStorage()->Teams()->Get(team_id_);
		CommandExecResultPtr result = std::make_unique<CommandDelegateExecResult>();

		if (team) {
			if (id_.empty()) {
				result->SetResult(false);
				result->SetOutput(GetText("ORPBadZoneId"));
			} else if (App->GetProtManager()->IsORPZonesLimitExceeded(team)) {
				result->SetResult(false);
				result->SetOutput(GetText("ORPZonesLimitExceeded"));
			}
			else {
				App->GetProtManager()->ORPAddZone(team, id_, &position_);
				result->SetResult(true);
				result->SetOutput(GetText("ORPZoneAdded"));
			}
		}
		else {
			result->SetResult(false);
			result->SetOutput(L"Internal error");
		}

		return result;
	}

	CliUnsetZoneCommandDelegate::CliUnsetZoneCommandDelegate(Application* app, int team_id, const std::string& id)
		: ICommandDelegate(app), team_id_(team_id), id_(id)
	{
	}

	CommandExecResultPtr CliUnsetZoneCommandDelegate::Exec()
	{
		TeamItemType team = app_->GetStorage()->Teams()->Get(team_id_);
		CommandExecResultPtr result = std::make_unique<CommandDelegateExecResult>();

		if (team) {
			if (id_.empty()) {
				result->SetResult(false);
				result->SetOutput(GetText("ORPBadZoneId"));
			}
			else {
				if (App->GetProtManager()->ORPRemoveZone(team, id_)) {
					result->SetResult(true);
					result->SetOutput(GetText("ORPZoneRemoved"));
				}
				else {
					result->SetResult(false);
					result->SetOutput(GetText("ORPZoneNotFound"));
				}
			}
		}
		else {
			result->SetResult(false);
			result->SetOutput(L"Internal error");
		}

		return result;
	}

	CliDisplayZonesCommandDelegate::CliDisplayZonesCommandDelegate(Application* app, int team_id, const FVector& player_position)
		: ICommandDelegate(app), team_id_(team_id), player_position_(player_position)
	{
	}

	CommandExecResultPtr CliDisplayZonesCommandDelegate::Exec()
	{
		TeamItemType team = app_->GetStorage()->Teams()->Get(team_id_);
		CommandExecResultPtr result = std::make_unique<CommandDelegateExecResult>();

		if (team) {
			const std::string zones_info = App->GetProtManager()->ORPGetZonesInfo(team);

			const std::string active_zone_id = App->GetProtManager()->GetActiveORPZoneId(team, &player_position_);

			result->SetResult(true);

			if (zones_info.empty())
				result->SetOutput(GetText("NoORPZones"));
			else {
				FString output = FString::Format(*GetText("ORPZones"), zones_info) + L'\n';

				if (active_zone_id.empty())
					output += GetText("NotInORPZone");
				else
					output += FString::Format(*GetText("InORPZone"), active_zone_id);

				result->SetOutput(*output);
			}
		}
		else {
			result->SetResult(false);
			result->SetOutput(L"Internal error");
		}

		return result;
	}

	DisableCommandDelegate::DisableCommandDelegate(Application* app, unsigned long long steam_id)
		: ICommandDelegate(app), steam_id_(steam_id)
	{
	}

	CommandExecResultPtr DisableCommandDelegate::Exec()
	{
		PlayerItemType player = app_->GetStorage()->Players()->Get(steam_id_);
		CommandExecResultPtr result = std::make_unique<CommandDelegateExecResult>();

		if (player) {
			TeamItemType team = app_->GetStorage()->Teams()->Get(player->GetTeamId());
			if (team) {
				app_->GetProtManager()->DisableAP(team);
				result->SetResult(true);
				result->SetOutput(GetText("ProtectionDisable"));
			}
			else {
				result->SetResult(false);
				result->SetOutput(GetText("PlayerNotFound"));
			}
		}
		else {
			result->SetResult(false);
			result->SetOutput(GetText("PlayerNotFound"));
		}

		return result;
	}

	RestoreCommandDelegate::RestoreCommandDelegate(Application* app, unsigned long long steam_id)
		: ICommandDelegate(app), steam_id_(steam_id)
	{
	}

	CommandExecResultPtr RestoreCommandDelegate::Exec()
	{
		PlayerItemType player = app_->GetStorage()->Players()->Get(steam_id_);
		CommandExecResultPtr result = std::make_unique<CommandDelegateExecResult>();

		if (player) {
			TeamItemType team = app_->GetStorage()->Teams()->Get(player->GetTeamId());
			if (team) {
				app_->GetProtManager()->ResetAP(team);
				result->SetResult(true);
				result->SetOutput(GetText("ProtectionReset"));
			}
			else {
				result->SetResult(false);
				result->SetOutput(GetText("PlayerNotFound"));
			}
		}
		else {
			result->SetResult(false);
			result->SetOutput(GetText("PlayerNotFound"));
		}

		return result;
	}

	AddProtectionCommandDelegate::AddProtectionCommandDelegate(Application* app, unsigned long long steam_id, int seconds)
		: ICommandDelegate(app), steam_id_(steam_id), seconds_(seconds)
	{
	}

	CommandExecResultPtr AddProtectionCommandDelegate::Exec()
	{
		PlayerItemType player = app_->GetStorage()->Players()->Get(steam_id_);
		CommandExecResultPtr result = std::make_unique<CommandDelegateExecResult>();

		if (player) {
			TeamItemType team = app_->GetStorage()->Teams()->Get(player->GetTeamId());
			if (team) {
				app_->GetProtManager()->AddAPSeconds(team, seconds_);
				result->SetResult(true);
				result->SetOutput(L"Protection added");
			}
			else {
				result->SetResult(false);
				result->SetOutput(GetText("PlayerNotFound"));
			}
		}
		else {
			result->SetResult(false);
			result->SetOutput(GetText("PlayerNotFound"));
		}

		return result;
	}

	SetProtectionCommandDelegate::SetProtectionCommandDelegate(Application* app, unsigned long long steam_id, const std::string& cmd)
		: ICommandDelegate(app), steam_id_(steam_id), cmd_(cmd)
	{
	}

	CommandExecResultPtr SetProtectionCommandDelegate::Exec()
	{
		PlayerItemType player = app_->GetStorage()->Players()->Get(steam_id_);
		CommandExecResultPtr result = std::make_unique<CommandDelegateExecResult>();

		if (player) {
			TeamItemType team = app_->GetStorage()->Teams()->Get(player->GetTeamId());
			if (team) {
				TArray<FString> parsed;
				FString cmd(cmd_);

				cmd.ParseIntoArray(parsed, L" ");

				try {
					if (parsed.IsValidIndex(2)) {
						int counter = 2;
						int seconds = 0;

						std::regex days_regex("^([0-9]{1,3})d$");
						std::regex hours_regex("^([0-9]{1,2})h$");
						std::regex minutes_regex("^([0-9]{1,2})m$");
						std::cmatch match;

						while (counter < parsed.Num()) {
							if (std::regex_search(parsed[counter].ToString().c_str(), match, days_regex))
								seconds += std::stoi(match[1].str()) * 24 * 60 * 60;
							else if (std::regex_search(parsed[counter].ToString().c_str(), match, hours_regex))
								seconds += std::stoi(match[1].str()) * 60 * 60;
							else if (std::regex_search(parsed[counter].ToString().c_str(), match, minutes_regex))
								seconds += std::stoi(match[1].str()) * 60;

							counter++;
						}

						App->GetProtManager()->SetAPSeconds(team, seconds);

						result->SetResult(true);
						result->SetOutput(L"Protection added");
					}
					else {
						result->SetResult(false);
						result->SetOutput(L"Bad syntax");
					}
				}
				catch (const std::exception&) {
					result->SetResult(false);
					result->SetOutput(L"Bad syntax");
				}
			}
			else {
				result->SetResult(false);
				result->SetOutput(GetText("PlayerNotFound"));
			}
		}
		else {
			result->SetResult(false);
			result->SetOutput(GetText("PlayerNotFound"));
		}

		return result;
	}

	BlackListAddCommandDelegate::BlackListAddCommandDelegate(Application* app, int team_id)
		: ICommandDelegate(app), team_id_(team_id)
	{
	}

	CommandExecResultPtr BlackListAddCommandDelegate::Exec()
	{
		TeamItemType team = app_->GetStorage()->Teams()->Get(team_id_);
		CommandExecResultPtr result = std::make_unique<CommandDelegateExecResult>();

		if (!team) {
			result->SetResult(false);
			result->SetOutput(L"Team is not found");
		}
		else {
			app_->GetBlackList()->ForceAddTeam(team);
			result->SetResult(true);
			result->SetOutput(L"Team added to Black List");
		}

		return result;
	}

	BlackListRemoveCommandDelegate::BlackListRemoveCommandDelegate(Application* app, int team_id)
		: ICommandDelegate(app), team_id_(team_id)
	{
	}

	CommandExecResultPtr BlackListRemoveCommandDelegate::Exec()
	{
		TeamItemType team = app_->GetStorage()->Teams()->Get(team_id_);
		CommandExecResultPtr result = std::make_unique<CommandDelegateExecResult>();

		if (!team) {
			result->SetResult(false);
			result->SetOutput(L"Team is not found");
		}
		else {
			if (app_->GetBlackList()->ForceRemoveTeam(team)) {
				result->SetResult(true);
				result->SetOutput(L"Team removed to Black List");
			}
			else {
				result->SetResult(false);
				result->SetOutput(L"Team can't be removed");
			}
		}

		return result;
	}

	EnableORPCommandDelegate::EnableORPCommandDelegate(Application* app)
		: ICommandDelegate(app)
	{
	}

	CommandExecResultPtr EnableORPCommandDelegate::Exec()
	{
		CommandExecResultPtr result = std::make_unique<CommandDelegateExecResult>();

		app_->GetProtManager()->DisableNoORPMode();

		result->SetResult(true);
		result->SetOutput("ORP Enabled");

		return result;
	}

	DisableORPCommandDelegate::DisableORPCommandDelegate(Application* app, const std::string& cmd)
		: ICommandDelegate(app), cmd_(cmd)
	{
	}

	CommandExecResultPtr DisableORPCommandDelegate::Exec()
	{
		CommandExecResultPtr result = std::make_unique<CommandDelegateExecResult>();

		TArray<FString> parsed;
		FString cmd(cmd_);

		cmd.ParseIntoArray(parsed, L" ");

		try {
			if (parsed.IsValidIndex(1)) {
				int counter = 1;
				int seconds = 0;

				std::regex days_regex("^([0-9]{1,3})d$");
				std::regex hours_regex("^([0-9]{1,2})h$");
				std::regex minutes_regex("^([0-9]{1,2})m$");
				std::cmatch match;

				while (counter < parsed.Num()) {
					if (std::regex_search(parsed[counter].ToString().c_str(), match, days_regex))
						seconds += std::stoi(match[1].str()) * 24 * 60 * 60;
					else if (std::regex_search(parsed[counter].ToString().c_str(), match, hours_regex))
						seconds += std::stoi(match[1].str()) * 60 * 60;
					else if (std::regex_search(parsed[counter].ToString().c_str(), match, minutes_regex))
						seconds += std::stoi(match[1].str()) * 60;

					counter++;
				}

				if (seconds > 0)
				{
					app_->GetProtManager()->EnableNoORPMode(seconds);
				}

				result->SetResult(true);
				result->SetOutput(L"ORP Disabled");
			}
			else {
				result->SetResult(false);
				result->SetOutput(L"Bad syntax");
			}
		}
		catch (const std::exception&) {
			result->SetResult(false);
			result->SetOutput(L"Bad syntax");
		}

		return result;
	}

	ReloadCommandDelegate::ReloadCommandDelegate(Application* app)
		: ICommandDelegate(app)
	{
	}

	CommandExecResultPtr ReloadCommandDelegate::Exec()
	{
		CommandExecResultPtr result = std::make_unique<CommandDelegateExecResult>();

		if (app_->Reload()) {
			result->SetResult(true);
			result->SetOutput(L"Plugin is reloaded");
		}
		else {
			result->SetResult(false);
			result->SetOutput(L"Plugin isn't reloaded");
		}

		return result;
	}
}