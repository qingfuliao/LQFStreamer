/**********************************************
*Date:  2018年9月21日
*Description: 简单视频播放器（只能播放视频）
**********************************************/

#include <iostream>

using namespace std;


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


bool g_quit = false;     // 当g_quit == true时程序退出
bool g_pause = false;    // 当g_pause == true时画面暂停
int  g_frame_rate = 25;    // 默认

const char *s_picture_type[] =
{
	"AV_PICTURE_TYPE_NONE", ///< Undefined
	"AV_PICTURE_TYPE_I",     ///< Intra
	"AV_PICTURE_TYPE_P",     ///< Predicted
	"AV_PICTURE_TYPE_B",     ///< Bi-dir predicted
	"AV_PICTURE_TYPE_S",     ///< S(GMC)-VOP MPEG-4
	"AV_PICTURE_TYPE_SI",    ///< Switching Intra
	"AV_PICTURE_TYPE_SP",    ///< Switching Predicted
	"AV_PICTURE_TYPE_BI"   ///< BI type
};

// 自定义SDL事件
#define FRAME_REFRESH_EVENT (SDL_USEREVENT+1)
static int frameRefreshThread(void *arg)
{
	while (!g_quit)
	{
		if (!g_pause)
		{
			SDL_Event event;
			event.type = FRAME_REFRESH_EVENT;
			SDL_PushEvent(&event);
		}
		if (g_frame_rate > 0)
			SDL_Delay(1000 / g_frame_rate);     // 这里控制播放速度，有时候因为调试所以使用了倍速。
		else
			SDL_Delay(40);
	}
	return 0;
}

static char err_buf[128] = { 0 };
static char* av_get_err(int errnum)
{
	av_strerror(errnum, err_buf, 128);
	return err_buf;
}

#undef main
int main(int argc, char *argv[])
{
	AVFormatContext *pFormatCtx = NULL;
	int             i, videoindex, audioindex, titleindex;
	AVCodecContext  *pCodecCtx;
	AVCodec         *pCodec;
	AVFrame         *pFrame;
	AVStream	    *avStream;
	AVPacket        *packet;
	int             ret;
	char filePath[256] = { 0 };
	bool isFileEnd = false; // 文件是否读取结束

							// SDL 这部分主要是显示相关，我们先不用关注
	int screen_w, screen_h = 0;
	SDL_Window   *window;
	SDL_Renderer *renderer;
	SDL_Texture  *sdlTexture;
	SDL_Rect     sdlRect;
	SDL_Event    event;
	SDL_Thread   *refreshThread;

	//strcpy(filePath, "axin.flv");
	//strcpy(filePath, "E:\\动脑科技\\课程资料\\C++公开课\\Martin 2018-04-08 Linux 服务器开发-你必须懂的Epoll玩法.mp4");

	strcpy(filePath, "source.200kbps.768x320.flv");
	// 分配解复用器的内存，使用avformat_close_input释放
	pFormatCtx = avformat_alloc_context();
	if (!pFormatCtx)
	{
		printf("[error] Could not allocate context.\n");
		return -1;
	}

	// 根据url打开码流，并选择匹配的解复用器
	ret = avformat_open_input(&pFormatCtx, filePath, NULL, NULL);
	if (ret != 0)
	{
		printf("[error]avformat_open_input: %s\n", av_get_err(ret));
		return -1;
	}

	// 读取媒体文件的部分数据包以获取码流信息
	ret = avformat_find_stream_info(pFormatCtx, NULL);
	if (ret < 0)
	{
		printf("[error]avformat_find_stream_info: %s\n", av_get_err(ret));

		avformat_close_input(&pFormatCtx);
		return -1;
	}

	// 查找出哪个码流是video/audio/subtitles
	videoindex = audioindex = titleindex = -1;
	//    for(i = 0; i < pFormatCtx->nb_streams; i++)   // 老版本的方式
	//    {
	//        avStream = pFormatCtx->streams[i];
	//        switch(avStream->codecpar->codec_type)
	//        {
	//        case AVMEDIA_TYPE_VIDEO:
	//            videoindex = i;
	//            // 计算帧率，以控制视频刷新间隔
	//            g_frame_rate = avStream->avg_frame_rate.num / avStream->avg_frame_rate.den;
	//            break;
	//        case AVMEDIA_TYPE_AUDIO:
	//            audioindex = i;
	//            break;
	//        case AVMEDIA_TYPE_SUBTITLE:
	//            titleindex = i;
	//            break;
	//        }
	//    }
	// 推荐的方式
	videoindex = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	if (videoindex == -1)
	{
		printf("Didn't find a video stream.\n");
		avformat_close_input(&pFormatCtx);
		return -1;
	}
	avStream = pFormatCtx->streams[videoindex];
	g_frame_rate = avStream->avg_frame_rate.num / avStream->avg_frame_rate.den;

	// 分配解码器上下文内存，使用avcodec_free_context来释放
	pCodecCtx = avcodec_alloc_context3(NULL);
	if (!pCodecCtx)
	{
		printf("[error]avcodec_alloc_context3() fail\n");
		avformat_close_input(&pFormatCtx);
		return -1;
	}
	// 将码流中的编解码器信息AVCodecParameters拷贝到AVCodecContex
	ret = avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoindex]->codecpar);
	if (ret < 0) {
		// printf("[error]avcodec_parameters_to_context: %s\n", av_err2str(ret));
		avcodec_free_context(&pCodecCtx);
		avformat_close_input(&pFormatCtx);
		return -1;
	}
	// 查找解码器
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL) {
		printf("Video Codec not found.\n");
		avcodec_free_context(&pCodecCtx);
		avformat_close_input(&pFormatCtx);
		return -1;
	}
	// 打开解码器
	ret = avcodec_open2(pCodecCtx, pCodec, NULL);
	if (ret < 0) {
		//printf("[error]avcodec_open2: %s\n", av_err2str(ret));
		avcodec_free_context(&pCodecCtx);
		avformat_close_input(&pFormatCtx);
		return -1;
	}

	// 输出文件信息
	printf("-------------File Information-------------\n");
	av_dump_format(pFormatCtx, 0, filePath, 0);
	printf("------------------------------------------\n");

	// 分配一个Frame
	pFrame = av_frame_alloc();   /* av_frame_free(&pFrame); */

								 // 初始SDL化界面, 先不用理会
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0)
	{
		printf("Could not initialize SDL - %s\n", SDL_GetError());
		av_frame_free(&pFrame);
		avcodec_free_context(&pCodecCtx);
		avformat_close_input(&pFormatCtx);
		return -1;
	}

	screen_w = pCodecCtx->width;
	screen_h = pCodecCtx->height;
	window = SDL_CreateWindow("Simple Player", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		screen_w, screen_h, SDL_WINDOW_RESIZABLE);  //SDL_WINDOW_OPENGL
	if (!window)
	{
		printf("SDL: could not create window - %s\n", SDL_GetError());

		av_frame_free(&pFrame);
		avcodec_free_context(&pCodecCtx);
		avformat_close_input(&pFormatCtx);
		return -1;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	sdlTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING,
		pCodecCtx->width, pCodecCtx->height);
	sdlRect.x = 0;
	sdlRect.y = 0;
	sdlRect.w = screen_w;
	sdlRect.h = screen_h;
	// 初始SDL化界面结束

	// 分配数据包
	packet = av_packet_alloc();
	av_init_packet(packet);

	// 创建一个刷新线程，刷新线程定义发送刷新信号给主线程，触发主线程解码和显示画面
	refreshThread = SDL_CreateThread(frameRefreshThread, NULL, NULL);

	while (!g_quit)      // 主循环
	{
		if (SDL_WaitEvent(&event) != 1)
			continue;

		switch (event.type)
		{
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE)
				g_quit = true;
			if (event.key.keysym.sym == SDLK_SPACE)
				g_pause = !g_pause;
			break;

		case SDL_QUIT:    /* Window is closed */
			g_quit = true;
			break;

		case FRAME_REFRESH_EVENT:
		read_packet_again:                  // 如果读取到的是非video packet，则重新读取
			if ((ret = av_read_frame(pFormatCtx, packet)) < 0)
			{
				// 没有更多包可读
				isFileEnd = true;
				printf("read file end\n");
			}

			if (ret == 0 && packet->stream_index != videoindex)
			{
				av_packet_unref(packet);        // 释放内存
				goto read_packet_again;
			}

			if (ret == 0)
			{
				// 发送要解码的数据
				ret = avcodec_send_packet(pCodecCtx, packet);
				if (ret != 0)
				{
					av_packet_unref(packet);
					break;
				}
			}
			else
			{
				// 刷空包进去
				printf("flush packet\n");
				av_packet_unref(packet);        // 如果还占用内存则释放
				ret = avcodec_send_packet(pCodecCtx, packet);
			}

			// 获取解码后的帧
			do
			{
				/*     0:                 success, a frame was returned
				*      AVERROR(EAGAIN):   output is not available in this state - user must try
				*                         to send new input
				*      AVERROR_EOF:       the decoder has been fully flushed, and there will be
				*                         no more output frames
				*      AVERROR(EINVAL):   codec not opened, or it is an encoder
				*      other negative values: legitimate decoding errors
				*/
				ret = avcodec_receive_frame(pCodecCtx, pFrame);
				if (ret == 0)
				{  // 成功获取到解码后的帧
					printf("frame type = %s\n", s_picture_type[pFrame->pict_type]);
					SDL_UpdateYUVTexture(sdlTexture, &sdlRect,
						pFrame->data[0], pFrame->linesize[0],
						pFrame->data[1], pFrame->linesize[1],
						pFrame->data[2], pFrame->linesize[2]);
					SDL_RenderClear(renderer);
					SDL_RenderCopy(renderer, sdlTexture, NULL, &sdlRect);
					SDL_RenderPresent(renderer);

					// 测试内存泄漏
					//                    AVFrame *localFrame = av_frame_alloc();
					//                    av_frame_move_ref(localFrame, pFrame);
				}
				else if (ret == AVERROR(EAGAIN))
				{
					// 没有帧可读，等待下一次输入后再读取
					break;
				}
				else if (ret == AVERROR_EOF)
				{
					// 解码器所有帧都已经被读取
					avcodec_flush_buffers(pCodecCtx);// YUV 1920*1080*1.5*5个buffer
					printf("avcodec_flush_buffers\n");
					g_quit = true;          // 退出播放
					break;
				}
				else if (ret < 0)
				{
					printf("if(ret < 0)\n");
					break;
				}
			} while (ret != AVERROR(EAGAIN));
			if (packet->buf)        // 打印referenc-counted，必须保证传入的是有效指针
				printf("ref_count(packet) = %d\n", av_buffer_get_ref_count(packet->buf));
			av_packet_unref(packet);        // 释放内存


			break;
		default:
			//printf("unknow sdl event.......... event.type = %x\n", event.type);
			break;
		} /* End of switch */
	} /* End of while(!g_quit) */


	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	av_packet_free(&packet);
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	avcodec_free_context(&pCodecCtx);
	avformat_close_input(&pFormatCtx);

	printf("程序结束\n");

	//system("pause");
	return 0;
}
