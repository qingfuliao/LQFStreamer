#include "AudioOutput.h"
#include "Clock.h"
#include "Decoder.h"
#include "DecodeThread.h"
#include "Demuxer.h"
#include "FrameQueue.h"
#include "logger.h"
#include "MediaPlayer.h"
#include "PacketQueue.h"
#include "VideoOutput.h"

using namespace toolkit;
#ifdef __cplusplus
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "SDL.h"
#include "libavutil/imgutils.h"
#include "libavutil/time.h"
}
#endif

static int sReadThread(void *arg)
{
	MediaPlayer *player = (MediaPlayer *)arg;
	return player->read_thread();
}

static int sEventLoop(void *arg)
{
	MediaPlayer *player = (MediaPlayer *)arg;
	return player->event_loop();
}
static char err_buf[128] = { 0 };
static char* av_get_err(int errnum)
{
	av_strerror(errnum, err_buf, 128);
	return err_buf;
}

MediaPlayer::MediaPlayer(VideoWidget *video_dev)
	:video_dev_(video_dev)
{
	cmd_queue_ = new RingBuffer<PlayerConmandMessage *>(5);	
}

void MediaPlayer::RegisterEventCallback(pEventCallBack ecb, void *user)
{
	event_cb_ = ecb;
	event_data_ = user;

	event_loop_tid = SDL_CreateThread(sEventLoop, "event_loop", this);
	if (!event_loop_tid)
	{
		av_log(NULL, AV_LOG_FATAL, "SDL_CreateThread(): %s\n", SDL_GetError());
	}
}


MediaPlayer::~MediaPlayer()
{
	abort_app = true;				// 请求退出
	if(event_loop_tid)
	SDL_WaitThread(event_loop_tid, NULL);	// 等待数据读取线程退出
}

bool MediaPlayer::PushCommandMessage(PlayerConmandMessage * cmdMsg)
{
	return cmd_queue_->Push(cmdMsg);
}

void MediaPlayer::checkPlayFinish()
{
	if (eof &&  PLAYER_ST_STOPED != player_state_)
	{
		bool play_finish = true;
		if (play_finish && pictq && pictq->frame_queue_nb_remaining() > 0)
			play_finish = false;
		if (play_finish && sampq && sampq->frame_queue_nb_remaining() > 0)
			play_finish = false;
		if (play_finish && videoq && videoq->get_nb_packets() > 0)
			play_finish = false;
		if (play_finish && audioq && audioq->get_nb_packets() > 0)
			play_finish = false;
		if (play_finish)		// 文件读取完毕且缓存都已经播放完毕则停止播放
		{
			player_state_ = PLAYER_ST_STOPED;
			stream_close();
			event_cb_(PLAYER_EVENT_PLAYBACK_FINISHED, NULL, event_data_);
		}
	}
}

void MediaPlayer::toggle_pause()
{
	stream_toggle_pause();
	step = 0;
	DebugL << "step = 0; toggle_pause";
}

bool MediaPlayer::stream_open(const char *url)
{
	strcpy(filename, url);
	ytop = 0;
	xleft = 0;
	default_width = 640;
	default_height = 480;
	screen_width = 0;
	screen_height = 0;

	speed_ = 1.0;        // 一定要初始化

	// 初始化解码器前packet的队列
	videoq = new PacketQueue();
	videoq->packet_queue_init("videoq");
	audioq = new PacketQueue();
	audioq->packet_queue_init("audioq");

	if (!videoq || !audioq)
	{
		ErrorL << "packet_queue_init failed";
		goto fail;;
	}

	// 初始化解码后的帧队列
	pictq = new FrameQueue();
	pictq->frame_queue_init(videoq, VIDEO_PICTURE_QUEUE_SIZE, 1);
	sampq = new FrameQueue();
	sampq->frame_queue_init(audioq, SAMPLE_QUEUE_SIZE, 0);

	if (!pictq || !sampq)
	{
		ErrorL << "packet_queue_init failed";
		goto fail;;
	}

	/*
	* 初始化时钟
	* 时钟序列->queue_serial，实际上指向的是audioq->serial
	*/
	audclk = new Clock(this);
	audclk->init_clock(&audioq->serial_, "audclk");
	vidclk = new Clock(this);
	vidclk->init_clock(&videoq->serial_, "vidclk");
	extclk = new Clock(this);
	extclk->init_clock(NULL, "extclk");
	if (!audclk || !vidclk || !extclk)
	{
		ErrorL << "init_clock failed";
		goto fail;;
	}
	audio_clock_serial = -1;


	// 创建状态变量，这里主要用于seek以及数据队列缓冲区已满
	if (!(continue_read_thread = SDL_CreateCond()))
	{
		ErrorL <<  "SDL_CreateCond(): " << SDL_GetError();
		goto fail;
	}

	/* 创建读线程 */
	read_tid = SDL_CreateThread(sReadThread, "read_thread", this);
	if (!read_tid)
	{
		ErrorL << "SDL_CreateThread(): " << SDL_GetError();
	fail:
		stream_close();
		return false;
	}
	return true;
}

void MediaPlayer::stream_close()
{
	/* XXX: use a special url_shutdown call to abort parse cleanly */
	abort_request = 1;                      // 请求退出
	SDL_WaitThread(read_tid, NULL);       // 等待数据读取线程退出
	read_tid = NULL;
	abort_request = 0;
		/* close each stream */
	if (audio_stream >= 0)
	{
		stream_component_close(audio_stream);
	}

	if (video_stream >= 0)
	{
		stream_component_close(video_stream);
	}

	avformat_close_input(&format_context_);

	// 释放时钟
	if (audclk)
	{
		delete audclk;
		audclk = nullptr;
	}
	if (vidclk)
	{
		delete vidclk;
		vidclk = nullptr;
	}
	if (extclk)
	{
		delete extclk;
		extclk = nullptr;
	}
	/* free all pictures */
	if (pictq)
	{
		pictq->frame_queue_destory();
		delete pictq;
		pictq = nullptr;
	}
	if (sampq)
	{
		sampq->frame_queue_destory();
		delete sampq;
		sampq = nullptr;
	}
	SDL_DestroyCond(continue_read_thread);
	continue_read_thread = NULL;
	if (audioq)
	{
		audioq->packet_queue_destroy();
		delete audioq;
		audioq = nullptr;
	}
	if(videoq)
	{ 
		videoq->packet_queue_destroy();
		delete videoq;
		videoq = nullptr;
	}
	//display_deinit(is);     // 显示释放资源 darren
	if (demxer_)
	{
		delete demxer_;
		demxer_ = NULL;
	}
	seek_req = 0;
	eof = false;
	player_state_ = PLAYER_ST_STOPED;
}

int MediaPlayer::stream_component_open(int stream_index)
{
	int ret;
	AVCodecContext *avctx;
	AVCodec *codec;
	int sample_rate, nb_channels;
	int64_t channel_layout;

	// 分配解码器上下文内存，使用avcodec_free_context来释放
	avctx = avcodec_alloc_context3(NULL);
	if (!avctx)
	{
		ErrorL << "[error] avcodec_alloc_context3: " << av_get_err(ret);
		goto failed;
	}
	// 将码流中的编解码器信息AVCodecParameters拷贝到AVCodecContex
	ret = avcodec_parameters_to_context(avctx, format_context_->streams[stream_index]->codecpar);
	if (ret < 0)
	{
		ErrorL << "[error] avcodec_parameters_to_context: " << av_get_err(ret);
		goto failed;
	}
	// 查找解码器
	codec = avcodec_find_decoder(avctx->codec_id);
	if (codec == NULL)
	{
		ErrorL << "[error] Video Codec not found";
		goto failed;
	}
	// 打开解码器
	ret = avcodec_open2(avctx, codec, NULL);
	if (ret < 0)
	{
		ErrorL << "[error]avcodec_open2 failed: " << av_get_err(ret);
		goto failed;
	}
	switch (avctx->codec_type)
	{
	case AVMEDIA_TYPE_AUDIO:
		audio_st = format_context_->streams[audio_stream];
		sample_rate = avctx->sample_rate;
		nb_channels = avctx->channels;
		channel_layout = avctx->channel_layout;
		audio_tgt = (AudioParams *)av_mallocz(sizeof(AudioParams));
		audio_src = (AudioParams *)av_mallocz(sizeof(AudioParams));
		audio_buf_index = 0;
		if (!auddec)
		{
			auddec = new Decoder();
		}
		auddec->decoder_init(avctx, audioq, continue_read_thread);

		if ((ret = auddec->decoder_start(audio_thread, this)) < 0)
			goto out;

		audio_buf_size = 0;
		audio_buf_index = 0;
		if (!audio_output_)
		{
			audio_output_ = new AudioOutput(this);
		}
		if ((ret = audio_output_->audio_open(channel_layout, nb_channels, sample_rate, audio_tgt)) < 0)
			goto failed;
		*(audio_src) = *(audio_tgt);
		
		break;
	case AVMEDIA_TYPE_VIDEO:
		video_st = format_context_->streams[video_stream];
		if (!viddec)
		{
			viddec = new Decoder();
		}
		viddec->decoder_init(avctx, videoq, continue_read_thread);
		frame_rate = video_st->avg_frame_rate.num / video_st->avg_frame_rate.den;
		screen_width = viddec->avctx_->width;
		screen_height = viddec->avctx_->height;
		if ((ret = viddec->decoder_start(video_thread, this)) < 0)
			goto out;
		if (!video_output_)
		{
			video_output_ = new VideoOutput(this);
		}
		if (!video_output_->video_open())
			goto out;
		break;
	}
	return 0;
failed:
out:
	return -1;
}

void MediaPlayer::stream_component_close(int stream_index)
{
	AVCodecParameters *codecpar;

	if (stream_index < 0 || stream_index >= format_context_->nb_streams)
		return;
	codecpar = format_context_->streams[stream_index]->codecpar;

	switch (codecpar->codec_type) {
	case AVMEDIA_TYPE_AUDIO:
		if (audio_output_)
		{
			audio_output_->audio_stop();		// 初始化audio输出设备
			delete audio_output_;
			audio_output_ = NULL;
		}
		auddec->decoder_abort(sampq);
		auddec->decoder_destroy();
		delete auddec;
		auddec = nullptr;
		swr_free(&swr_ctx);
		av_freep(&audio_buf1);      // 释放buf1 音频格式转换的
		audio_buf1_size = 0;
		av_freep(&audio_buf2);      // 释放buf2 变速后的
		audio_buf2_size = 0;
		audio_buf = NULL;
		break;
	case AVMEDIA_TYPE_VIDEO:
		if (video_output_)
		{
			video_output_->video_stop();
			delete video_output_;
			video_output_ = nullptr;
		}
		viddec->decoder_abort(pictq);
		viddec->decoder_destroy();
		delete viddec;
		viddec = nullptr;
		break;
	default:
		break;
	}

	format_context_->streams[stream_index]->discard = AVDISCARD_ALL;
	switch (codecpar->codec_type) {
	case AVMEDIA_TYPE_AUDIO:
		audio_st = NULL;
		audio_stream = -1;
		break;
	case AVMEDIA_TYPE_VIDEO:
		video_st = NULL;
		video_stream = -1;
		break;
	default:
		break;
	}
}

void MediaPlayer::stream_seek(int64_t pos, int64_t rel, int seek_by_bytes)
{
	if (!seek_req) 
	{
		DebugL << "stream_seek";
		seek_pos = pos; // 微秒 1727,118,162
		seek_rel = rel;
		seek_flags = 0;
		seek_flags &= ~AVSEEK_FLAG_BYTE;
		if (seek_by_bytes)
			seek_flags |= AVSEEK_FLAG_BYTE;
		seek_req = 1;       // 请求seek
		SDL_CondSignal(continue_read_thread);
	}
}

int MediaPlayer::read_thread()
{
	SDL_mutex *wait_mutex = SDL_CreateMutex();

	if (!wait_mutex) 
	{
		ErrorL << "SDL_CreateMutex(): " << SDL_GetError();
		return -1;
	}

	demxer_ = new Demuxer(this);
	demxer_->Open(filename);
	// 视频解码器
	if (video_stream >= 0)
	{
		if (stream_component_open(video_stream) < 0)
		{
			ErrorL << "stream_component_open video failed";
		}
	}

	// 音频解码器
	if (audio_stream >= 0)
	{
		if (stream_component_open(audio_stream) < 0)
		{
			ErrorL << "stream_component_open video failed";
		}
	}

	// 输出文件信息
// 	LogDebug( "-------------File Information-------------\n");
	//av_dump_format(format_context_, 0, filename, 0);
// 	LogDebug( "------------------------------------------\n");

	UpdateDuration(format_context_);

	AVPacket	pkt1;
	AVPacket	*packet = &pkt1;
	int			ret;

	av_init_packet(packet);
	DebugL << "开始读取数据...";

	// 回调开始播放
	event_cb_(PLAYER_EVENT_PLAYBACK_START, NULL, event_data_);
	player_state_ = PLAYER_ST_PLAYING;
	for (;;)
	{
		if (abort_request)
		{
			DebugL << "quit, size = " << audioq->size_ + videoq->size_;
			break;
		}

		if (paused != last_paused) {
			last_paused = paused;
			if (paused)
				read_pause_return = av_read_pause(format_context_);  // 针对网络流
			else
				av_read_play(format_context_);
		}

		if (seek_req) 
		{	// 是否有seek请求
			int64_t seek_target = seek_pos;     // 目标点
			int64_t seek_min = seek_rel > 0 ? seek_target - seek_rel + 2 : INT64_MIN;
			int64_t seek_max = seek_rel < 0 ? seek_target - seek_rel - 2 : INT64_MAX;
			// FIXME the +-2 is due to rounding being not done in the correct direction in generation
			//      of the seek_pos/seek_rel variables
			// 快进 seek_target = 14 058 980 seek_min =    4 058 982 seek_max =    9223372036854775807
			// 快退 seek_target = 55 958 994 seek_min =    -9223372036854775808 seek_max =  65 958 992
			DebugL << "seek_target = " << seek_target;
			DebugL << "seek_min = " << seek_min;
			DebugL << "seek_max = " << seek_max;
			ret = avformat_seek_file(format_context_, -1, seek_min, 
				seek_target, seek_max, seek_flags);
			if (ret < 0)
			{
				ErrorL << format_context_->url << ": error while seeking";
			}
			else
			{
				/* seek的时候，要把原先的数据情况，并重启解码器，
				put flush_pkt的目的是告知解码线程需要reset decoder
				*/
				if (audio_stream >= 0) 
				{
					audioq->packet_queue_flush();
					audioq->packet_queue_put(PacketQueue::GetFlushPacket());
				}
				if (video_stream >= 0) 
				{
					videoq->packet_queue_flush();
					videoq->packet_queue_put(PacketQueue::GetFlushPacket());
				}
				if (seek_flags & AVSEEK_FLAG_BYTE) {
					extclk->set_clock(NAN, 0);
				}
				else {
					extclk->set_clock(seek_target / (double)AV_TIME_BASE, 0);
				}
			}
			seek_req = 0;
			eof = 0;    //
		}

		if (audioq->size_ + videoq->size_ > MAX_QUEUE_SIZE)
		{
			/* wait 10 ms */
			SDL_LockMutex(wait_mutex);
			// 如果没有唤醒则超时10ms退出，比如在seek操作时这里会被唤醒
			SDL_CondWaitTimeout(continue_read_thread, wait_mutex, 10);
			SDL_UnlockMutex(wait_mutex);
			continue;
		}
		if ((ret = av_read_frame(format_context_, packet)) < 0)
		{
			// 没有更多包可读
			if ((ret == AVERROR_EOF || avio_feof(format_context_->pb)) && !eof)
			{
				DebugL << "push null packet";
				videoq->packet_queue_put_nullpacket(video_stream);
				audioq->packet_queue_put_nullpacket(audio_stream);
				eof = true;            // 文件读取结束
			}
			av_usleep(10 * 1000);
			continue;
		}

		// Is this a packet from the video stream?.
		if (packet->stream_index == video_stream)
		{
// 			DebugL << "videoq";
			videoq->packet_queue_put(packet);
		}
		else if (packet->stream_index == audio_stream)
		{
// 			DebugL << "audioq";
			audioq->packet_queue_put(packet);
		}
		else
		{
			av_packet_unref(packet);
		}
	}
// 	av_packet_free(&packet);
	return 0;
failed:
	// 请求关闭程序
	if (format_context_)
		avformat_close_input(&format_context_);

	DebugL  << "read_thread finish";
	return 0;
}

int MediaPlayer::event_loop()
{
	PlayerConmandMessage *msg = nullptr;
	while (true)
	{
		if (abort_app)
		{
			break;
		}
		if (cmd_queue_->Size() > 0)
		{
			// 读取事件进行解析
			cmd_queue_->Pop(msg);
			switch (msg->cmd)
			{
			case PLAYER_CMD_PLAY:
			{
				PlayerPlayCmd *cmd = (PlayerPlayCmd *)msg->parm;
				stream_open(cmd->url_.c_str());
			}
			break;
			case PLAYER_CMD_STOP:
			{
				player_state_ = PLAYER_ST_STOPED;
				PlayerStopCmd *cmd = (PlayerStopCmd *)msg->parm;
				stream_close();
				event_cb_(PLAYER_EVENT_PLAYBACK_STOPED, NULL, event_data_);
			}
			break;
			case PLAYER_CMD_SEEK:
			{
				PlayerSeekCmd *cmd = (PlayerSeekCmd *)msg->parm;
				double ts;
				ts = format_context_->duration * cmd->percent;
				stream_seek(ts, 0, 0);
			}
			break;
			default:
				break;
			}
			delete msg;	// 释放内存
		}
		else
		{
			av_usleep(20000);
		}
		UpdateProgress();
		checkPlayFinish();
	}
	return 0;
}

void MediaPlayer::stream_toggle_pause()
{
	if (paused) 
	{
		/* 恢复暂停状态时也需要恢复时钟，需要更新vidclk */
		frame_timer += av_gettime_relative() / 1000000.0 - vidclk->last_updated_;
		if (read_pause_return != AVERROR(ENOSYS)) {
			vidclk->paused_ = 0;
		}
		vidclk->set_clock(vidclk->get_clock(), vidclk->serial_);
	}
	extclk->set_clock(extclk->get_clock(), extclk->serial_);
	// 切换 pause/resume 两种状态
	paused = audclk->paused_ = vidclk->paused_ = extclk->paused_ = !paused;
	if (paused)
		player_state_ = PLAYER_ST_PAUSE;
	else
		player_state_ = PLAYER_ST_PLAYING;
	DebugL << "step = " << step << ", stream_toggle_pause";
}

void MediaPlayer::UpdateDuration(AVFormatContext * ic)
{
	if (ic->duration != AV_NOPTS_VALUE) 
	{
		int hours, mins, secs, us;
		int64_t duration = ic->duration + (ic->duration <= INT64_MAX - 5000 ? 5000 : 0);
		secs = duration / AV_TIME_BASE;
		player_duration_.total_seconds = secs;
		us = duration % AV_TIME_BASE;
		mins = secs / 60;
		secs %= 60;
		hours = mins / 60;
		mins %= 60;
		DebugL << hours << ":" << mins << ":" << secs;
		player_duration_.hour = hours;
		player_duration_.minute = mins;
		player_duration_.second = secs;
	}
	else 
	{
		DebugL << "N/A";
		player_duration_.hour = -1;
		player_duration_.minute = -1;
		player_duration_.second = -1;
	}
	event_cb_(PLAYER_EVENT_UPDATE_DURATION, &player_duration_, event_data_);
}

void MediaPlayer::UpdateProgress()
{
	if (player_state_ == PLAYER_ST_PLAYING)
	{
		static int64_t last_time = 0;
		int64_t cur_time = 0;
		cur_time = av_gettime_relative();		// 获取到的是微妙
		if (!last_time || (cur_time - last_time) >= 1000000) 
		{
			last_time = cur_time;
			int64_t secs = Clock::get_master_clock(this);
			player_progress_.second = secs % 60;
			player_progress_.minute = (secs / 60) % 60;
			player_progress_.hour = secs / 3600;
			if (player_progress_.second < 0 || player_progress_.second>59)
			{
				player_progress_.second = 0;
				secs = 0;
			}
			if (player_progress_.minute < 0 || player_progress_.minute>59)
			{
				player_progress_.minute = 0;
				secs = 0;
			}
			if (player_progress_.hour < 0 || player_progress_.hour>59)
			{
				player_progress_.hour = 0;
				secs = 0;
			}
			if (player_duration_.total_seconds != 0)
				player_progress_.percentage = 1.0*secs / player_duration_.total_seconds;
			else
				player_progress_.percentage = 0;
			event_cb_(PLAYER_EVENT_UPDATE_PROGRESS, &player_progress_, event_data_);
		}
	}
}

bool MediaPlayer::PlayCommand(const char * url, int64_t start_time)
{
	if (player_state_ != PLAYER_ST_STOPED)
	{
		DebugL << "player have play";
		return true;
	}
	play_cmd_.setURL(url);
	play_cmd_.setStartTime(start_time);
	PlayerConmandMessage *cmd = new PlayerConmandMessage();
	cmd->cmd = PLAYER_CMD_PLAY;
	cmd->parm = &play_cmd_;
	if(!cmd_queue_->Push(cmd))
	{ 
		ErrorL << "PlayCommand push failed";
		return false;
	}
	return true;
}

bool MediaPlayer::TogglePauseCommand()
{
	toggle_pause();
	return true;
}

bool MediaPlayer::StopCommand()
{
	PlayerConmandMessage *cmd = new PlayerConmandMessage();
	cmd->cmd = PLAYER_CMD_STOP;
	cmd->parm = &stop_cmd_;
	if (!cmd_queue_->Push(cmd))
	{
		ErrorL << "PlayCommand push failed";
		return false;
	}
	return true;
}

bool MediaPlayer::SeekCommand(double seek_percent)
{
	// 正在seek或者处于停止状态时不能seek
	if(!seek_req && player_state_ != PLAYER_ST_STOPED)
	{ 
		seek_cmd_.percent = seek_percent;
		PlayerConmandMessage *cmd = new PlayerConmandMessage();
		cmd->cmd = PLAYER_CMD_SEEK;
		cmd->parm = &seek_cmd_;
		if (!cmd_queue_->Push(cmd))
		{
			ErrorL << "SeekCommand push failed";
			return false;
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool MediaPlayer::AccelerateCommand()
{
	if (speed_ < 2.0)
	{
		speed_ += 0.5;
	}
	return true;
}

bool MediaPlayer::DecelerateCommand()
{
	if (speed_ > 0.5)
	{
		speed_ -= 0.5;
	}
	return true;
}

bool MediaPlayer::AdjustVolumeCommand(float ratio)
{
	audio_volume = 100 * ratio;
	return true;
}

float MediaPlayer::GetCurrentPlaySpeed()
{
	return speed_;
}

PlayerState MediaPlayer::GetPlayerState()
{
	return player_state_;
}

bool MediaPlayer::IsStoped()
{
	return player_state_ == PLAYER_ST_STOPED;
}

bool MediaPlayer::IsPaused()
{
	return player_state_ == PLAYER_ST_PAUSE;
}
