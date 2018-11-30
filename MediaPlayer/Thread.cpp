#include "Thread.h"

#include "LogUtil.h"

Thread::Thread():
	thread_(nullptr)
{
}


Thread::~Thread()
{
}

bool Thread::Start()
{
	if (!thread_)
	{
		thread_ = new thread(&Thread::Run, this);
	}
	return true;
}

bool Thread::Stop()
{
	if (thread_ != nullptr)
	{
		requet_abort_ = true;
		thread_->join();			// 等等线程退出
		delete thread_;
		thread_ = nullptr;
	}
	return true;
}

bool Thread::IsRunning()
{
	return running_;
}

void Thread::Run(void)
{
	running_ = true;
	while (true)
	{
		Sleep(1000);
		LogDebug("Thread::Run");
		if (requet_abort_)
		{
			break;
		}
	}
	running_ = false;
}
