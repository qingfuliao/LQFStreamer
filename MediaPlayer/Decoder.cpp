#include "Decoder.h"
#include "FrameQueue.h"
#include "PacketQueue.h"


Decoder::Decoder()
{
}


Decoder::~Decoder()
{
	if (avctx)
	{
		decoder_destroy();
	}
}

int Decoder::decoder_init(AVCodecContext * avctx, PacketQueue * queue, SDL_cond * empty_queue_cond)
{
	avctx = avctx;
	queue = queue;
	empty_queue_cond = empty_queue_cond;
	start_pts = AV_NOPTS_VALUE;
	pkt_serial = -1;
	return 0;
}

int Decoder::decoder_start(int(*fn)(void *), void * arg)
{
	queue->packet_queue_start();
	decoder_tid = SDL_CreateThread(fn, "decoder", arg);
	if (!decoder_tid) {
		av_log(NULL, AV_LOG_ERROR, "SDL_CreateThread(): %s\n", SDL_GetError());
		return AVERROR(ENOMEM);
	}
	return 0;
}

void Decoder::decoder_abort(FrameQueue * fq)
{
	queue->packet_queue_abort();
	fq->frame_queue_signal();
	if (decoder_tid)
		SDL_WaitThread(decoder_tid, NULL);
	decoder_tid = NULL;
	queue->packet_queue_flush();
}

void Decoder::decoder_destroy(void)
{
	av_packet_unref(&pkt);
	avcodec_free_context(&avctx);
}
