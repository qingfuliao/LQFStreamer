#pragma once
#include <thread>
#include <Windows.h>
using namespace std;

class Thread
{
public:
	Thread();
	virtual ~Thread();
	virtual bool Start();		// 启动线程
	virtual bool Stop();		// 停止线程
	virtual bool IsRunning();		// 是否处于运行状态
	
	virtual void Run(void);
	protected:
	thread *thread_ = nullptr;
	bool running_ = false;	// 线程是否处于运行态
	bool requet_abort_ = false;

};

