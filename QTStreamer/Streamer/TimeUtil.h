#pragma once
#include <cstdint>

#include <windows.h>
class TimeUtil
{
public:
	TimeUtil();
	static int64_t GetNowTime()
	{
		return  (int64_t)GetTickCount();
	}
	virtual ~TimeUtil();
};

