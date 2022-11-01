#pragma once

#include <thread>
#include <atomic>


namespace AutoDecay
{
	class PeriodicTimer
	{
	public:
		PeriodicTimer(__int64 interval, std::function<void()>&& handler)
			: interval_(interval),
			  handler_(std::move(handler)),
			  expires_at_(0),
			  cancel_req_(false)
		{
			worker_thread_ = std::make_unique<std::thread>(std::bind(&PeriodicTimer::Counter, this));
		}

		~PeriodicTimer()
		{
			cancel_req_ = true;

			if (worker_thread_ && worker_thread_->joinable())
			{
				worker_thread_->join();
			}
		}

	private:
		void Counter()
		{
			while (!cancel_req_)
			{
				expires_at_ = time(0) + interval_;

				while (time(0) < expires_at_)
				{
					if (cancel_req_)
					{
						break;
					}

					std::this_thread::sleep_for(std::chrono::seconds(1));
				}

				if (!cancel_req_)
				{
					handler_();
				}
			}
		}

		std::unique_ptr<std::thread> worker_thread_;
		std::function<void()> handler_;
		std::atomic<bool> cancel_req_;
		__int64 interval_;
		time_t expires_at_;
	};
}