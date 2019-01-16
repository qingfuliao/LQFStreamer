#include "FrameQueue.h"

#include "logger.h"
#include "PacketQueue.h"

using namespace toolkit;
FrameQueue::FrameQueue():
	mutex_(NULL),
	cond_(NULL)
{
	for (int i = 0; i < max_size_; i++)
	{
		queue_[i].frame = NULL;
	}
}

FrameQueue::~FrameQueue()
{
	if (mutex_)
	{
		frame_queue_destory();
	}
}

int FrameQueue::frame_queue_init(PacketQueue * pktq, int max_size, int keep_last)
{
	if (!(mutex_ = SDL_CreateMutex())) {
		ErrorL << "SDL_CreateMutex():" << SDL_GetError();
		return -1;
	}
	if (!(cond_ = SDL_CreateCond())) {
		ErrorL <<"SDL_CreateCond(): ", SDL_GetError();
		return -1;
	}
	pktq_ = pktq;
	max_size_ = FFMIN(max_size, FRAME_QUEUE_SIZE);
	keep_last_ = !!keep_last;
	for (int i = 0; i < max_size_; i++)
	{
		if (!(queue_[i].frame = av_frame_alloc()))
		{
			ErrorL <<"av_frame_alloc(): ", SDL_GetError();
			return -1;
		}
	}
	return 0;
}

void FrameQueue::frame_queue_destory(void)
{
	for (int i = 0; i < max_size_; i++) 
	{
		Frame *vp = &queue_[i];
		frame_queue_unref_item(vp); /* 释放数据 */
		av_frame_free(&vp->frame);
	}
	if(mutex_)
	{ 
		SDL_DestroyMutex(mutex_);
		mutex_ = NULL;
	}
	if (cond_)
	{
		SDL_DestroyCond(cond_);
		cond_ = NULL;
	}
}

void FrameQueue::frame_queue_signal(void)
{
	SDL_LockMutex(mutex_);
	SDL_CondSignal(cond_);
	SDL_UnlockMutex(mutex_);
}

Frame * FrameQueue::frame_queue_peek(void)
{
	return &queue_[(rindex_ + rindex_shown_) % max_size_];
}

Frame * FrameQueue::frame_queue_peek_next(void)
{
	return &queue_[(rindex_ + rindex_shown_ + 1) % max_size_];
}

Frame * FrameQueue::frame_queue_peek_last(void)
{
	return &queue_[rindex_];
}

Frame * FrameQueue::frame_queue_peek_writable(void)
{
	/* wait until we have space to put a new frame */
	SDL_LockMutex(mutex_);
	while (size_ >= max_size_ &&
		!pktq_->get_abort_request()) {	/* 检查是否需要退出 */
		SDL_CondWait(cond_, mutex_);
	}
	SDL_UnlockMutex(mutex_);

	if (pktq_->get_abort_request())		 /* 检查是不是要退出 */
		return NULL;

	return &queue_[windex_];
}

Frame * FrameQueue::frame_queue_peek_readable(void)
{
	/* wait until we have a readable a new frame */
	SDL_LockMutex(mutex_);
	while (size_ - rindex_shown_ <= 0 &&
		!pktq_->get_abort_request())
	{
		SDL_CondWait(cond_, mutex_);
	}
	SDL_UnlockMutex(mutex_);

	if (pktq_->get_abort_request())
		return NULL;

	return &queue_[(rindex_ + rindex_shown_) % max_size_];
}

void FrameQueue::frame_queue_push(void)
{
	if (++windex_ == max_size_)	/* 循环写入 */
		windex_ = 0;
	SDL_LockMutex(mutex_);
	size_++;
	SDL_CondSignal(cond_);
	SDL_UnlockMutex(mutex_);
}

int FrameQueue::frame_queue_next(void)
{
	if (keep_last_ && !rindex_shown_) {
		rindex_shown_ = 1;
		return 0;
	}
	frame_queue_unref_item(&queue_[rindex_]);
	if (++rindex_ == max_size_)
		rindex_ = 0;
	SDL_LockMutex(mutex_);
	size_--;
	SDL_CondSignal(cond_);
	SDL_UnlockMutex(mutex_);
	return 0;
}

int FrameQueue::frame_queue_nb_remaining(void)
{
	return size_ - rindex_shown_; // 注意这里为什么要减去rindex_shown
}

int64_t FrameQueue::frame_queue_last_pos(void)
{
	Frame *fp = &queue_[rindex_];
	if (rindex_shown_ && fp->serial == pktq_->get_serial())
		return fp->pos;
	else
		return -1;
}

void FrameQueue::frame_queue_unref_item(Frame *vp)
{
	av_frame_unref(vp->frame); /* 释放数据 */
	//avsubtitle_free(&vp->sub);
}