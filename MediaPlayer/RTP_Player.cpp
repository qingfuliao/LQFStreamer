/*
* 模块划分
* （1）RTP数据接收；
* （2）RTP解析包解析，将数据包插入队列
* （3）SPS/PPS解析，主要是解出宽高以设置SDL显示大小
* （4）FFMPEG解码，从数据包队列读取
* 
* 总体思路，使用ffplay之前划分的数据队列，C/C++代码混合
* （1）先能够正常接收H264数据，解析出来每一个完整的数据包在插入队列的同时也保存到本地。
*/

#ifdef __cplusplus
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "SDL.h"
#include "libavutil/imgutils.h"
}
#endif

#include "RTPReceiver.h"
#include "LogUtil.h"
#include "SDL2Display.h"

// 可以固定播放器的大小，以方便调试
#define  PLAYER_WIDTH  (1920*0.7)
#define  PLAYER_HEIGHT  (1080*0.7)

bool g_quit = false;
// 自定义SDL事件
#define FRAME_REFRESH_EVENT (SDL_USEREVENT+1)
static int frameRefreshThread(void *arg)
{
	while (!g_quit)
	{
		SDL_Event event;
		event.type = FRAME_REFRESH_EVENT;
		SDL_PushEvent(&event);
		SDL_Delay(20);	// 20ms触发一次读取帧
	}
	return 0;
}


int RTP_PlayerTest(int argc, char *argv[])
{
	RTPReceiver rtp_receiver;
	RTP_CONNECT_PARAM_T connect_param;
	connect_param.enable_rtp_recv = true;		// 允许接收
	connect_param.listen_ip[0] = 127;			// 回环测试
	connect_param.listen_ip[1] = 0;
	connect_param.listen_ip[2] = 0;
	connect_param.listen_ip[3] = 1;
	connect_param.listen_port = 9000;
	connect_param.timestamp_unit = 1000;
	connect_param.payload_type = RTP_PAYLOAD_TYPE_H264;

	connect_param.enable_rtp_send = true;		// 
	connect_param.dest_ip[0] = 127;			// 回环测试
	connect_param.dest_ip[1] = 0;
	connect_param.dest_ip[2] = 0;
	connect_param.dest_ip[3] = 1;
	connect_param.dest_port = 8000;
	if (!rtp_receiver.Init(connect_param))
	{
		LogError("RTPReceiver init failed");
		return -1;
	}
	rtp_receiver.Start();

	SDL2Display sdl2_display;
	

	AVCodecID codec_id = AV_CODEC_ID_H264;
	LogDebug("avcodec_find_decoder");
	AVCodec* pCodec = avcodec_find_decoder(codec_id);
	if (!pCodec)
	{
		LogError("avcodec_find_decoder failed");
		return -1;
	}
	LogDebug("avcodec_alloc_context3");
	AVCodecContext *pCodecCtx = avcodec_alloc_context3(pCodec);
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		LogError("avcodec_open2 failed");
		return -1;
	}
	pCodecCtx->width = 1920;
	pCodecCtx->height = 1080;
	pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;

	// 分配一个Frame
	AVFrame *pFrame = av_frame_alloc();   /* av_frame_free(&pFrame); */
	AVPacket *packet = av_packet_alloc();
	av_init_packet(packet);
	int ret = 0;

	int is_display_open = 0;
	LogDebug("wait rtp packet.........");

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0)
	{
		LogError("Could not initialize SDL - %s\n", SDL_GetError());
		return -1;
	}
	SDL_Event    event;
	

	// 创建一个刷新线程，刷新线程定义发送刷新信号给主线程，触发主线程解码和显示画面
	SDL_Thread   *refreshThread = SDL_CreateThread(frameRefreshThread, NULL, NULL);
	while (true)      // 主循环
	{
		if (SDL_WaitEvent(&event) != 1)
			continue;
		if (g_quit)
		{
			break;
		}
		switch (event.type)
		{
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE)
				g_quit = true;
			break;

		case SDL_QUIT:    /* Window is closed */
			g_quit = true;
			break;
		case FRAME_REFRESH_EVENT:
			send_packet_again:
			LQF::AVPacket rtp_pkt;
			if (rtp_receiver.PopPacket(rtp_pkt))
			{
				// 取帧成功，重新封装成ffmpeg AVPacket进行解码
				packet->data = (uint8_t*)rtp_pkt.buffer.get();
				packet->size = rtp_pkt.size;
				packet->pts = rtp_pkt.timestamp;
				ret = avcodec_send_packet(pCodecCtx, packet);
				if (ret == AVERROR_EOF)
				{
					LogError("avcodec_send_packet failed, ret = %d", ret);
				}
				else if(ret == EAGAIN)
				{
					goto send_packet_again;
				}
				ret = avcodec_receive_frame(pCodecCtx, pFrame);
				if (ret == 0)
				{  // 成功获取到解码后的帧
				   // 直接显示出来
					if (!is_display_open)
					{
						if (sdl2_display.display_init(PLAYER_WIDTH, PLAYER_HEIGHT) < 0)
						{
							LogError("display_init failed");
							rtp_receiver.Stop();
							return -1;
						}
						if (!(sdl2_display.display_open() < 0))
						{
							is_display_open = true;
						}
					}
					if (is_display_open)
						sdl2_display.display_frame(pFrame);      // 显示画面
					else
						LogError("sdl2 display open failed");
				}
				else if (ret == AVERROR(EAGAIN))
				{
					// 没有帧可读，等待下一次输入后再读取
					continue;
				}
			}
			break;
		}
		
		Sleep(10);
	}
	// 清理工作
	rtp_receiver.Stop();
	sdl2_display.display_deinit();
	SDL_Quit();
	av_packet_free(&packet);
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	avcodec_free_context(&pCodecCtx);

	return 0;
}