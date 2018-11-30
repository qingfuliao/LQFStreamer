#ifndef PACKET_QUEUE_H
#define PACKET_QUEUE_H

#ifdef __cplusplus
extern "C"
{
#include "libavformat/avformat.h"
#include "SDL.h"
}
#endif

typedef struct MyAVPacketList {
	AVPacket		pkt;
	struct MyAVPacketList	*next;          // 指向下一个元素
	int			serial;
} MyAVPacketList;

class PacketQueue
{
public:
	PacketQueue();
	~PacketQueue();
	int packet_queue_init(const char *name);
	void packet_queue_start();
	int packet_queue_put(AVPacket *pkt);
	/**
	* @brief 获取数据包
	* @param q
	* @param pkt
	* @param block 是否阻塞式获取数据包
	* @return 队列状态
	*  @retval -1  播放退出请求
	*  @retval 0   没有数据包可获取
	*  @retval 1   获取到数据包
	*/
	int packet_queue_get(AVPacket *pkt, int block, int *serial);
	void packet_queue_flush(void);
	int packet_queue_put_nullpacket(int stream_index);
	void packet_queue_abort(void);
	void packet_queue_destroy(void);
	int get_abort_request() const
	{
		return abort_request;
	}
	int get_serial() const
	{
		return serial;
	}
	int get_size() const
	{
		return size;
	}
	int get_nb_packets() const
	{
		return nb_packets;
	}
	
	static AVPacket *GetFlushPacket();
	
private:
	int packet_queue_put_private(AVPacket * pkt);


	MyAVPacketList *first_pkt;
	MyAVPacketList *last_pkt;
	int nb_packets;             // 包数量
	int64_t		duration;       // 队列所有元素的数据播放持续时间
	int size;                   // 队列存储的媒体数据总大小
	int		serial;
	SDL_mutex *mutex;
	SDL_cond *cond;
	int abort_request;          // 是否退出等待
	char		name_[20];       // 存储队列名称以方便debug
};
#endif
