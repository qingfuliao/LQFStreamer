#include "LogUtil.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mutex>
#include <windows.h>
static FILE *fFile = NULL;
const int kLogFileSize = 1024 * 1024;
std::mutex log_mutex_;

long getfilesize(FILE *pFile)
{
	// check FILE*.
	if (pFile == NULL)
	{
		return -1;
	}

	// get current file pointer offset.
	long cur_pos = ftell(pFile);
	if (cur_pos == -1)
	{
		return -1;
	}

	// fseek to the position of file end.
	if (fseek(pFile, 0L, SEEK_END) == -1)
	{
		return -1;
	}

	// get file size.
	long size = ftell(pFile);

	// fseek back to previous pos.
	if (fseek(pFile, cur_pos, SEEK_SET) == -1)
	{
		return -1;
	}

	// deal returns.
	return size;
}

void LogWrite(char *str, int size)
{
	static unsigned int count = 0;
	if (fFile)
	{
		std::unique_lock <std::mutex> lck(log_mutex_);
		if (getfilesize(fFile) > kLogFileSize)//only about 2m memory is free while app is running, for better performance, log file size needs to be stricted to be 512K.(old setting is 5M)
		{
			fseek(fFile, 0, SEEK_SET);
		}
		fwrite(str, 1, size, fFile);
		if((count %10) == 0)		// 每十条刷一次，避免频繁刷新
			fflush(fFile);			// 刷新log
	}
}

void LogInit(const char *logfile)
{
	fFile = fopen(logfile, "w+");
	if (fFile == NULL)
	{
		return;
	}
}

void LogDeinit()
{
	if (fFile )
	{
		fclose(fFile);
	}
}

#include <ctime>

void suck(const char* fmt, ...)
{

	char buf[1024] = { 0 };
	char timeBuf[1024] = { 0 };
	/*
	timeval now;
	gettimeofday(&now, NULL);
	const time_t timeseconds = now.tv_sec;
	strftime(timeBuf, 128, "%m-%d %H:%M:%S ", localtime(&timeseconds));
	long int timemillseconds = now.tv_usec / 1000;
	sprintf(timeBuf + strlen(timeBuf), "%ld ", timemillseconds);*/



	SYSTEMTIME lpsystime;
	GetLocalTime(&lpsystime);
	sprintf(timeBuf, "%04u-%02u-%02u %u:%02u:%02u - [%003u] ",
		lpsystime.wYear, lpsystime.wMonth, lpsystime.wDay, lpsystime.wHour, lpsystime.wMinute, lpsystime.wSecond, 
		lpsystime.wMilliseconds);

	std::string fmt_str(timeBuf);
	fmt_str += fmt;
	fmt_str += " \n";


	va_list arglist;
	va_start(arglist, fmt);
	//vfprintf(stderr, fmt_str.c_str(), arglist);
	vsnprintf(buf, sizeof(buf), fmt_str.c_str(), arglist);
	fprintf(stderr, buf);
	//printf("%s", buf);
	LogWrite(buf, strlen(buf));
	va_end(arglist);
}
