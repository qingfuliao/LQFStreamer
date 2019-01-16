#pragma once

#ifdef __cplusplus
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "SDL.h"
#include "libavutil/imgutils.h"
#include "libavutil/time.h"
#include "libavutil/common.h"
}
#endif

/* no AV sync correction is done if below the minimum AV sync threshold */
#define AV_SYNC_THRESHOLD_MIN 0.01              /* 40ms */
/* AV sync correction is done if above the maximum AV sync threshold */
#define AV_SYNC_THRESHOLD_MAX 0.1               /* 100ms */
/* If a frame duration is longer than this, it will not be duplicated to compensate AV sync */
#define AV_SYNC_FRAMEDUP_THRESHOLD 0.1
/* no AV correction is done if too big error */
#define AV_NOSYNC_THRESHOLD 10.0                /* AV严重不同步时 */

/* external clock speed adjustment constants for realtime sources based on buffer fullness */
#define EXTERNAL_CLOCK_SPEED_MIN  0.900
#define EXTERNAL_CLOCK_SPEED_MAX  1.010
#define EXTERNAL_CLOCK_SPEED_STEP 0.001

/**
*音视频同步方式，缺省以音频为基准
*/
enum {
	AV_SYNC_AUDIO_MASTER,                   // 以音频为基准
	AV_SYNC_VIDEO_MASTER,                   // 以视频为基准
	AV_SYNC_EXTERNAL_CLOCK,                 // 以外部时钟为基准，synchronize to an external clock */
};

class MediaPlayer;
class Clock
{
public:
	Clock(MediaPlayer *player);
	virtual ~Clock();

	double get_clock();
	void set_clock_at(double pts, int serial, double time);
	void set_clock(double pts, int serial);
	void init_clock(int *queue_serial, const char *name);
	void set_clock_speed(double speed);
	static int get_master_sync_type(MediaPlayer *player);
	static double get_master_clock(MediaPlayer *player);
	static void set_master_sync_type(MediaPlayer *player, int sync_type);
	static void check_external_clock_speed(MediaPlayer *player);
	void sync_clock_to_slave(Clock *slave);


	double	pts_;                        // clock base 时钟基础
	int	paused_;                         // = 1 说明是暂停状态
	double	last_updated_;               // 最后一次更新的系统时钟
	double	speed_;                      // 这里的speed主要用于外部时钟同步
										// 相对时钟的讲解，需要讲透彻
	double	pts_drift_;                  // clock base minus time at which we updated the clock，当前pts与当前系统时钟的差值
										// serial的作用
	int	serial_;                         // clock is based on a packet with this serial
										// *queue_serial的作用，实际指向的是packet queue 的serial
	int	*queue_serial_;                  // pointer to the current packet queue serial, used for obsolete clock detection
	char	name_[20];                   // 存储时钟名称以方便debug */

private:
	MediaPlayer *player_;
};

