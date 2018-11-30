#include "FrameQueue.h"
#include "PacketQueue.h"

#include "LogUtil.h"
FrameQueue::FrameQueue():
	mutex(NULL),
	cond(NULL)
{
}


FrameQueue::~FrameQueue()
{
	if (mutex)
	{
		frame_queue_destory();
	}
}

int FrameQueue::frame_queue_init(PacketQueue * pktq, int max_size, int keep_last)
{
	int i;
	if (!(mutex = SDL_CreateMutex())) {
		LogError("SDL_CreateMutex(): %s\n", SDL_GetError());
		return -1;
	}
	if (!(cond = SDL_CreateCond())) {
		LogError("SDL_CreateCond(): %s\n", SDL_GetError());
		return -1;
	}
	pktq = pktq;
	max_size = FFMIN(max_size, FRAME_QUEUE_SIZE);
	keep_last = !!keep_last;
	for (i = 0; i < max_size; i++)
	{
		if (!(queue[i].frame = av_frame_alloc()))
		{
			LogError("av_frame_alloc(): %s\n", SDL_GetError());
			return -1;
		}
	}
	return 0;
}

void FrameQueue::frame_queue_destory(void)
{
	int i;
	for (i = 0; i < max_size; i++) 
	{
		Frame *vp = &queue[i];
		frame_queue_unref_item(vp); /* 释放数据 */
		av_frame_free(&vp->frame);
	}
	SDL_DestroyMutex(mutex);
	mutex = NULL;
	SDL_DestroyCond(cond);
	cond = NULL;
}

void FrameQueue::frame_queue_signal(void)
{
	SDL_LockMutex(mutex);
	SDL_CondSignal(cond);
	SDL_UnlockMutex(mutex);
}

Frame * FrameQueue::frame_queue_peek(void)
{
	return &queue[(rindex + rindex_shown) % max_size];
}

Frame * FrameQueue::frame_queue_peek_next(void)
{
	return &queue[(rindex + rindex_shown + 1) % max_size];
}

Frame * FrameQueue::frame_queue_peek_last(void)
{
	return &queue[rindex];
}

Frame * FrameQueue::frame_queue_peek_writable(void)
{
	/* wait until we have space to put a new frame */
	SDL_LockMutex(mutex);
	while (size >= max_size &&
		!pktq->get_abort_request()) {	/* 检查是否需要退出 */
		SDL_CondWait(cond, mutex);
	}
	SDL_UnlockMutex(mutex);

	if (pktq->get_abort_request())		 /* 检查是不是要退出 */
		return NULL;

	return &queue[windex];
}

Frame * FrameQueue::frame_queue_peek_readable(void)
{
	/* wait until we have a readable a new frame */
	SDL_LockMutex(mutex);
	while (size - rindex_shown <= 0 &&
		!pktq->get_abort_request())
	{
		SDL_CondWait(cond, mutex);
	}
	SDL_UnlockMutex(mutex);

	if (pktq->get_abort_request())
		return NULL;

	return &queue[(rindex + rindex_shown) % max_size];
}

void FrameQueue::frame_queue_push(void)
{
	if (++windex == max_size)	/* 循环写入 */
		windex = 0;
	SDL_LockMutex(mutex);
	size++;
	SDL_CondSignal(cond);
	SDL_UnlockMutex(mutex);
}

int FrameQueue::frame_queue_next(void)
{
	if (keep_last && !rindex_shown) {
		rindex_shown = 1;
		return 0;
	}
	frame_queue_unref_item(&queue[rindex]);
	if (++rindex == max_size)
		rindex = 0;
	SDL_LockMutex(mutex);
	size--;
	SDL_CondSignal(cond);
	SDL_UnlockMutex(mutex);
	return 0;
}

int FrameQueue::frame_queue_nb_remaining(void)
{
	return size - rindex_shown; // 注意这里为什么要减去rindex_shown
}

int64_t FrameQueue::frame_queue_last_pos(void)
{
	Frame *fp = &queue[rindex];
	if (rindex_shown && fp->serial == pktq->get_serial())
		return fp->pos;
	else
		return -1;
}

void FrameQueue::frame_queue_unref_item(Frame *vp)
{
	av_frame_unref(vp->frame); /* 释放数据 */
	avsubtitle_free(&vp->sub);
}