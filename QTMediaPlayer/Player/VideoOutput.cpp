#include "VideoOutput.h"
#include "MediaPlayer.h"
#include "FrameQueue.h"
#include "Clock.h"
#include "PacketQueue.h"
#include "VideoWidget.h"
#include "MediaPlayer.h"
#include "Decoder.h"
#include "logger.h"
using namespace toolkit;
#ifdef __cplusplus
extern "C"
{
#include "libavutil/time.h"
#include "SDL.h"
}
#endif

#define REFRESH_RATE  0.01
int framedrop = 2;

static int sRefreshThread(void *arg)
{
	VideoOutput *video_output = (VideoOutput *)arg;
	return video_output->refreshThread();	
}
/**
* @brief 计算正在显示帧需要持续播放的时间，通常情况下返回的是相邻帧间隔。
* @param delay 该参数实际传递的是当前显示帧和准备要播放帧的间隔。
* @param is
* @return 返回当前显示帧要持续播放的时间。为什么要调整返回的delay？为什么不支持使用相邻间隔帧时间？
*/
static double compute_target_delay(double delay, MediaPlayer *is)
{
	double sync_threshold, diff = 0;

	/* update delay to follow master synchronisation source */
	/* 如果发现当前主Clock源不是video，则计算当前视频时钟与主时钟的差值 */
	if (Clock::get_master_sync_type(is) != AV_SYNC_VIDEO_MASTER) {
		/* if video is slave, we try to correct big delays by
		duplicating or deleting a frame */
		diff = is->vidclk->get_clock() - Clock::get_master_clock(is);

		/* skip or repeat frame. We take into account the
		delay to compute the threshold. I still don't know
		if it is the best guess */
		sync_threshold = FFMAX(AV_SYNC_THRESHOLD_MIN, FFMIN(AV_SYNC_THRESHOLD_MAX, delay));
		if (!isnan(diff) && fabs(diff) < is->max_frame_duration) {
			if (diff <= -sync_threshold)
				delay = FFMAX(0, delay + diff);
			else if (diff >= sync_threshold && delay > AV_SYNC_FRAMEDUP_THRESHOLD)
				delay = delay + diff;
			else if (diff >= sync_threshold)
				delay = 2 * delay;
		}
	}

	av_log(NULL, AV_LOG_TRACE, "video: delay=%0.3f A-V=%f\n",
		delay, -diff);

	return delay;
}

static void update_video_pts(MediaPlayer *is, double pts, int64_t pos, int serial) 
{
	/* update current video pts */
	is->vidclk->set_clock(pts, serial);
	is->extclk->sync_clock_to_slave(is->vidclk);
}

VideoOutput::VideoOutput(MediaPlayer *player)
	:player_(player)
{
}


VideoOutput::~VideoOutput()
{
}

bool VideoOutput::video_open()
{
	abort_request = false;
	refresh_thread_tid_ = SDL_CreateThread(sRefreshThread, "refresh_thread", this);
	if (!refresh_thread_tid_)
	{
		av_log(NULL, AV_LOG_FATAL, "SDL_CreateThread(): %s\n", SDL_GetError());
		return false;
	}
	return true;
}

bool VideoOutput::video_stop()
{
	abort_request = true;
	if (refresh_thread_tid_)
		SDL_WaitThread(refresh_thread_tid_, NULL);	// 等待数据读取线程退出

	if (is_pic_init_)
	{
		avpicture_free(&picture);
		is_pic_init_ = false;
	}
	return true;
}

int VideoOutput::refreshThread()
{
	double remaining_time = 0.0; /* 休眠等待，remaining_time的计算在video_refresh中 */

	while (true)
	{
		if (abort_request)
		{
			break;
		}
		if (remaining_time > 0.0)
		{
			av_usleep((int64_t)(remaining_time * 1000000.0));    /* 转换为为微妙 */
		}

		remaining_time = REFRESH_RATE;      // 这个参数决定了每次最长的休眠时间，比如这里缺省是0.01秒，即是10ms

											/* 调用video_refresh方法，刷新视频帧 */
		video_refresh(&remaining_time);                   /* 非暂停或者强制刷新时调用 */
	}
	return 0;
}
double VideoOutput::vp_duration(Frame *vp, Frame *nextvp) 
{
	if (vp->serial == nextvp->serial) 
	{
		double duration = nextvp->pts - vp->pts;
		if (isnan(duration) || duration <= 0 || duration > player_->max_frame_duration)
			return vp->duration / player_->speed_;	 /* 异常时以帧时间为基准(1秒/帧率) */
		else
			return duration / player_->speed_;
	}
	else {
		return 0.0;
	}
}
void VideoOutput::video_refresh(double * remaining_time)
{
	MediaPlayer *is = player_;
	double time;

	Frame *sp, *sp2;

	if (!is->paused && Clock::get_master_sync_type(player_) == AV_SYNC_EXTERNAL_CLOCK && is->realtime)
		Clock::check_external_clock_speed(player_);

	if (is->video_st) {
	retry:
		if (is->pictq->frame_queue_nb_remaining() == 0) 
		{
			// nothing to do, no picture to display in the queue
// 			DebugL << "no frame";
		}
		else 
		{
			double last_duration, duration, delay;
			Frame *vp, *lastvp;

			/* dequeue the picture */
			lastvp = is->pictq->frame_queue_peek_last();
			vp = is->pictq->frame_queue_peek();

			if (vp->serial != is->videoq->serial_) 
			{
				is->pictq->frame_queue_next();
				DebugL << "retry vpts =" << vp->pts;
				goto retry;
			}

			if (lastvp->serial != vp->serial)
				is->frame_timer = av_gettime_relative() / 1000000.0;

			if (is->paused)
			{
				goto display;
			}
			/* compute nominal last_duration */
			/*
			* last_duration 根据当前帧和上一帧的pts计算出来上一帧的显示需要持续的时间
			* 如果是video master的模式，则只需要计算出每帧持续播放的时间，按帧率播放即可
			*/
			last_duration = vp_duration(lastvp, vp);
			/* 经过compute_target_delay方法，计算出正在显示的帧要持续播放的时间 */
			// 如果以video clock为master，则delay直接等于last_duration。
			// 如果是以audio/external clock为master，则需要比对主时钟调整当前帧要持续播放的时间。
			delay = compute_target_delay(last_duration, is);
// 			DebugL << "delay = " << delay << ", vpts = " << vp->pts;
			time = av_gettime_relative() / 1000000.0;
			if (time < is->frame_timer + delay) {
				*remaining_time = FFMIN(is->frame_timer + delay - time, *remaining_time);
				goto display;
			}

			is->frame_timer += delay;
			if (delay > 0 && time - is->frame_timer > AV_SYNC_THRESHOLD_MAX)
				is->frame_timer = time;

			SDL_LockMutex(is->pictq->mutex_);
			if (!isnan(vp->pts))
				update_video_pts(is, vp->pts / player_->speed_, vp->pos, vp->serial);
			SDL_UnlockMutex(is->pictq->mutex_);

			if (is->pictq->frame_queue_nb_remaining() > 1) 
			{
				Frame *nextvp = is->pictq->frame_queue_peek_next();
				duration = vp_duration(vp, nextvp);
				if (!is->step && (framedrop>0 || (framedrop && Clock::get_master_sync_type(is) != AV_SYNC_VIDEO_MASTER)) && time > is->frame_timer + duration) {
					is->frame_drops_late++;
					is->pictq->frame_queue_next();
					goto retry;
				}
			}

			is->pictq->frame_queue_next();
			is->force_refresh = 1;	 /* 说明需要刷新视频帧 */

			if (is->step && !is->paused)
				is->stream_toggle_pause();
		}
	display:
		/* display picture */
		if (is->force_refresh &&  is->pictq->rindex_shown_)
		{
			video_display();
		}
	}
	is->force_refresh = 0;
}

void VideoOutput::video_display()
{
	if (player_->video_st)
		video_image_display();
}

// 比较占用cpu的函数调用
void VideoOutput::video_image_display()
{
	Frame *vp;
	Frame *sp = NULL;
	SDL_Rect rect;
	vp = player_->pictq->frame_queue_peek_last();
	DebugL << "video_display pts = " << vp->pts;
	AVFrame		*frame = vp->frame;
	width_ = frame->width;
	height_ = frame->height;
	if (is_first_frame_)
	{
		is_first_frame_ = false;
		player_->video_dev_->InitShader(width_, height_);
	}
	
	convert_ctx = sws_getCachedContext(convert_ctx, width_, height_, 
		 player_->viddec->avctx_->pix_fmt, width_, height_, AV_PIX_FMT_YUV420P, 
		SWS_FAST_BILINEAR, NULL, NULL, NULL);

	if (!is_pic_init_)
	{
		int ret = avpicture_alloc(&picture, AV_PIX_FMT_YUV420P, width_, height_);
		is_pic_init_ = true;
	}

	int rr = sws_scale(convert_ctx, frame->data, frame->linesize,
		0, frame->height, picture.data, picture.linesize);

	player_->video_dev_->UpdateData(picture.data);
}
