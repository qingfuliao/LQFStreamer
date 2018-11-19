#include <iostream>
#ifdef _WIN32
#include <Winsock2.h>
#endif


#include "MediaCenter.h"
#include "AACEncoder.h"
#include "X264Encoder.h"
#include "LogUtil.h"
#include "TimerManager.h"

#define RTMP_URL "rtmp://192.168.100.36/live/35"

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

int main_rtmp()
{
	LogInit("rtmp.txt");


	MediaCenter media_center;
	
	AudioEncoderConfig audio_config;
	audio_config.bitrate = 128000;
	audio_config.channels = 2;
	audio_config.sample_rate = 44100;
	audio_config.profile = PROFILE_AAC_LC;
	media_center.ConfigAudioEncoder(audio_config);

	int preset = H264_PRESET_VERYFAST;
	VideoEncoderConfig encode_config;
	encode_config.bitrate = 3000;
	encode_config.fps = 16;
	encode_config.gop = encode_config.fps*1;
	encode_config.width = 1920;
	encode_config.height = 1080;
	encode_config.preset = preset;
	encode_config.qp = 26;
	encode_config.use_qp = 0;			// 平均码率
	encode_config.slice_max_size = 500000;		// 
	encode_config.bframes = 0;
	encode_config.b_annexb = 1;
	encode_config.b_repeat_headers = 0;			// 不repeat sps pps sei
	encode_config.profile = H264_PROFILE_BASELINE;
	media_center.ConfigVideoEncoder(encode_config);
	media_center.InitAVTimer();
	media_center.StartRtmpPush(RTMP_URL, 5);
	media_center.StartRecord("record.mp4");
	if (!media_center.StartAudio())
	{
		LogError("StartAudio failed");
	}
	if (!media_center.StartVideo())
	{
		LogError("StartVideo failed");
	}

	TimerManager::GetInstance()->Start();	// 启动定时器

	int count = 0;
	while(true)
	{ 
		Sleep(1000 * 5);
		if (LQF::AVTimer::GetInstance()->GetTime() > 10*1000)
		{
			break;
		}
		printf("time = %llu秒\n", LQF::AVTimer::GetInstance()->GetTime() / 1000);
	}
	//
	LogDebug("StopAudio");
	media_center.StopAudio();
	LogDebug("StopVideo");
	media_center.StopVideo();
	LogDebug("StopRtmpPush");
	media_center.StopRtmpPush();
	LogDebug("StopRecord");
	media_center.StopRecord();
	LogDebug("TimerManager::GetInstance()->Stop();");
	TimerManager::GetInstance()->Stop();
	LogDebug("LogDeinit");
	LogDeinit();
	return 0;
}


void TimerHandler(void *user)
{
	std::cout << "TimerHandler" << std::endl;
}
int main_timer_test()
{
	Timer t(*TimerManager::GetInstance());
	t.Start(&TimerHandler, NULL, 500);
	TimerManager::GetInstance()->Start();

		
	std::this_thread::sleep_for(std::chrono::milliseconds(15000));
	TimerManager::GetInstance()->Stop();

	std::cout << "main_timer_test pass" << std::endl;
	return 0;
}

int main(void)
{
	InitSockets();

	std::cout << "rtmp publisher" << std::endl;
	main_rtmp();
	//main_timer_test();
	CleanupSockets();
	system("pause");
	return 0;
}