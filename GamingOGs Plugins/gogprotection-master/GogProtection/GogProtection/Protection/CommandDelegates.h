#ifndef _GOG_PROT_COMMAND_DELEGATES_H_
#define _GOG_PROT_COMMAND_DELEGATES_H_

#include <API/UE/Containers/FString.h>
#include <API/UE/Math/Vector.h>
#include <memory>

namespace Protection
{
    class Application;

    class CommandDelegateExecResult
    {
    public:
        CommandDelegateExecResult()
            : result_(false), output_(L"")
        {  
        }

        CommandDelegateExecResult(bool result, const FString& output)
            : result_(result), output_(output)
        {
        }

        ~CommandDelegateExecResult()
        {
        }

        inline bool GetResult() const { return result_; }
        inline FString GetOutput() const { return output_; }

        inline void SetResult(bool result) { result_ = result; }
        inline void SetOutput(const FString& output) { output_ = output; }

    private:
        bool result_;
        FString output_;
    };

	using CommandExecResultPtr = std::unique_ptr<CommandDelegateExecResult>;

    class ICommandDelegate
    {
    public:
        ICommandDelegate(Application* app)
            : app_(app)
        {
        }

        virtual ~ICommandDelegate()
        {
        }

        virtual CommandExecResultPtr Exec() = 0;

    protected:
        Application* app_;
    };

    class CliInfoCommandDelegate : public ICommandDelegate
    {
    public:
        CliInfoCommandDelegate(Application* app);
        CommandExecResultPtr Exec() override final;
    };

    class CliStatusCommandDelegate : public ICommandDelegate
    {
    public:
        CliStatusCommandDelegate(Application* app, int team_id);
        CommandExecResultPtr Exec() override final;

    private:
        int team_id_;
    };

    class CliDisableCommandDelegate : public ICommandDelegate
    {
    public:
        CliDisableCommandDelegate(Application* app, int team_id, bool invoked_by_team_admin);
        CommandExecResultPtr Exec() override final;

    private:
        int team_id_;
        bool invoked_by_team_admin_;
    };

	class CliSetZoneCommandDelegate : public ICommandDelegate
	{
	public:
		CliSetZoneCommandDelegate(Application* app, int team_id, const std::string& id, const FVector& position);
		CommandExecResultPtr Exec() override final;

	private:
		int team_id_;
		std::string id_;
		FVector position_;
	};

	class CliUnsetZoneCommandDelegate : public ICommandDelegate
	{
	public:
		CliUnsetZoneCommandDelegate(Application* app, int team_id, const std::string& id);
		CommandExecResultPtr Exec() override final;

	private:
		int team_id_;
		std::string id_;
	};

	class CliDisplayZonesCommandDelegate : public ICommandDelegate
	{
	public:
		CliDisplayZonesCommandDelegate(Application* app, int team_id, const FVector& player_position);
		CommandExecResultPtr Exec() override final;

	private:
		int team_id_;
		FVector player_position_;
	};

	class DisableCommandDelegate : public ICommandDelegate
	{
	public:
		DisableCommandDelegate(Application* app, unsigned long long steam_id);
		CommandExecResultPtr Exec() override final;

	private:
		unsigned long long steam_id_;
	};

	class RestoreCommandDelegate : public ICommandDelegate
	{
	public:
		RestoreCommandDelegate(Application* app, unsigned long long steam_id);
		CommandExecResultPtr Exec() override final;

	private:
		unsigned long long steam_id_;
	};

	class AddProtectionCommandDelegate : public ICommandDelegate
	{
	public:
		AddProtectionCommandDelegate(Application* app, unsigned long long steam_id, int seconds);
		CommandExecResultPtr Exec() override final;

	private:
		unsigned long long steam_id_;
		int seconds_;
	};

	class SetProtectionCommandDelegate : public ICommandDelegate
	{
	public:
		SetProtectionCommandDelegate(Application* app, unsigned long long steam_id, const std::string& cmd);
		CommandExecResultPtr Exec() override final;

	private:
		unsigned long long steam_id_;
		std::string cmd_;
	};

	class BlackListAddCommandDelegate : public ICommandDelegate
	{
	public:
		BlackListAddCommandDelegate(Application* app, int team_id);
		CommandExecResultPtr Exec() override final;

	private:
		int team_id_;
	};

	class BlackListRemoveCommandDelegate : public ICommandDelegate
	{
	public:
		BlackListRemoveCommandDelegate(Application* app, int team_id);
		CommandExecResultPtr Exec() override final;

	private:
		int team_id_;
	};

	class EnableORPCommandDelegate : public ICommandDelegate
	{
	public:
		EnableORPCommandDelegate(Application* app);
		CommandExecResultPtr Exec() override final;
	};

	class DisableORPCommandDelegate : public ICommandDelegate
	{
	public:
		DisableORPCommandDelegate(Application* app, const std::string& cmd);
		CommandExecResultPtr Exec() override final;

	private:
		std::string cmd_;
	};

	class ReloadCommandDelegate : public ICommandDelegate
	{
	public:
		ReloadCommandDelegate(Application* app);
		CommandExecResultPtr Exec() override final;
	};
}


#endif // !_GOG_PROT_COMMAND_DELEGATES_H_