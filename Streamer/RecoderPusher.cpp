#include "RecoderPusher.h"

#include "LogUtil.h"
#include "Media.h"
using namespace LQF;

RecoderPusher::RecoderPusher(ReordConfig &record_config):
	record_config_(record_config),
	Pusher(record_config.audio_samplreate, record_config.video_fps)
{
}

RecoderPusher::~RecoderPusher()
{
	StopRecod();
}

bool RecoderPusher::StartRecod(void)
{
	mp4_muxer_.reset(new MP4Muxer(record_config_.file_name));
	if (!mp4_muxer_)
	{
		LogError("new MP4Muxer(file_name_) failed");
		return false;
	}
	
	if (!mp4_muxer_->Start(record_config_.width, record_config_.height, record_config_.video_fps))
	{
		LogError("mp4_muxer_ start failed");
		return false;
	}
	
	return Thread::Start();
}

bool RecoderPusher::StopRecod(void)
{
	if (mp4_muxer_)
	{
		mp4_muxer_->Stop();
		mp4_muxer_.reset();
	}
	if (Thread::IsRunning())
	{
		Thread::Stop();
	}
	return true;
}

void RecoderPusher::Run()
{
	while (true)
	{
		if (requet_abort_)
		{
			break;
		}
		AVPacket av_pkt;
		bool b_get_packet = false;

		// 发送视频包
		if (video_pkt_queue_->Pop(av_pkt))
		{
			b_get_packet = true;
			if (!mp4_muxer_->WriteH264Data((uint8_t *)av_pkt.buffer.get(), 
				av_pkt.size, av_pkt.timestamp))
			{
				LogError("WriteH264Data failed");
			}
		}

		// 发送音频包
		if (audio_pkt_queue_->Pop(av_pkt))
		{
			b_get_packet = true;
			if (!mp4_muxer_->WriteAACData((uint8_t *)av_pkt.buffer.get(),
				av_pkt.size, av_pkt.timestamp))
			{
				LogError("WriteAACData failed");
			}
		}
		if (!b_get_packet)			// 如果音视频队列都为空则休眠
			Sleep(5);
	}
}
