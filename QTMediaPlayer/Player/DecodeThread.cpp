#include "DecodeThread.h"
#include "Decoder.h"
#include "FrameQueue.h"
#include "PacketQueue.h"
#include "MediaPlayer.h"
#include "logger.h"
using namespace toolkit;

static int decoder_decode_frame(Decoder *d, AVFrame *frame, AVSubtitle *sub)
{
	int ret = AVERROR(EAGAIN);

	for (;;) {
		AVPacket pkt;

		if (d->queue_->serial_ == d->pkt_serial) {
			do {
				if (d->queue_->abort_request_)
					return -1;

				switch (d->avctx_->codec_type) {
				case AVMEDIA_TYPE_VIDEO:
					ret = avcodec_receive_frame(d->avctx_, frame);
					if (ret >= 0) 
					{
						frame->pts = frame->best_effort_timestamp;
					}
					break;
				case AVMEDIA_TYPE_AUDIO:
					ret = avcodec_receive_frame(d->avctx_, frame);
					if (ret >= 0) 
					{
						frame->pts = frame->best_effort_timestamp;
					}
					break;
				}
				if (ret == AVERROR_EOF) {
					d->finished = d->pkt_serial;
					avcodec_flush_buffers(d->avctx_);
					return 0;
				}
				if (ret >= 0)
					return 1;
			} while (ret != AVERROR(EAGAIN));
		}

		do {
			if (d->queue_->nb_packets_ == 0)
				SDL_CondSignal(d->empty_queue_cond_);
			if (d->packet_pending) {
				av_packet_move_ref(&pkt, &d->pkt_);
				d->packet_pending = 0;
			}
			else {
				if (d->queue_->packet_queue_get(&pkt, 1, &d->pkt_serial) < 0)
					return -1;
			}
		} while (d->queue_->serial_ != d->pkt_serial);

		if (pkt.data == PacketQueue::GetFlushPacket()->data) 
		{
			avcodec_flush_buffers(d->avctx_); //清空里面的缓存帧
			DebugL << d->queue_->name_ << " avcodec_flush_buffers";
			d->finished = 0;
			d->next_pts = d->start_pts;
			d->next_pts_tb = d->start_pts_tb;
		}
		else {
			if (d->avctx_->codec_type == AVMEDIA_TYPE_SUBTITLE) {
				int got_frame = 0;
				ret = avcodec_decode_subtitle2(d->avctx_, sub, &got_frame, &pkt);
				if (ret < 0) {
					ret = AVERROR(EAGAIN);
				}
				else {
					if (got_frame && !pkt.data) {
						d->packet_pending = 1;
						av_packet_move_ref(&d->pkt_, &pkt);
					}
					ret = got_frame ? 0 : (pkt.data ? AVERROR(EAGAIN) : AVERROR_EOF);
				}
			}
			else {
				if (avcodec_send_packet(d->avctx_, &pkt) == AVERROR(EAGAIN)) {
					av_log(d->avctx_, AV_LOG_ERROR, "Receive_frame and send_packet both returned EAGAIN, which is an API violation.\n");
					d->packet_pending = 1;
					av_packet_move_ref(&d->pkt_, &pkt);
				}
			}
			av_packet_unref(&pkt);	// 一定要自己去释放音视频数据
		}
	}
}

static int queue_picture(MediaPlayer *is, AVFrame *src_frame, double pts,
	double duration, int64_t pos, int serial)
{
	Frame *vp;  // AVFrame

	if (!(vp = is->pictq->frame_queue_peek_writable()))	/* 检测队列是否有可写空间 */
		return -1;

	vp->sar = src_frame->sample_aspect_ratio;
	vp->uploaded = 0;

	vp->width  = src_frame->width;
	vp->height = src_frame->height;
	vp->format = src_frame->format;

	vp->pts			= pts;
	vp->duration	= duration;
	vp->pos			= pos;
	vp->serial		= serial;

	av_frame_move_ref(vp->frame, src_frame);
	is->pictq->frame_queue_push();
	return 0;
}

int video_thread(void *arg)
{
	MediaPlayer	*is = (MediaPlayer *)arg;
	AVFrame		*frame = av_frame_alloc();
	double		pts;
	double		duration;
	int		ret;
	AVRational	tb = is->video_st->time_base;  // 时间基
	AVRational	frame_rate = av_guess_frame_rate(is->format_context_, is->video_st, NULL);
	int got_picture;

	if (!frame) {

		return AVERROR(ENOMEM);
	}

	for (;;) {
		ret = decoder_decode_frame(is->viddec, frame, NULL);

		if (ret < 0)
		{
			ErrorL <<"decoder_decode_frame failed";
			goto the_end;
		}
		if (!ret)
		{
			DebugL << "no get frame";
			continue;
		}
		// 根据帧率计算出每帧该持续的时间
		AVRational rational = { frame_rate.den, frame_rate.num };
		duration = (frame_rate.num && frame_rate.den ? av_q2d(rational) : 0);
		pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
 		DebugL << "vpts = " << pts;
		// duration和pts都是秒的单位
		ret = queue_picture(is, frame, pts, duration, frame->pkt_pos, is->viddec->pkt_serial);
		av_frame_unref(frame);

		if (ret < 0)
			goto the_end;
	}
the_end:
	av_frame_free(&frame);
	DebugL << "finish";
	return 0;
}


int audio_thread(void *arg)
{
	MediaPlayer	*is = (MediaPlayer *)arg;
	AVFrame		*frame = av_frame_alloc();
	Frame		*af;

	int		got_frame = 0;
	int		ret = 0;
	AVRational	tb = is->audio_st->time_base;  // 时间基
	if (!frame)
		return AVERROR(ENOMEM);

	do {
		if ((got_frame = decoder_decode_frame(is->auddec, frame, NULL)) < 0)
		{
			ErrorL << "decoder_decode_frame failed";
			goto the_end;
		}
		if (got_frame)
		{
			if (!(af = is->sampq->frame_queue_peek_writable()))
			{
				ErrorL << "frame_queue_peek_writable failed";
				goto the_end;
			}
			af->pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
			DebugL << "apts = " << af->pts;
			af->pos = frame->pkt_pos;
			af->serial = is->auddec->pkt_serial;
			AVRational rational = { frame->nb_samples, frame->sample_rate };
			af->duration = av_q2d(rational);

			av_frame_move_ref(af->frame, frame);
			is->sampq->frame_queue_push();
		}
	} while (ret >= 0 || ret == AVERROR(EAGAIN) || ret == AVERROR_EOF);
the_end:
	av_frame_free(&frame);
	return ret;
}
