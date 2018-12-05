/**********************************************
*Date:  2018年9月21日
*Description: 简单视频播放器（只能播放视频）
**********************************************/

#include <iostream>
#ifdef _WIN32
#include <Winsock2.h>
#endif

#include "LogUtil.h"
using namespace std;

int InitSockets()
{
#ifdef _WIN32
	WORD version;
	WSADATA wsaData;
	version = MAKEWORD(2, 2);
	return (WSAStartup(version, &wsaData) == 0);
#endif
}

void CleanupSockets()
{
#ifdef _WIN32
	WSACleanup();
#endif
}


extern int simple_player_test(int argc, char *argv[]);
int RTP_PlayerTest(int argc, char *argv[]);
#undef main
int main(int argc, char *argv[])
{
	LogInit("rtp.txt");
	InitSockets();

	int ret = 0;
// 	if (simple_player_test(argc, argv) < 0)
// 	{
// 		printf("simple_player_test failed\n");
// 	}
	if (RTP_PlayerTest(argc, argv) < 0)
	{
		printf("RTP_PlayerTest failed\n");
	}
	LogDeinit();
	CleanupSockets();

	system("pause");
	
	return 0;
}


union test
{
	int a;
	char b : 1;
	char c : 7;	// 小端的时候，对于bit操作，高位仍然在高位
};

int endian_test(void)
{
	union test t1;
	t1.a = 1;
	return t1.c;
}

int main2(void)
{
	int i = endian_test();
	if (i == 1)
	{
		printf("is little endian.\n");
	}
	else
	{
		printf("is big endian.\n");
	}

	printf("i = %d.\n", i);
	system("pause");
	return 0;
}