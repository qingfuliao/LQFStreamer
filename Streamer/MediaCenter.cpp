#include <libyuv.h>

#include "MediaCenter.h"
#include "LogUtil.h"
#include "RtmpPusher.h"
#include "Timer.h"
#include "TimeUtil.h"
#include "TimerManager.h"

using namespace LQF;


MediaCenter::MediaCenter()
{
}

MediaCenter::~MediaCenter()
{
}

uint32_t MediaCenter::VideoGetTimeStamp()
{
	auto timePoint = chrono::time_point_cast<chrono::microseconds>(chrono::high_resolution_clock::now());
	return (uint32_t)((timePoint.time_since_epoch().count() + 500) / 1000 * 90);
}

uint32_t MediaCenter::AudioGetTimeStamp(uint32_t sampleRate)
{
	auto timePoint = chrono::time_point_cast<chrono::microseconds>(chrono::high_resolution_clock::now());
	return (uint32_t)((timePoint.time_since_epoch().count() + 500) / 1000 * sampleRate / 1000);
}
bool MediaCenter::StartRtmpPush(const string url, int timeout)
{
	FunEntry();
	rtmp_pusher_.reset(new RtmpPusher());
	rtmp_pusher_->SetAudioConfig(RtmpPusher::AAC_PROFILE_LC,
		audio_encoder_config_.sample_rate,
		audio_encoder_config_.channels);
	if (!rtmp_pusher_->Connect(url.c_str(), timeout))
	{
		rtmp_pusher_.reset();
		LogError("rtmp connect failed");
		FunExit();
		return false;
	}
	FunExit();
	return true;
}

bool MediaCenter::StopRtmpPush(void)
{
	FunEntry();
	if (rtmp_pusher_)
	{
		rtmp_pusher_.reset();
	}
	FunExit();
	return true;
}

bool MediaCenter::StartRecord(string url)
{
	FunEntry();
	ReordConfig record_config;
	record_config.audio_samplreate = audio_encoder_config_.sample_rate;
	record_config.file_name = url;
	record_config.muxer_type = RecoderPusher::MP4_MUXER;
	record_config.width = video_encoder_config_.width;
	record_config.height = video_encoder_config_.height;
	record_config.video_fps = video_encoder_config_.fps;
	recod_pusher_.reset(new RecoderPusher(record_config));
	if (!recod_pusher_->StartRecod())
	{
		LogError("StartRecod failed");
	}

	FunExit();
	return true;
}

bool MediaCenter::StopRecord(void)
{
	FunEntry();
	if (recod_pusher_)
	{
		recod_pusher_.reset();
	}
	FunExit();
	return true;
}

bool MediaCenter::ConfigAudioEncoder(AudioEncoderConfig & config)
{
	audio_encoder_config_ = config;
	return true;
}

void MediaCenter::GetAudioEncoderConfig(AudioEncoderConfig & config)
{
	config = audio_encoder_config_;
}

bool MediaCenter::ConfigVideoEncoder(VideoEncoderConfig & config)
{
	video_encoder_config_ = config;
	return true;
}

void MediaCenter::GetVideoEncoderConfig(VideoEncoderConfig & config)
{
	config = video_encoder_config_;
}

void MediaCenter::ConfigAudioCapture(AudioCaptureConfig & config)
{

}

void MediaCenter::ConfigVideoCapture(VideoCaptureConfig & config)
{

}

void MediaCenter::InitAVTimer()
{
	AVTimer::GetInstance()->Reset();
}

bool MediaCenter::StartAudio()
{
	audio_capture_.reset(new AudioCapture());

	if (audio_capture_->Init(audio_encoder_config_.sample_rate, audio_encoder_config_.channels))
	{
		audio_encoder_.reset(new AACEncoder());

		if (audio_encoder_->Init(audio_encoder_config_.sample_rate,
			audio_encoder_config_.channels,
			audio_encoder_config_.bitrate, PROFILE_AAC_LC) < 0)
		{
			audio_encoder_.reset();
			audio_capture_.reset();

			return false;
		}
	}

	if (audio_capture_->Start())
	{
		if (audio_capture_->IsCapturing())
		{
			aud_thread_req_abort_ = false;
			audio_thread_.reset(new std::thread(&MediaCenter::PushAudio, this));
		}
	}

	return true;
}

bool MediaCenter::StopAudio()
{
	if (audio_capture_)
	{
		if (audio_capture_->IsCapturing())
		{
			aud_thread_req_abort_ = true;
			audio_thread_->join();
		}
		// 等待线程退出后再停止捕获
		audio_capture_.reset();
		audio_thread_.reset();
	}
	if (audio_encoder_)
	{
		audio_encoder_.reset();
	}

	return true;
}

bool MediaCenter::StartVideo()
{
	video_capture_.reset(new VideoCapture());
	if (!video_capture_)
	{
		return false;
	}
	if (video_capture_->Init(video_encoder_config_.fps))
	{
		video_encoder_config_.width = video_capture_->Width();
		video_encoder_config_.height = video_capture_->Height();
		video_encoder_.reset(new X264Encoder());
		if (!video_encoder_)
		{
			video_capture_.reset();
			return false;
		}
		video_encoder_->Init(video_encoder_config_);
	}
	else
	{
		video_capture_.reset();
		return false;
	}
	if (video_capture_->Start())
	{
		if (video_capture_->IsCapturing())
		{
			vid_thread_req_abort_ = false;
			video_thread_.reset(new std::thread(&MediaCenter::PushVideo, this));
		}
	}
	else
	{
		video_capture_.reset();
		video_encoder_.reset();
		return false;
	}

	return true;
}

bool MediaCenter::StopVideo()
{
	if (video_capture_)
	{
		if (video_capture_->IsCapturing())
		{
			vid_thread_req_abort_ = true;
			video_thread_->join();
		}
		// 等待线程退出后再停止捕获
		video_capture_.reset();
		video_thread_.reset();
	}
	if (video_encoder_)
	{
		video_encoder_.reset();
	}

	return true;
}

void MediaCenter::TriggerEncodeVideo()
{
	std::unique_lock<std::mutex> lock(mutex_vid_enc_trigger_);
	cv_vid_enc_trigger_.notify_one();
}

void MediaCenter::WaitTriggerEncodeVideo()
{
	std::unique_lock<std::mutex> lock(mutex_vid_enc_trigger_);
	cv_vid_enc_trigger_.wait(lock);
}

void MediaCenter::PushAudio()
{
	unsigned char enc_data[8192];
	int enc_len = 0;
	while (true)
	{
		if (aud_thread_req_abort_)
		{
			break;
		}
		PCMFrame frame(0);
		if (audio_capture_->GetFrame(frame))
		{
			enc_len = 8192;
			audio_encoder_->Encode((const uint8_t *)frame.data.get(),
				frame.size, enc_data,
				enc_len);

			if (enc_len > 0)
			{
				LQF::AVPacket audio_packet(enc_len);
				audio_packet.timestamp = (uint32_t)frame.timpestamp;
				audio_packet.type = AUDIO_PACKET;
				memcpy(audio_packet.buffer.get(), enc_data, enc_len);
				if (rtmp_pusher_)
				{
					if (!rtmp_pusher_->PushPacket(audio_packet))
					{
						LogError("rtmp push audio frame failed");
					}
				}
				if (recod_pusher_)
				{
					if (!recod_pusher_->PushPacket(audio_packet))
					{
						LogError("record push audio packet failed");
					}
				}
			}
		}
		else
		{
			Sleep(10);
		}
	}
	aud_thread_req_abort_ = false;
}

void MediaCenter::PushVideo()
{
	LogDebug("into");

	int width = video_encoder_config_.width;
	int height = video_encoder_config_.height;
	int luma_size = width * height;
	int chroma_size = luma_size/4;

	uint8_t *y_buf = (uint8_t *)malloc(luma_size);
	uint8_t *u_buf = (uint8_t *)malloc(chroma_size);
	uint8_t *v_buf = (uint8_t *)malloc(chroma_size);

	MediaFrame av_frame;
	memset(&av_frame, 0, sizeof(MediaFrame));
	MediaPacket packet;
	memset(&packet, 0, sizeof(MediaPacket));
	packet.data = (uint8_t *)malloc(1024 * 1024);	// 暂且分配为1M
	int i_frame = 0;
	LogDebug("vid_thread_req_abort_ = %d", this->vid_thread_req_abort_);
	uint32_t frame_count = 1;
	int bitrate = 900;
	int64_t start_time = TimeUtil::GetNowTime();
	int64_t current_time = TimeUtil::GetNowTime();
	int size_clac = 0;
	int fps = 0;

	LQF::Timer trigger_timer(*LQF::TimerManager::GetInstance());
	trigger_timer.Start(TriggerEncodeVideoCallback, this, 1000 / video_encoder_config_.fps);

	bool b_trigger_sps_pps = true;		// 首帧 或者改变编码参数时触发

	while (true)
	{
		if (vid_thread_req_abort_)
		{
			break;
		}
		int64_t time = TimeUtil::GetNowTime();
		RGBAFrame frame(0);
		if (video_capture_->GetFrame(frame))	
		{
			// 确定是否需要插入sps/pps/sei
			if (b_trigger_sps_pps)
			{
				if(pushVideoMetadata())
					b_trigger_sps_pps = false;		// 每次
			}
#if 0
			if (++frame_count % 500 == 0)
			{
				LogDebug("change video bitrate to %dkbps", bitrate);
				video_encoder_->SetBitrate(bitrate);
				bitrate += 100;
				if (bitrate > 2000)
				{
					bitrate = 900;
				}
			}
#endif
			//i_frame++;
			libyuv::ARGBToI420(frame.data.get(), width * 4,
				y_buf, width,
				u_buf, (width + 1) / 2,
				v_buf, (width + 1) / 2,
				width, height);
			av_frame.pts = (int64_t)frame.timestamp;
			av_frame.data[0] = y_buf;
			av_frame.data[1] = u_buf;
			av_frame.data[2] = v_buf;
			av_frame.linesize[0] = luma_size;// luma_size;
			av_frame.linesize[1] = chroma_size;// chroma_size;
			av_frame.linesize[2] = chroma_size;// chroma_size;
			av_frame.pict_type = MD_PICTURE_TYPE_NONE;
			int ret = video_encoder_->EncodeFrame(&packet, &av_frame);
			if (ret == H264_ENCODE_GOT_PACKET)
			{
				//LogDebug("pkt_typ = %d",packet.pkt_type);
				LQF::AVPacket video_packet(packet.size);
				video_packet.size = packet.size;
				video_packet.type = VIDEO_PACKET;				
				video_packet.timestamp = (uint32_t)packet.pts;
				memcpy(video_packet.buffer.get(), packet.data, video_packet.size);
				if (rtmp_pusher_)
				{
					if (!rtmp_pusher_->PushPacket(video_packet))
					{
						LogError("rtmp push video packet failed");
					}
				}

				if (recod_pusher_)
				{
					if (!recod_pusher_->PushPacket(video_packet))
					{
						LogError("record push video packet failed");
					}
				}

				#if 1
				fps++;
				size_clac += packet.size;
				current_time = TimeUtil::GetNowTime();
				if (current_time - start_time > 6000)
				{
					start_time = current_time;
					LogDebug("fps:%d,bitrate:%0.2fkbps", fps / 6,
						(float)(size_clac * 8 / 1024.0 / 6));
					size_clac = 0;
					fps = 0;
				}
				#endif
			}
			if ((H264_ENCODE_ERROR == ret) || (H264_ENCODE_EOF == ret))
			{
				LogDebug("X264_encoder_frame = %s  and break\n",
					video_encoder_->GetErrorString(ret));
				break;
			}
		}
		WaitTriggerEncodeVideo();
	}
	trigger_timer.Stop();
	free(y_buf);
	free(u_buf);
	free(v_buf);
	free(packet.data);
	vid_thread_req_abort_ = false;
}

bool MediaCenter::pushVideoMetadata()
{
	bool ret = true;
	video_encoder_->GetSPS(sps_);
	video_encoder_->GetPPS(pps_);
	video_encoder_->GetSEI(sei_);
	//sps
	LQF::AVPacket sps_packet(sps_.size());
	sps_packet.size = sps_.size();
	sps_packet.type = VIDEO_PACKET;
	sps_packet.timestamp = 0;
	memcpy(sps_packet.buffer.get(), sps_.c_str(), sps_.size());
	// pps
	LQF::AVPacket pps_packet(pps_.size());
	pps_packet.size = pps_.size();
	pps_packet.type = VIDEO_PACKET;
	pps_packet.timestamp = 0;
	memcpy(pps_packet.buffer.get(), pps_.c_str(), pps_.size());
	// sei
	LQF::AVPacket sei_packet(sei_.size());
	sei_packet.size = sei_.size();
	sei_packet.type = VIDEO_PACKET;
	sei_packet.timestamp = 0;
	memcpy(sei_packet.buffer.get(), sei_.c_str(), sei_.size());
	if (rtmp_pusher_)
	{
		if (!rtmp_pusher_->PushPacket(sps_packet))
		{
			LogError("rtmp push sps packet failed");
			ret = false;
		}
		if (!rtmp_pusher_->PushPacket(pps_packet))
		{
			LogError("rtmp push pps packet failed");
			ret = false;
		}
		if (!rtmp_pusher_->PushPacket(sei_packet))
		{
			LogError("rtmp push sei packet failed");
			ret = false;
		}
	}

	if (recod_pusher_)
	{
		if (!recod_pusher_->PushPacket(sps_packet))
		{
			LogError("record push sps packet failed");
			ret = false;
		}
		if (!recod_pusher_->PushPacket(pps_packet))
		{
			LogError("record push pps packet failed");
			ret = false;
		}
		if (!recod_pusher_->PushPacket(sei_packet))
		{
			LogError("record push sei packet failed");
			ret = false;
		}
	}
	return ret;
}
