#pragma once

#ifdef __cplusplus
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "SDL.h"
#include "libavutil/imgutils.h"
#include "libavutil/time.h"
}
#endif

class PacketQueue; 
class FrameQueue;
class Decoder
{
public:
	Decoder();
	~Decoder();
	int decoder_init(AVCodecContext *avctx, PacketQueue *queue, SDL_cond *empty_queue_cond);
	int decoder_start(int(*fn)(void *), void *arg);
	void decoder_abort(FrameQueue *fq);
	void decoder_destroy(void);
public:

	AVPacket pkt_;
	PacketQueue	*queue_ = NULL;         // 数据包队列
	AVCodecContext	*avctx_ = NULL;     // 解码器上下文
	int		pkt_serial = -1;         // 包序列
	int		finished = 0;           // =0，解码器处于工作状态；=非0，解码器处于空闲状态
	int		packet_pending = 0;     // =0，解码器处于异常状态，需要考虑重置解码器；=1，解码器处于正常状态
	SDL_cond	*empty_queue_cond_ = NULL;  // 检查到packet队列空时发送 signal缓存read_thread读取数据
	int64_t		start_pts = 0;          // 初始化时是stream的start time
	AVRational	start_pts_tb;       // 初始化时是stream的time_base
	int64_t		next_pts = 0;           // 记录最近一次解码后的frame的pts，当解出来的部分帧没有有效的pts时则使用next_pts进行推算
	AVRational	next_pts_tb;        // next_pts的单位
	SDL_Thread	*decoder_tid = NULL;       // 线程句柄
};

