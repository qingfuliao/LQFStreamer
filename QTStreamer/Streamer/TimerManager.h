#pragma once
#include <vector>
#include <functional>
#include <thread>

namespace LQF
{
	class TimerManager;

	class Timer
	{
	public:
		enum TimerType { ONCE, CIRCLE };

		Timer(TimerManager& manager);
		~Timer();

		//template<typename Fun>
		void Start(std::function<void(void *)> fun, void *user, unsigned interval, TimerType timeType = CIRCLE);
		void Stop();

	private:
		void OnTimer(unsigned long long now);

	private:
		friend class TimerManager;
		TimerManager& manager_;
		TimerType timerType_;
		std::function<void(void *)> timerFun_;
		void *user_;
		unsigned interval_;
		unsigned long long expires_;

		size_t heapIndex_;
	};

	class TimerManager
	{
	public:
		TimerManager(const int millisecs = 5);
		~TimerManager();
		bool Start();
		bool Stop();

		void DetectTimers();
		static uint64_t GetCurrentMillisecs();
		static TimerManager *GetInstance(const int milliseconds = 1);

	private:
		friend class Timer;
		void AddTimer(Timer* timer);
		void RemoveTimer(Timer* timer);

		void UpHeap(size_t index);
		void DownHeap(size_t index);
		void SwapHeap(size_t, size_t index2);

	private:
		struct HeapEntry
		{
			unsigned long long time;
			Timer* timer;
		};
		std::vector<HeapEntry> heap_;
		static TimerManager *timerManger_;
		std::shared_ptr<std::thread> timer_thread_ = nullptr;
		bool request_abort_ = false;
		int millisecs_ = 5;
	};

	// 用于时钟
	class AVTimer
	{
	public:
		AVTimer();
		void Reset();			// 重置起始时间
		uint64_t GetTime();		// 单位毫秒
		void SetAudioLatency(uint64_t audio_latency)
		{
			audio_latency_ = audio_latency;
		}
		void SetVideoLatency(uint64_t video_latency)
		{
			video_latency_ = video_latency;
		}

		uint64_t GetAudioLatency()
		{
			return audio_latency_;
		}
		uint64_t GetVideoLatency()
		{
			return video_latency_;
		}

		static AVTimer *GetInstance();
	private:
		uint64_t start_time_ = 0;
		uint64_t audio_latency_ = 0;
		uint64_t video_latency_ = 0;
		static AVTimer *av_timer_;
	};
}


