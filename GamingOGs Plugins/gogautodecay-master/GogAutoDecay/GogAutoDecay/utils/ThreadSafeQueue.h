#pragma once

#include <queue>
#include <mutex>

namespace AutoDecay
{
	template <class _Ty>
	class ThreadSafeQueue
	{
	public:
		ThreadSafeQueue() = default;
		~ThreadSafeQueue() = default;

		ThreadSafeQueue(const ThreadSafeQueue& other) = delete;
		ThreadSafeQueue(ThreadSafeQueue&& other) = delete;

		ThreadSafeQueue& operator=(const ThreadSafeQueue& other) = delete;
		ThreadSafeQueue& operator=(ThreadSafeQueue&& other) = delete;

		bool IsEmpty() const
		{
			std::lock_guard<std::mutex> guard(mu_);
			return queue_.empty();
		}

		size_t Size() const
		{
			std::lock_guard<std::mutex> guard(mu_);
			return queue_.size();
		}

		void Push(const _Ty& item)
		{
			std::lock_guard<std::mutex> guard(mu_);
			queue_.push(item);
		}

		void Push(_Ty&& item)
		{
			std::lock_guard<std::mutex> guard(mu_);
			queue_.push(std::move(item));
		}

		_Ty Pop()
		{
			if (!queue_.empty())
			{
				_Ty top_item(std::move(queue_.front()));
				queue_.pop();
				return top_item;
			}

			return _Ty{};
		}

	private:
		std::queue<_Ty> queue_;
		mutable std::mutex mu_;
	};
}
