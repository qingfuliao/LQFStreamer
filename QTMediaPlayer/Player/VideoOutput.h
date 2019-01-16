#pragma once

#ifdef __cplusplus
extern "C"
{
#include "SDL.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
}
#endif
class MediaPlayer;
struct Frame;

class VideoOutput
{
public:
	VideoOutput(MediaPlayer *player);
	~VideoOutput();
	bool video_open();
	bool video_stop();
	int refreshThread();
	double vp_duration(Frame *vp, Frame *nextvp);
	void video_refresh(double *remaining_time);
	void video_display();
	void video_image_display();
	MediaPlayer *player_ = nullptr;
	bool is_first_frame_ = true;
	bool abort_request = false;
	SDL_Thread   *refresh_thread_tid_ = nullptr;
	SwsContext *convert_ctx = nullptr;
	AVPicture picture;
	bool is_pic_init_ = false;
	int width_;
	int height_;
};

