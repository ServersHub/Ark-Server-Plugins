#ifndef _LOOTBOX_UTILS_H_
#define _LOOTBOX_UTILS_H_

#include <API/ARK/Ark.h>
#include <random>
#include "Configuration.h"

namespace LootBox
{

	class Message
	{
	public:
		Message(FString text, bool ok) : m_text(std::move(text)), m_ok(ok) {}
		Message(const std::string& text, bool ok) : m_text(text.c_str()), m_ok(ok) {}
		Message(const char* text, bool ok) : m_text(text), m_ok(ok) {}

		[[nodiscard]] const FString& Text() const noexcept { return m_text; }
		[[nodiscard]] FString& Text() noexcept { return m_text; }
		[[nodiscard]] bool IsOk() const noexcept { return m_ok; }

	private:
		FString m_text;
		bool m_ok;
	};

	inline void SendChatMessage(AShooterPlayerController* controller, Message msg)
	{
		ArkApi::GetApiUtils().SendChatMessage(controller, Configuration::Get().GetText("Sender"), *msg.Text());
	}

	inline void SendChatMessageToAll(Message msg)
	{
		ArkApi::GetApiUtils().SendChatMessageToAll(Configuration::Get().GetText("Sender"), *msg.Text());
	}

	template <class ... Args>
	inline void SendChatMessage(AShooterPlayerController* controller, const FString& msg, Args&& ... args)
	{
		ArkApi::GetApiUtils().SendChatMessage(controller, Configuration::Get().GetText("Sender"), *msg, std::forward<Args>(args)...);
	}

	template <class ... Args>
	inline void SendChatMessageToAll(const FString& msg, Args&& ... args)
	{
		ArkApi::GetApiUtils().SendChatMessageToAll(Configuration::Get().GetText("Sender"), *msg, std::forward<Args>(args)...);
	}

	inline void SendServerMessage(APlayerController* controller, Message msg)
	{
		if (msg.IsOk())
		{
			ArkApi::GetApiUtils().SendServerMessage(static_cast<AShooterPlayerController*>(controller), FColorList::Green, *msg.Text());
		}
		else
		{
			ArkApi::GetApiUtils().SendServerMessage(static_cast<AShooterPlayerController*>(controller), FColorList::Red, *msg.Text());
		}
	}

	inline void SendRconMessage(RCONClientConnection* conn, RCONPacket* packet, Message msg)
	{
		conn->SendMessageW(packet->Id, 0, &msg.Text());
	}

	template <class ... Types>
	class CommandParser
	{
		using TypesList = std::tuple<Types...>;
		template <int N>
		using ArgType = typename std::tuple_element<N, TypesList>::type;
		static constexpr int ArgsCount = sizeof...(Types);

	public:
		explicit CommandParser(const FString& command)
		{
			TArray<FString> parsed{};
			command.ParseIntoArray(parsed, L" ");

			if (parsed.Num() - 1 < ArgsCount)
			{
				throw std::runtime_error("Not enough arguments");
			}
			else if (parsed.Num() - 1 > ArgsCount)
			{
				throw std::runtime_error("Too many arguments");
			}

			for (int i = 1; i <= parsed.Num() - 1; i++)
			{
				m_elements.push_back(parsed[i].ToString());
			}
		}

		std::tuple<Types...> Parse()
		{
			return m_ParseImpl(std::make_index_sequence<ArgsCount>{});
		}

	private:
		template <std::size_t ... Is>
		std::tuple<Types...> m_ParseImpl(std::index_sequence<Is...>)
		{
			return std::make_tuple(m_ParseElement<Is>(m_elements[Is])...);
		}

		template <int N>
		ArgType<N> m_ParseElement(std::string s)
		{
			if constexpr (std::is_same_v<ArgType<N>, std::string>)
			{
				return s;
			}
			if constexpr (std::is_same_v<ArgType<N>, int>)
			{
				return std::stoi(s);
			}
			if constexpr (std::is_same_v<ArgType<N>, uint64>)
			{
				return std::stoull(s);
			}
		}

		std::vector<std::string> m_elements;
	};

	inline int RandFromRange(int min, int max)
	{
		if (min == max)
		{
			return min;
		}
		std::default_random_engine generator(std::random_device{}());
		const std::uniform_int_distribution<int> distribution(min, max);
		const int rnd = distribution(generator);
		return rnd;
	}

	inline bool RandWithProbability(int input)
	{
		if (input == 0)
		{
			return false;
		}
		if (input == 100)
		{
			return true;
		}

		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_int_distribution<int> dist(0, 99);
		return dist(mt) < input;
	}

	inline int RandWithProbability(std::vector<int>&& input)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::discrete_distribution<> d(input.begin(), input.end());
		return d(gen);
	}

}

#endif // !_LOOTBOX_UTILS_H_
