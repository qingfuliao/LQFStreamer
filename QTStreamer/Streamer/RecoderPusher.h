#if 1
#ifndef __RECODER_PUSHER_H__
#define __RECODER_PUSHER_H__
#include "Media.h"
#include "MP4Muxer.h"
#include "Pusher.h"
#include "LQFRingBuffer.h"
#include "Thread.h"

typedef struct _ReordConfig
{
	std::string file_name;
	int audio_samplreate;
	int video_fps;
	int width;
	int height;
	int muxer_type;
}ReordConfig;
class RecoderPusher :public Pusher
{
public:
	static const int MP4_MUXER = 0;
public:

	RecoderPusher(ReordConfig &record_config);

	virtual ~RecoderPusher();
	bool StartRecod(void);
	bool StopRecod(void);
	void Run();

private:
	int muxer_type_;
	std::shared_ptr<MP4Muxer> mp4_muxer_ = nullptr;
	ReordConfig record_config_;
};

#endif // !__RECODER_H__
#endif