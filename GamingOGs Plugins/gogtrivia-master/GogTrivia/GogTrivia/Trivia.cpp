#include "Trivia.h"

namespace GogTrivia
{
    bool Trivia::Load()
    {
        try {
            for (const auto& question : Platform::Conf()["Questions"].items())
            {
                auto answers = question.value().get<std::vector<std::string>>();
                for (auto& answer : answers)
                {
                    std::transform(answer.begin(), answer.end(), answer.begin(),
                        [](unsigned char c) { return std::tolower(c); });
                }
                entries_.push_back(TriviaEntry{ question.key(), std::move(answers) });
            }
        } catch(const std::exception&) {
            return false;
        }

        active_entry_ = std::nullopt;
        question_time_ = 0;
        previous_answered_time_ = time(0) - 60;

        return true;
    }

    void Trivia::Update()
    {
        const int min_players = Platform::Conf()["PlayersRequired"].get<int>();
        auto controllers = ArkApi::GetApiUtils().GetWorld()->PlayerControllerListField();

        if (controllers.Num() < min_players)
        {
            return;
        }

        if (active_entry_.has_value())
        {
            const int answer_time = Platform::Conf()["AnswerTime"].get<int>() * 60;
            if (time(0) - question_time_ > answer_time)
            {
                Ask();
            }
        }
        else
        {
            Ask();
        }
    }

    void Trivia::Answer(AShooterPlayerController* controller, const std::string& answer)
    {
        if (!active_entry_.has_value())
        {
            return;
        }

        const auto it = std::find(active_entry_->answers.cbegin(), active_entry_->answers.cend(), answer);

        if (it != active_entry_->answers.cend())
        {
            RewardManager::Get().RewardPlayer(controller);
            active_entry_.reset();
            question_time_ = 0;
            previous_answered_time_ = time(0);
        }
        else
        {
            ArkApi::GetApiUtils().SendChatMessage(controller, Platform::Msg("Sender"), *Platform::Msg("WrongAnswer"));
        }
    }

    void Trivia::Ask()
    {
        const int questions_interval = Platform::Conf()["QuestionInterval"].get<int>() * 60;

        if (entries_.empty() || time(0) - previous_answered_time_ < questions_interval)
        {
            return;
        }

        const int rnd = Platform::RandNumber(0, static_cast<int>(entries_.size()) - 1);
        active_entry_.emplace(entries_[rnd].question, entries_[rnd].answers);
        question_time_ = time(0);

        ArkApi::GetApiUtils().SendChatMessageToAll(Platform::Msg("Sender"), active_entry_->question.c_str());
        ArkApi::GetApiUtils().SendChatMessageToAll(Platform::Msg("Sender"), *Platform::Msg("Help"), Platform::Cmd("Answer").ToString());
    }

    void CliAnswer(AShooterPlayerController* shooter_controller, FString* message, int mode)
	{
        const std::string c_msg = message->ToLower().ToString();
        size_t pos = c_msg.find(' ');
        if (pos == std::string::npos)
        {
            return;
        }
        const std::string answer = c_msg.substr(pos + 1, c_msg.length() - pos);

        Trivia::Get().Answer(shooter_controller, answer);
	}

    bool Load()
    {
        if (!Trivia::Get().Load())
        {
            return false;
        }

        RewardManager::Get().SetJSON(Platform::Conf()["Rewards"]);

        ArkApi::GetCommands().AddChatCommand(Platform::Cmd("Answer"), &CliAnswer);
        ArkApi::GetCommands().AddOnTimerCallback(L"Trivia_Timer", std::bind(&Trivia::Update, &Trivia::Get()));

        return true;
    }

    void Unload()
    {
        ArkApi::GetCommands().RemoveChatCommand(Platform::Cmd("Answer"));
        ArkApi::GetCommands().RemoveOnTimerCallback(L"Trivia_Timer");
    }
}