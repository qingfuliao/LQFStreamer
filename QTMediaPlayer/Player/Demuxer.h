#pragma once
#ifdef __cplusplus
extern "C"
{
#include "libavformat/avformat.h"
#include "SDL.h"
}
#endif

class MediaPlayer;
class Demuxer
{
public:
	Demuxer(MediaPlayer *player);
	int Open(const char *url);
	virtual ~Demuxer();
private:
	AVFormatContext *ic = NULL;
	MediaPlayer *player_;
};

