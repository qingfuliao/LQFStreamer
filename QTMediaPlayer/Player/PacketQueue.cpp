#include "PacketQueue.h"
#include "logger.h"

using namespace toolkit;

PacketQueue::PacketQueue():
	first_pkt_(NULL),
	last_pkt_(NULL),
	cond_(NULL),
	mutex_(NULL)
{
}

PacketQueue::~PacketQueue()
{
	if (cond_)
	{
		packet_queue_destroy();
	}
}

int PacketQueue::packet_queue_init(const char *name)
{
	mutex_ = SDL_CreateMutex();
	if (!mutex_) {
		ErrorL << "SDL_CreateMutex():" << SDL_GetError();
		return -1;
	}
	cond_ = SDL_CreateCond();
	if (!cond_) {
		ErrorL << "SDL_CreateCond():" << SDL_GetError();
		return -1;
	}
	strcpy(name_, name);
	abort_request_ = 1;

	return 0;
}

void PacketQueue::packet_queue_start()
{
	SDL_LockMutex(mutex_);
	abort_request_ = 0;
	packet_queue_put_private(GetFlushPacket());
	SDL_UnlockMutex(mutex_);
}

int PacketQueue::packet_queue_put(AVPacket * pkt)
{
	int ret;

	SDL_LockMutex(mutex_);
	ret = packet_queue_put_private(pkt);
	SDL_UnlockMutex(mutex_);

	if (pkt != GetFlushPacket() && ret < 0)
		av_packet_unref(pkt);	/* 插入失败时则引用计数减一，以释放内存*/

	return ret;
}

int PacketQueue::packet_queue_get(AVPacket * pkt, int block, int * serial)
{
	MyAVPacketList *pkt1;
	int ret;

	SDL_LockMutex(mutex_);

	for (;;)
	{
		if (abort_request_)
		{
			ret = -1;
			DebugL << "abort_request";
			break;
		}

		pkt1 = first_pkt_;
		if (pkt1)
		{
			first_pkt_ = pkt1->next;
			if (!first_pkt_)
			{
				last_pkt_ = NULL;
			}
			nb_packets_--;
			size_ -= pkt1->pkt.size + sizeof(*pkt1);
			duration_ -= pkt1->pkt.duration;
			*pkt = pkt1->pkt;
			if (serial)
				*serial = pkt1->serial;
			av_free(pkt1);
			ret = 1;
			break;
		}
		else if (!block)
		{
			ret = 0;
			break;
		}
		else
		{
			SDL_CondWait(cond_, mutex_);
		}
	}
	SDL_UnlockMutex(mutex_);
	return ret;
}

void PacketQueue::packet_queue_flush(void)
{
	MyAVPacketList *pkt, *pkt1;

	SDL_LockMutex(mutex_);
	for (pkt = first_pkt_; pkt; pkt = pkt1)
	{
		pkt1 = pkt->next;
		if (pkt->pkt.buf != GetFlushPacket()->buf)
			av_packet_unref(&pkt->pkt);
		else
			DebugL << "flush flush_pkt";
		av_freep(&pkt);
	}
	last_pkt_ = NULL;
	first_pkt_ = NULL;
	nb_packets_ = 0;
	size_ = 0;
	duration_ = 0;
	SDL_UnlockMutex(mutex_);
}

int PacketQueue::packet_queue_put_nullpacket(int stream_index)
{
	AVPacket pkt1, *pkt = &pkt1;
	av_init_packet(pkt);
	pkt->data = NULL;
	pkt->size = 0;
	pkt->stream_index = stream_index;
	return packet_queue_put(pkt);
}

void PacketQueue::packet_queue_abort(void)
{
	SDL_LockMutex(mutex_);

	abort_request_ = 1;

	SDL_CondSignal(cond_);

	SDL_UnlockMutex(mutex_);
}

void PacketQueue::packet_queue_destroy(void)
{
	packet_queue_flush();
	SDL_DestroyMutex(mutex_);
	mutex_ = NULL;
	SDL_DestroyCond(cond_);
	cond_ = NULL;
}

AVPacket * PacketQueue::GetFlushPacket()
{
	static AVPacket flush_pkt;
	static bool flush_pkt_init = false;
	if (!flush_pkt_init)
	{
		flush_pkt_init = true;
		av_init_packet(&flush_pkt);				// 初始化flush_packet
		flush_pkt.data = (uint8_t *)&flush_pkt; // 初始化为数据指向自己本身
	}

	return &flush_pkt;
}


int PacketQueue::packet_queue_put_private(AVPacket *pkt)
{
	MyAVPacketList *pkt1;

	if (abort_request_)
		return -1;

	pkt1 = (MyAVPacketList *)av_malloc(sizeof(MyAVPacketList));
	if (!pkt1)
		return -1;
	pkt1->pkt = *pkt;
	pkt1->next = NULL;
	if (pkt == GetFlushPacket())
	{
		serial_++; /*  */
		DebugL << "name:" << name_ << ", serial = " << serial_;
	}
	pkt1->serial = serial_;

	if (!last_pkt_)
		first_pkt_ = pkt1;
	else
		last_pkt_->next = pkt1;
	last_pkt_ = pkt1;
	nb_packets_++;
	size_ += pkt1->pkt.size + sizeof(*pkt1);
	duration_ += pkt1->pkt.duration;
	/* XXX: should duplicate packet data in DV case */
	SDL_CondSignal(cond_);
	return 0;
}