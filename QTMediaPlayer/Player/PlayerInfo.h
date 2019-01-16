#ifndef _PLAYER_INFO_H_
#define _PLAYER_INFO_H_

// 接收外部命令并提供回调函数
typedef enum player_event
{
	PLAYER_EVENT_UNKNOWN,			// 未知状态
	PLAYER_EVENT_PLAYBACK_START,	// 播放状态
	PLAYER_EVENT_PLAYBACK_STOPED,	// 停止状态
	PLAYER_EVENT_PLAYBACK_FINISHED,	// 单曲播放完成
	PLAYER_EVENT_PLAYLIST_FINISHED,	// 目录播放完成
	PLAYER_EVENT_PLAYBACK_PAUSE,	// 暂停状态
	PLAYER_EVENT_PLAYBACK_RESUME,	// 恢复播放状态
	PLAYER_EVENT_FRAME_BY_FRAME,	// 逐帧播放
	PLAYER_EVENT_UPDATE_PROGRESS,	// 更新当前播放状态，进度条百分比，当前时间
	PLAYER_EVENT_UPDATE_DURATION,	// 当前文件的总时长
} PlayerEvent;

// 当前播放的时间以及百分比
typedef struct player_progress
{
	int hour;	// 当前时
	int minute;	// 当前分
	int second;	// 当前秒
	float percentage;		// 百分比
}PlayerProgress;

// 音量
typedef struct player_volume
{
	int volume;			// 音量
	int percentage;		// 百分比
}PlayerVolume;

// 文件长度
typedef struct player_duration
{
	int hour;	// 时
	int minute;	// 分
	int second;	// 秒
	int64_t total_seconds;
}PlayerDuration;
#endif
