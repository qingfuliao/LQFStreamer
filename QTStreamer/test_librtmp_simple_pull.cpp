#include <stdio.h>
#include "librtmp/rtmp_sys.h"
#include "librtmp/log.h"
#include <stdlib.h>
#define FLV_NAME "hks1.flv"
#define RTMP_URL "rtmp://live.hkstv.hk.lxdns.com/live/hks1"
#define FLV_PULL_TIME  (10*1000)	// 录制时间

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#endif

#include <Windows.h>
/**
* @brief get_millisecond
* @return 返回毫秒
*/
int64_t get_current_time_msec()
{
#ifdef _WIN32
	return (int64_t)GetTickCount();
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return ((int64_t)tv.tv_sec * 1000 + (unsigned long long)tv.tv_usec / 1000);
#endif
}

static int InitSockets()
{
	WORD version;
	WSADATA wsaData;
	version = MAKEWORD(1, 1);
	return (WSAStartup(version, &wsaData) == 0);
}

static void CleanupSockets()
{
	WSACleanup();
}

int test_librtmp_pull(int argc, char* argv[])
{
	InitSockets();

	double duration = -1;
	int nRead;
	//is live stream ?
	bool bLiveStream = true;


	int bufsize = 1024 * 1024 * 2;
	char *buf = (char*)malloc(bufsize);
	memset(buf, 0, bufsize);
	long countbufsize = 0;

	FILE *fp = fopen(FLV_NAME, "wb");
	if (!fp) {
		RTMP_LogPrintf("Open File Error.\n");
		CleanupSockets();
		return -1;
	}

	/* set log level */
	RTMP_LogLevel loglvl= RTMP_LOGINFO;
	RTMP_LogSetLevel(loglvl);

	RTMP_Log(RTMP_LOGINFO, "RTMP_Alloc ---------->\n");
	RTMP *rtmp = RTMP_Alloc();
	RTMP_Log(RTMP_LOGINFO, "RTMP_Init ---------->\n");
	RTMP_Init(rtmp);
	//set connection timeout,default 30s
	rtmp->Link.timeout = 10;
	RTMP_Log(RTMP_LOGINFO, "RTMP_SetupURL ---------->\n");
	if (!RTMP_SetupURL(rtmp, RTMP_URL))
	{
		RTMP_Log(RTMP_LOGERROR, "SetupURL Err\n");
		RTMP_Free(rtmp);
		CleanupSockets();
		return -1;
	}
	if (bLiveStream) {
		rtmp->Link.lFlags |= RTMP_LF_LIVE;
	}

	RTMP_Log(RTMP_LOGINFO, "RTMP_SetBufferMS ---------->\n");
	//1hour
	RTMP_SetBufferMS(rtmp, 3600 * 1000);

	RTMP_Log(RTMP_LOGINFO, "RTMP_Connect ---------->\n");
	if (!RTMP_Connect(rtmp, NULL)) {
		RTMP_Log(RTMP_LOGERROR, "Connect Err\n");
		RTMP_Free(rtmp);
		CleanupSockets();
		return -1;
	}
	RTMP_Log(RTMP_LOGINFO, "RTMP_ConnectStream ---------->\n");
	if (!RTMP_ConnectStream(rtmp, 0)) {
		RTMP_Log(RTMP_LOGERROR, "ConnectStream Err\n");
		RTMP_Close(rtmp);
		RTMP_Free(rtmp);
		CleanupSockets();
		return -1;
	}
	
	int64_t start_time = get_current_time_msec();
	int64_t current_time = 0;
	int frame_count = 0;
	RTMP_Log(RTMP_LOGINFO, "RTMP_Read ---------->\n");
	while (nRead = RTMP_Read(rtmp, buf, bufsize)) 
	{
		fwrite(buf, 1, nRead, fp);

		countbufsize += nRead;
		frame_count++;
		
		if(frame_count % 100 == 0)
		{
			current_time = get_current_time_msec();
			RTMP_LogPrintf("T-[%lld]，count-[%d],Receive: %5dByte, Total: %5.2fkB\n", 
				current_time - start_time, frame_count,
				nRead, countbufsize*1.0 / 1024);
			if (current_time - start_time > FLV_PULL_TIME)
			{
				break;
			}
		}
	}

	if (fp)
		fclose(fp);

	if (buf) {
		free(buf);
	}

	if (rtmp) {
		RTMP_Log(RTMP_LOGINFO, "RTMP_Close ---------->\n");
		RTMP_Close(rtmp);
		RTMP_Free(rtmp);
		CleanupSockets();
		rtmp = NULL;
	}
	system("pause");
	return 0;
}
