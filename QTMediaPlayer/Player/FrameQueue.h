#ifndef FRAME_QUEUE_H
#define FRAME_QUEUE_H

#ifdef __cplusplus
extern "C"
{
#include "libavformat/avformat.h"
#include "SDL.h"
}
#endif

#define VIDEO_PICTURE_QUEUE_SIZE	3       // 图像帧缓存数量
#define SUBPICTURE_QUEUE_SIZE		16      // 字幕帧缓存数量
#define SAMPLE_QUEUE_SIZE           9       // 采样帧缓存数量
#define FRAME_QUEUE_SIZE		FFMAX( SAMPLE_QUEUE_SIZE, FFMAX( VIDEO_PICTURE_QUEUE_SIZE, SUBPICTURE_QUEUE_SIZE ) )

typedef struct Frame
{
	AVFrame		*frame;         // 指向数据帧
	AVSubtitle	sub;            // 用于字幕
	int		serial;             // 帧序列，在seek的操作时serial会变化
	double		pts;            // 时间戳，单位为秒
	double		duration;       // 该帧持续时间，单位为秒
	int64_t		pos;            // 该帧在输入文件中的字节位置
	int		width;              // 图像宽度
	int		height;             // 图像高读
	int		format;             // 对于图像为(enum AVPixelFormat)，对于声音则为(enum AVSampleFormat)
	AVRational	sar;            // 图像的宽高比，如果未知或未指定则为0/1
	int		uploaded;           // 用来记录该帧是否已经显示过？
	int		flip_v;             // =1则旋转180， = 0则正常播放
} Frame;

class PacketQueue;

class FrameQueue
{
public:
	FrameQueue();
	~FrameQueue();

	// 队列初始化
	int frame_queue_init(PacketQueue *pktq, int max_size, int keep_last = 0);
	// 销毁队列
	void frame_queue_destory(void);
	// 释放信号唤醒其他
	void frame_queue_signal(void);
	// 从队列取出当前需要显示的一帧
	Frame *frame_queue_peek(void);
	// 从队列取出当前需要显示的下一帧
	Frame *frame_queue_peek_next(void);
	// 从队列取出最近被播放的一帧
	Frame *frame_queue_peek_last(void);
	// 检测队列是否可写空间
	Frame *frame_queue_peek_writable(void);
	// 检测队列是否有数据可读
	Frame *frame_queue_peek_readable(void);
	// 队列真正增加一帧，frame_queue_peek_writable只是获取一个可写的位置，但计数器没有修改
	void frame_queue_push(void);
	// 真正减少一帧数据，从帧队列中取出帧之后的参数操作，当rindex_show为0的时候使其变为1，否则出队列一帧
	int frame_queue_next(void);
	// 剩余帧数
	int frame_queue_nb_remaining(void);
	// 最近显示的一帧其在媒体文件中的偏移位置
	int64_t frame_queue_last_pos(void);

public:
	void frame_queue_unref_item(Frame * vp);

	Frame	queue_[FRAME_QUEUE_SIZE];        // FRAME_QUEUE_SIZE  最大size, 数字太大时会占用大量的内存，需要注意该值的设置
	int		rindex_ = 0;                         // 表示循环队列的结尾处
	int		windex_ = 0;                         // 表示循环队列的开始处
	int		size_ = 0;                           // 当前队列的有效帧数
	int		max_size_ = 0;                       // 当前队列最大的帧数容量
	int		keep_last_ = 0;                      // = 1说明要在队列里面保持最后一帧的数据不释放，只在销毁队列的时候才将其真正释放
	int		rindex_shown_ = 0;                   // 初始化为0，配合keep_last=1使用
	SDL_mutex	*mutex_ = NULL;                     // 互斥量
	SDL_cond	*cond_ = NULL;                      // 条件变量
	PacketQueue	*pktq_ = NULL;                      // 数据包缓冲队列
};

#endif