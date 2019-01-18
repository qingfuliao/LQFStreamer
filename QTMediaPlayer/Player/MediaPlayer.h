#pragma once
#include <iostream>

#ifdef __cplusplus
extern "C"
{
#include "libavformat/avformat.h"
#include "SDL.h"
}
#endif

#include "Ringbuffer.h"
#include "PlayerInfo.h"

// 播放器核心
typedef enum player_conmand
{
	PLAYER_CMD_PLAY = 0,
	PLAYER_CMD_STOP,
	PLAYER_CMD_TOGGLE_PAUSE,
	PLAYER_CMD_TOGGLE_MUTE,
	PLAYER_CMD_SEEK,
	PLAYER_CMD_FAST_FORWARD,	// 快进ast forward
	PLAYER_CMD_FAST_BACK,		// 快退
	PLAYER_CMD_STEP_TO_NEXT_FRAME,
	PLAYER_CMD_TOGGLE_FULL_SCREEN, 
	PLAYER_CMD_UP_VOLUME,			// 增加音量
	PLAYER_CMD_DOWN_VOLUME,			// 降低音量
	PLAYER_CMD_CYCLE_CHNANEL,
	PLAYER_CMD_CYCLE_AUDIO_TRACK,	// 切换音轨
	PLAYER_CMD_CYCLE_VIDEO_TRACK,	// 切换视频轨
	PLAYER_CMD_CYCLE_SUBTITLE_TRACK,	// 切换字幕
}PlayerConmand;

typedef enum player_state
{	
	PLAYER_ST_READY_PALY,	// 准备播放中，刚下播放命令时
	PLAYER_ST_PLAYING,	// 正在播放		
	PLAYER_ST_PAUSE,	// 暂停状态
	PLAYER_ST_STOPED,	// 停止状态
}PlayerState;

typedef struct  palyer_play_cmd
{
	std::string url_;				// 播放链接
	int64_t start_time_;			// 其实时间
	void setURL(const char *url)
	{
		url_.clear();
		url_.append(url);
	}
	void setStartTime(int64_t start_time)
	{
		start_time_ = start_time;
	}
}PlayerPlayCmd;

// 目前没有实际的参数，收到命令直接结束播放
typedef struct  palyer_stop_cmd
{
	bool stop_;
}PlayerStopCmd;

typedef struct  palyer_seek_cmd
{
	double percent;
}PlayerSeekCmd;

typedef struct player_cmd_
{
	void *parm;				// 命令由内部MediaPlayer生成
	PlayerConmand cmd;
}PlayerConmandMessage;


/* Minimum SDL audio buffer size, in samples. */
#define SDL_AUDIO_MIN_BUFFER_SIZE 512
/* Calculate actual buffer size keeping in mind not cause too frequent audio callbacks */
#define SDL_AUDIO_MAX_CALLBACKS_PER_SEC 30
typedef struct AudioParams
{
	int freq;                   // 帧率
	int channels;               // 通道数
	int64_t channel_layout;     // 通道布局 例如2.0立体声/2.1立体声+低音炮/5.1家庭影院环绕立体声等等
	enum AVSampleFormat fmt;    // 采样格式
	int frame_size;             // 一个采样单元占的字节数量（一个采样单元包括所有通道的采样点）
	int bytes_per_sec;          // 一秒钟消耗的字节数量
} AudioParams;


// 前置声明
class PacketQueue;
class Decoder;
class FrameQueue;
class Demuxer;
class Clock;
class AudioOutput;
class VideoOutput;
class VideoWidget;

// e事件，param事件参数，user用户自己的指针
typedef int (*pEventCallBack) (PlayerEvent e, void *param, void *user);

class MediaPlayer
{
public:
	MediaPlayer(VideoWidget *video_dev);
	// 返回打开状态，时间信息，播放进度等等
	void RegisterEventCallback(pEventCallBack ecb, void *user);
	~MediaPlayer();
	bool PushCommandMessage(PlayerConmandMessage *cmdMsg);			
	int read_thread();
	int event_loop();		// 响应外部事件
	
	/**
	* url 播放url
	* start_time 起始时间，默认为0
	*/
	bool PlayCommand(const char *url, int64_t start_time = 0);
	bool TogglePauseCommand();		// 暂停/恢复播放
	// 停止播放
	bool StopCommand();
	// 拖动播放, 只传递百分比(根据进度条的比值)
	bool SeekCommand(double seek_percent);
	bool AccelerateCommand();	// 加速播放
	bool DecelerateCommand();	// 减速播放
	// ratio范围[0~1.0]
	bool AdjustVolumeCommand(float ratio);
	float GetCurrentPlaySpeed();
	PlayerState GetPlayerState();
	bool IsStoped();
	bool IsPaused();

	void stream_toggle_pause();

	void UpdateDuration(AVFormatContext *ic);
	void UpdateProgress();
private:
	void checkPlayFinish();							// 检查是否已经播放完毕
	void toggle_pause();
	bool stream_open(const char *url);			// 初始化资源
	void stream_close();		// 释放资源
	int  stream_component_open(int stream_index);
	void stream_component_close(int stream_index);
	void stream_seek(int64_t pos, int64_t rel, int seek_by_bytes);
	RingBuffer<PlayerConmandMessage *> *cmd_queue_;

public:
	PlayerState player_state_ = PLAYER_ST_STOPED;	// 初始为停止状态

	pEventCallBack event_cb_;
	void* event_data_;
	int64_t audio_callback_time = 0;
	Demuxer *demxer_ = nullptr;
	AVFormatContext *format_context_ = nullptr;
	int		abort_request = 0;      // =1时请求退出播放
	int		realtime = 0;           // =1为实时流
	int		read_pause_return = 0;

	// 视频相关
	AVStream    *video_st = nullptr;
	PacketQueue *videoq = nullptr;
	int         video_stream = -1;   // 视频流索引
	Decoder     *viddec = nullptr;        // 音频解码器,原来的vid_codec_ctx放到viddec
	int         frame_rate = 25;     // 帧率
	FrameQueue	*pictq = nullptr;         // 图像帧队列

								// 音频相关
	AVStream    *audio_st = nullptr;
	PacketQueue *audioq = nullptr;
	int         audio_stream = -1;   // 音频流索引
	Decoder     *auddec = nullptr;        // 视频解码器，原来的aud_codec_ctx放到auddec
	int         sample_rate = 44100;    // 采样率
	FrameQueue	*sampq = nullptr;         // PCM帧队列
								// 声音输出相关
	SDL_AudioDeviceID audio_dev;
	AudioParams *audio_tgt = nullptr;         // SDL播放音频需要的格式
	AudioParams *audio_src = nullptr;         // 解出来音频帧的格式  当audio_tgt和audio_src不同则需要重采样
	struct SwrContext *swr_ctx = nullptr;     // 音频重采样

	uint8_t *audio_buf = nullptr;             // 指向解码后的数据，它只是一个指针，实际存储解码后的数据在audio_buf1
	uint32_t audio_buf_size = 0;        // audio_buf指向数据帧的数据长度，以字节为单位
	uint32_t audio_buf_index = 0;       // audio_buf_index当前读取的位置，不能超过audio_buf_size

	uint8_t *audio_buf1 = 0;            // 存储解码后的音频数据帧，动态申请，当不能满足长度时则重新释放再分配
	uint32_t audio_buf1_size = 0;       // 存储的数据长度，以字节为单位

	uint8_t *audio_buf2 = nullptr;            // 存储变速后的音频数据帧
	uint32_t audio_buf2_size = 0;       // 存储的数据长度，以字节为单位

									// 显示相关
	int		force_refresh  = 0;      // =1时需要刷新画面，请求立即刷新画面的意思
								// SDL 这部分主要是显示相关，
	int default_width = 640;
	int default_height = 480;
	int screen_width = 0;
	int screen_height = 0;
	int	width, height, xleft, ytop;
	bool is_display_open = false;

	// 音频输出
	AudioOutput *audio_output_ = nullptr;
	// 视频输出
	VideoOutput *video_output_ = nullptr;

	// 音视频同步相关
	Clock	*audclk = nullptr;            // 音频机制时钟
	Clock	*vidclk = nullptr;             // 视频时钟
	Clock	*extclk = nullptr;             // 外部时钟
	int     audio_clock_serial = -1; // serial
	double  audio_clock = 0;        // 计算邻近PTS的值，并将该值设置给时钟
	double  video_clock = 0;        // 计算邻近PTS的值，用于和audio pts做比对
	double	frame_timer = 0;        // 用于video计算输出时间
	float speed_ = 1.0;                // 播放速度控制，默认为1.0，对音频和视频都起效
	int av_sync_type = 0;                       // 音视频同步类型
											/* maximum duration of a frame - above this, we consider the jump a timestamp discontinuity */
											// 大于该值则认为帧不连续，中间有跳帧的情况
	double		max_frame_duration;
	int			frame_drops_early;      // drop帧相关
	int			frame_drops_late;

	// 文件相关
	char filename[1024];
	bool eof = false;        // 是否已经读取结束

	SDL_Thread   *refresh_tid = nullptr;
	SDL_Thread   *read_tid = nullptr;

	bool abort_app = false;
	SDL_Thread   *event_loop_tid = nullptr;

	// 控制相关
	bool quit = false;       // = ture时程序退出
	bool paused = false;      // = ture暂停, false h恢复播放
	int	last_paused;        // 暂存“暂停”/“播放”状态
							// 快进 快退 跳转
	int		seek_req = 0;       // seek请求标志
							// AVSEEK_FLAG_BACKWARD AVSEEK_FLAG_BYTE AVSEEK_FLAG_ANY  AVSEEK_FLAG_FRAME
	int		seek_flags = 0;     // seek类型，按字节还是按时间
							// 注意seek_pos和seek_rel的最后单位，如果是按字节seek则单位为字节，如果按时间则单位为 us微妙
	int64_t	seek_pos;       // 请求播放的位置
	int64_t	seek_rel;       // 用来说明seek的偏移位置
							// 逐帧播放
	int	step = 0;
	// 音量
	int			audio_volume;                   /* 音量 */
	int			muted;                      // =1静音，=0则正常

	SDL_cond	*continue_read_thread = nullptr;
	VideoWidget *video_dev_ = nullptr;

	// 命令本体
	PlayerPlayCmd play_cmd_;
	PlayerStopCmd stop_cmd_;
	PlayerSeekCmd seek_cmd_;

	PlayerDuration player_duration_;
	PlayerProgress player_progress_ ;
};

