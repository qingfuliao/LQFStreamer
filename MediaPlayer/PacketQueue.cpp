#include "PacketQueue.h"
#include "LogUtil.h"


PacketQueue::PacketQueue():
	first_pkt(NULL),
	last_pkt(NULL),
	cond(NULL),
	mutex(NULL)
{
}

PacketQueue::~PacketQueue()
{
	if (cond)
	{
		packet_queue_flush();
		packet_queue_destroy();
	}
}

int PacketQueue::packet_queue_init(const char *name)
{
	mutex = SDL_CreateMutex();
	if (!mutex) {
		LogError("SDL_CreateMutex(): %s\n", SDL_GetError());
		return -1;
	}
	cond = SDL_CreateCond();
	if (!cond) {
		LogError("SDL_CreateCond(): %s\n", SDL_GetError());
		return -1;
	}
	strcpy(name_, name);
	abort_request = 1;

	return 0;
}

void PacketQueue::packet_queue_start()
{
	SDL_LockMutex(mutex);
	abort_request = 0;
	packet_queue_put_private(GetFlushPacket());
	SDL_UnlockMutex(mutex);
}

int PacketQueue::packet_queue_put(AVPacket * pkt)
{
	int ret;

	SDL_LockMutex(mutex);
	ret = packet_queue_put_private(pkt);
	SDL_UnlockMutex(mutex);

	if (pkt != GetFlushPacket() && ret < 0)
		av_packet_unref(pkt);	/* 插入失败时则引用计数减一，以释放内存*/

	return ret;
}

int PacketQueue::packet_queue_get(AVPacket * pkt, int block, int * serial)
{
	MyAVPacketList *pkt1;
	int ret;

	SDL_LockMutex(mutex);

	for (;;)
	{
		if (abort_request)
		{
			ret = -1;
			LogDebug("abort_request");
			break;
		}

		pkt1 = first_pkt;
		if (pkt1)
		{
			first_pkt = pkt1->next;
			if (!first_pkt)
			{
				last_pkt = NULL;
			}
			nb_packets--;
			size -= pkt1->pkt.size + sizeof(*pkt1);
			duration -= pkt1->pkt.duration;
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
			SDL_CondWait(cond, mutex);
		}
	}
	SDL_UnlockMutex(mutex);
	return ret;
}

void PacketQueue::packet_queue_flush(void)
{
	LogDebug("%s into", name_);
	MyAVPacketList *pkt, *pkt1;

	SDL_LockMutex(mutex);
	for (pkt = first_pkt; pkt; pkt = pkt1)
	{
		pkt1 = pkt->next;
		if (pkt->pkt.buf != GetFlushPacket()->buf)
			av_packet_unref(&pkt->pkt);
		else
			LogDebug("no flush flush_pkt");
		av_freep(&pkt);
	}
	last_pkt = NULL;
	first_pkt = NULL;
	nb_packets = 0;
	size = 0;
	duration = 0;
	SDL_UnlockMutex(mutex);
	LogDebug("leave");
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
	SDL_LockMutex(mutex);

	abort_request = 1;

	SDL_CondSignal(cond);

	SDL_UnlockMutex(mutex);
}

void PacketQueue::packet_queue_destroy(void)
{
	SDL_DestroyMutex(mutex);
	mutex = NULL;
	SDL_DestroyCond(cond);
	cond = NULL;
}

AVPacket * PacketQueue::GetFlushPacket()
{
	static AVPacket flush_pkt;

	return &flush_pkt;
}


int PacketQueue::packet_queue_put_private(AVPacket *pkt)
{
	MyAVPacketList *pkt1;

	if (abort_request)
		return -1;

	pkt1 = (MyAVPacketList *)av_malloc(sizeof(MyAVPacketList));
	if (!pkt1)
		return -1;
	pkt1->pkt = *pkt;
	pkt1->next = NULL;
	if (pkt == GetFlushPacket())
	{
		serial++; /*  */
		LogDebug("packetqueue = %s, serial = %d\n",  name_, serial);
	}
	pkt1->serial = serial;

	if (!last_pkt)
		first_pkt = pkt1;
	else
		last_pkt->next = pkt1;
	last_pkt = pkt1;
	nb_packets++;
	size += pkt1->pkt.size + sizeof(*pkt1);
	duration += pkt1->pkt.duration;
	/* XXX: should duplicate packet data in DV case */
	SDL_CondSignal(cond);
	return 0;
}