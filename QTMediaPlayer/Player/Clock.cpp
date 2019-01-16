#include "Clock.h"

#include "MediaPlayer.h"
#include "PacketQueue.h"

Clock::Clock(MediaPlayer *player)
	:player_(player)
{
}


Clock::~Clock()
{
}
/**
* 获取到的实际上是:最后一帧的pts 加上 从处理最后一帧开始到现在的时间,具体参考set_clock_at 和get_clock的代码
* pts_drift_=最后一帧的pts-从处理最后一帧时间
* clock=pts_drift_+现在的时候
* get_clock(&player_->vidclk) ==player_->vidclk.pts, av_gettime_relative() / 1000000.0 -player_->vidclk.last_updated  +player_->vidclk.pts
*/
double Clock::get_clock()
{
	if (*queue_serial_ != serial_)
	{
		return(NAN);
	}
	if (paused_)
	{
		return pts_;
	}
	/* 如果当前正处在播放状态，则返回的时间为最新的pts + 更新pts之后流逝的时间 */
	double time = av_gettime_relative() / 1000000.0;

	return(pts_drift_ + time - (time - last_updated_) * (1.0 - speed_));
}

void Clock::set_clock_at(double pts, int serial, double time)
{
	pts_ = pts;                      /* 当前帧的pts */
	pts_drift_ = pts_ - time;        /* 当前帧pts和系统时间的差值，正常播放情况下两者的差值应该是比较固定的，因为两者都是以时间为基准进行线性增长 */
	last_updated_ = time;
	serial_ = serial;                     // 起源于packet serial
}

void Clock::set_clock(double pts, int serial)
{
	double time = av_gettime_relative() / 1000000.0;
	set_clock_at(pts, serial, time); /* 修正外部时钟 */
}

void Clock::set_clock_speed(double speed)
{
	set_clock(get_clock(), serial_);
	speed_ = speed;
}

int Clock::get_master_sync_type(MediaPlayer *player)
{
	if (player->av_sync_type == AV_SYNC_VIDEO_MASTER) {
		if (player->video_st)
			return AV_SYNC_VIDEO_MASTER;
		else
			return AV_SYNC_AUDIO_MASTER;	 /* 如果没有视频成分则使用 audio master */
	}
	else if (player->av_sync_type == AV_SYNC_AUDIO_MASTER) {
		if (player->audio_st)
			return AV_SYNC_AUDIO_MASTER;
		else
			return AV_SYNC_EXTERNAL_CLOCK;	 /* 没有音频的时候那就用外部时钟 */
	}
	else {
		return AV_SYNC_EXTERNAL_CLOCK;
	}
}

double Clock::get_master_clock(MediaPlayer *player)
{
	double val;

	switch (get_master_sync_type(player)) 
	{
	case AV_SYNC_VIDEO_MASTER:
		val = player->vidclk->get_clock();
		break;
	case AV_SYNC_AUDIO_MASTER:
		val = player->audclk->get_clock();
		break;
	default:
		val = player->extclk->get_clock();
		break;
	}
	return val;
}

void Clock::set_master_sync_type(MediaPlayer *player, int sync_type)
{
	player->av_sync_type = sync_type;
}

void Clock::check_external_clock_speed(MediaPlayer *player)
{
	if (player->video_stream >= 0 && player->videoq->nb_packets_ <= EXTERNAL_CLOCK_MIN_FRAMES ||
		player->audio_stream >= 0 && player->audioq->nb_packets_ <= EXTERNAL_CLOCK_MIN_FRAMES)
	{
		player->extclk->set_clock_speed(FFMAX(EXTERNAL_CLOCK_SPEED_MIN, player->extclk->speed_ - EXTERNAL_CLOCK_SPEED_STEP));
	}
	else if ((player->video_stream < 0 || player->videoq->nb_packets_ > EXTERNAL_CLOCK_MAX_FRAMES) &&
		(player->audio_stream < 0 || player->audioq->nb_packets_ > EXTERNAL_CLOCK_MAX_FRAMES)) {
		player->extclk->set_clock_speed(FFMIN(EXTERNAL_CLOCK_SPEED_MAX, player->extclk->speed_ + EXTERNAL_CLOCK_SPEED_STEP));
	}
	else {
		double speed = player->extclk->speed_;
		if (speed != 1.0)
			player->extclk->set_clock_speed(speed + EXTERNAL_CLOCK_SPEED_STEP * (1.0 - speed) / fabs(1.0 - speed));
	}
}

void Clock::sync_clock_to_slave(Clock * slave)
{
	double	clock = get_clock();                                                               /* 外部时钟的时间 */
	double	slave_clock = slave->get_clock();                                                           /* slave时钟的时间 */
	if (!isnan(slave_clock) && (isnan(clock) || fabs(clock - slave_clock) > AV_NOSYNC_THRESHOLD))   /* 超过最大no sync */
	{
		set_clock(slave_clock, slave->serial_);
	}
}

void Clock::init_clock(int * queue_serial, const char * name)
{
	speed_ = 1.0;
	if (queue_serial)
		queue_serial_ = queue_serial;
	else
		queue_serial_ = &(serial_);
	paused_ = 0;
	if (name)
	{
		strcpy(name_, name);
	}
	set_clock( NAN, -1);
}

