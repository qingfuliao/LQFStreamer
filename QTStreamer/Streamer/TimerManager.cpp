#define _CRT_SECURE_NO_WARNINGS
#include "TimerManager.h"
#ifdef _MSC_VER
# include <sys/timeb.h>
#else
# include <sys/time.h>
#endif
namespace LQF
{
	TimerManager *TimerManager::timerManger_ = nullptr;
	//////////////////////////////////////////////////////////////////////////
	// Timer

	Timer::Timer(TimerManager& manager)
		: manager_(manager)
		, heapIndex_(-1)
	{
	}

	Timer::~Timer()
	{
		Stop();
	}

	//template<typename Fun>
	void Timer::Start(std::function<void(void *)> fun, void *user, unsigned interval, TimerType timeType)
	{
		Stop();
		interval_ = interval;
		timerFun_ = fun;
		user_ = user;
		timerType_ = timeType;
		expires_ = interval_ + TimerManager::GetCurrentMillisecs();
		manager_.AddTimer(this);
	}

	void Timer::Stop()
	{
		if (heapIndex_ != -1)
		{
			manager_.RemoveTimer(this);
			heapIndex_ = -1;
		}
	}

	void Timer::OnTimer(unsigned long long now)
	{
		if (timerType_ == Timer::CIRCLE)
		{
			expires_ = interval_ + now;
			manager_.AddTimer(this);
		}
		else
		{
			heapIndex_ = -1;
		}
		timerFun_(user_);
	}

	//////////////////////////////////////////////////////////////////////////
	// TimerManager

	void TimerManager::AddTimer(Timer* timer)
	{
		timer->heapIndex_ = heap_.size();
		HeapEntry entry = { timer->expires_, timer };
		heap_.push_back(entry);
		UpHeap(heap_.size() - 1);
	}

	void TimerManager::RemoveTimer(Timer* timer)
	{
		size_t index = timer->heapIndex_;
		if (!heap_.empty() && index < heap_.size())
		{
			if (index == heap_.size() - 1)
			{
				heap_.pop_back();
			}
			else
			{
				SwapHeap(index, heap_.size() - 1);
				heap_.pop_back();
				size_t parent = (index - 1) / 2;
				if (index > 0 && heap_[index].time < heap_[parent].time)
					UpHeap(index);
				else
					DownHeap(index);
			}
		}
	}

	TimerManager::TimerManager(const int millisecs) :
		millisecs_(millisecs)
	{

	}

	TimerManager::~TimerManager()
	{
		Stop();
	}

	bool TimerManager::Start()
	{
		if(!timer_thread_)
			timer_thread_.reset(new std::thread(&TimerManager::DetectTimers, this));

		return true;
	}

	bool TimerManager::Stop()
	{
		request_abort_ = true;

		if (timer_thread_)
		{
			timer_thread_->join();
			timer_thread_.reset();
		}

		return true;
	}

	void TimerManager::DetectTimers()
	{
		while (true)
		{
			if (request_abort_)
			{
				break;
			}
			unsigned long long now = GetCurrentMillisecs();

			while (!heap_.empty() && (heap_[0].time) <= (now))
			{
				Timer* timer = heap_[0].timer;
				RemoveTimer(timer);
				timer->OnTimer(now);
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(millisecs_));
		}
		request_abort_ = false;
	}

	void TimerManager::UpHeap(size_t index)
	{
		size_t parent = (index - 1) / 2;
		while (index > 0 && heap_[index].time < heap_[parent].time)
		{
			SwapHeap(index, parent);
			index = parent;
			parent = (index - 1) / 2;
		}
	}

	void TimerManager::DownHeap(size_t index)
	{
		size_t child = index * 2 + 1;
		while (child < heap_.size())
		{
			size_t minChild = (child + 1 == heap_.size() || heap_[child].time < heap_[child + 1].time)
				? child : child + 1;
			if (heap_[index].time < heap_[minChild].time)
				break;
			SwapHeap(index, minChild);
			index = minChild;
			child = index * 2 + 1;
		}
	}

	void TimerManager::SwapHeap(size_t index1, size_t index2)
	{
		HeapEntry tmp = heap_[index1];
		heap_[index1] = heap_[index2];
		heap_[index2] = tmp;
		heap_[index1].timer->heapIndex_ = index1;
		heap_[index2].timer->heapIndex_ = index2;
	}


	uint64_t TimerManager::GetCurrentMillisecs()
	{
#ifdef _MSC_VER
		_timeb timebuffer;
		_ftime(&timebuffer);
		unsigned long long ret = timebuffer.time;
		ret = ret * 1000 + timebuffer.millitm;
		return ret;
#else
		timeval tv;
		::gettimeofday(&tv, 0);
		unsigned long long ret = tv.tv_sec;
		return ret * 1000 + tv.tv_usec / 1000;
#endif
	}

	TimerManager * TimerManager::GetInstance(const int milliseconds)
	{
		if (!timerManger_)
		{
			timerManger_ = new TimerManager(5);		//精度
		}
		return timerManger_;
	}


	AVTimer *AVTimer::av_timer_ = nullptr;
	AVTimer::AVTimer()
	{
		Reset();
	}

	void AVTimer::Reset()
	{
		start_time_ = TimerManager::GetCurrentMillisecs();
	}

	uint64_t AVTimer::GetTime()
	{
		return (TimerManager::GetCurrentMillisecs() - start_time_);
	}
	AVTimer * AVTimer::GetInstance()
	{
		if (!av_timer_)
		{
			av_timer_ = new AVTimer();		//精度
		}
		return av_timer_;
	}
}

