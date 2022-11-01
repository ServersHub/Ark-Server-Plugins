#pragma once

#include "Platform.h"
#include "Rewards.h"

#include <vector>
#include <optional>

namespace GogTrivia
{
    class Trivia
    {
        struct TriviaEntry
        {
            TriviaEntry() : question("")
            {
            }
            
            TriviaEntry(std::string q, std::vector<std::string> a)
                : question(std::move(q)), answers(std::move(a))
            {
            }

            std::string question;
            std::vector<std::string> answers;
        };

    public:
        Trivia() = default;
        ~Trivia() = default;

        static Trivia& Get()
        {
            static Trivia instance;
            return instance;
        }

        bool Load();
        void Update();
        void Answer(AShooterPlayerController* controller, const std::string& answer);

    private:
        void Ask();

        std::vector<TriviaEntry> entries_;
        std::optional<TriviaEntry> active_entry_;
        time_t question_time_;
        time_t previous_answered_time_;
    };

    bool Load();

    void Unload();
}