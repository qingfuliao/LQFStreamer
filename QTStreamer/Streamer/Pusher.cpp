#include "Pusher.h"
#include "LogUtil.h"

static const char start_code_4bytes[4] = { 0x00, 0x00, 0x00, 0x01 };
static const char start_code_3bytes[3] = { 0x00, 0x00, 0x01 };

Pusher::Pusher(const int audio_samplerate, const int video_fps)
{
	// 暂时认为一帧的采样点数固定为1024
	audio_pkt_queue_.reset(new LQFRingBuffer<LQF::AVPacket>(1000.0 * 1024 / audio_samplerate));																					
	video_pkt_queue_.reset(new LQFRingBuffer<LQF::AVPacket>(video_fps));
}


Pusher::~Pusher()
{
	audio_pkt_queue_.reset();
	video_pkt_queue_.reset();
}

void Pusher::SetAudioConfig(int profile, int samplerate, int channel)
{
	audio_profile_ = profile;
	audio_samplerate_ = samplerate;
	audio_channel_ = channel;
}

void Pusher::Run()
{
	// noting
}

bool Pusher::PushPacket(LQF::AVPacket &pkt)
{
	if (pkt.type & AUDIO_PACKET)
	{
		return audio_pkt_queue_->Push(std::move(pkt));
	}
	else if (pkt.type & VIDEO_PACKET)
	{
		return video_pkt_queue_->Push(std::move(pkt));
	}
	else
	{
		LogError("Undefine media type = 0x%x", pkt.type);
		return false;
	}
}

int Pusher::GetAudioCachePackets() const
{
	return audio_pkt_queue_->Size();
}

int Pusher::GetVideoCachePackets() const
{
	return video_pkt_queue_->Size();
}

bool Pusher::readNaluFromBuf(const uint8_t * buffer, uint32_t nBufferSize, LQF::NaluUnit & nalu)
{
	if (nBufferSize < 4)
	{
		return false;
	}
	int start_code_len = 4;

	if (memcmp(buffer, start_code_4bytes, 4) == 0)
	{
		start_code_len = 4;
	}
	else if (memcmp(buffer, start_code_3bytes, 3) == 0)
	{
		start_code_len = 3;
	}
	else
	{
		return false;
	}

	nalu.type = buffer[start_code_len] & 0x1f;
	nalu.data = (uint8_t*)&buffer[start_code_len];
	nalu.size = nBufferSize - start_code_len;

	return true;
}
