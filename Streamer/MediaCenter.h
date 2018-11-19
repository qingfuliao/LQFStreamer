#pragma once
#include <iostream>
#include "AACEncoder.h"
#include "AudioCapture.h"
#include "X264Encoder.h"
#include "VideoCapture.h"
#include "RtmpPusher.h"
#include "RecoderPusher.h"
#include "Media.h"
using namespace std;


// 应用中心
class MediaCenter
{
public:
	MediaCenter();
	virtual ~MediaCenter();
	uint32_t VideoGetTimeStamp();
	uint32_t AudioGetTimeStamp(uint32_t sampleRate);
	bool StartRtmpPush(const string url, int timeout);
	bool StopRtmpPush(void);

	bool StartRecord(string url);
	bool StopRecord(void);

	bool ConfigAudioEncoder(AudioEncoderConfig &config);	// 配置audio编码器参数
	void GetAudioEncoderConfig(AudioEncoderConfig &config);	// 获取audio编码器参数
	bool ConfigVideoEncoder(VideoEncoderConfig &config);//配置video编码器参数
	void GetVideoEncoderConfig(VideoEncoderConfig &config);// 获取video编码器参数

														   // 设置捕获参数
	void ConfigAudioCapture(AudioCaptureConfig &config);
	void ConfigVideoCapture(VideoCaptureConfig &config);

	void InitAVTimer();
	bool StartAudio();			// 启动audio源
	bool StopAudio();			// 停止audio源
	bool StartVideo();			// 启动video源
	bool StopVideo();			// 停止video源
private:
	friend void TriggerEncodeVideoCallback(void *user)
	{
		MediaCenter *media_center = (MediaCenter *)user;
		media_center->TriggerEncodeVideo();
	}
	void TriggerEncodeVideo();
	void WaitTriggerEncodeVideo();
	void PushAudio();
	void PushVideo();
	bool pushVideoMetadata();	// 发送video的sps pps sei等

	// 配置参数
	AudioEncoderConfig audio_encoder_config_;
	VideoEncoderConfig video_encoder_config_;
	AudioCaptureConfig audio_capture_config_;
	VideoCaptureConfig video_capture_config_;
	// 
	std::shared_ptr<std::thread> video_thread_;
	std::shared_ptr<std::thread> audio_thread_;
	bool vid_thread_req_abort_ = false;
	bool aud_thread_req_abort_ = false;
	// 视频信息
	std::string sps_;
	std::string pps_;
	std::string sei_;

	std::shared_ptr<AudioCapture> audio_capture_ = nullptr;
	std::shared_ptr<VideoCapture> video_capture_ = nullptr;

	std::shared_ptr<AACEncoder>  audio_encoder_ = nullptr;
	std::shared_ptr<X264Encoder> video_encoder_ = nullptr;
	std::mutex mutex_vid_enc_trigger_;
	std::condition_variable cv_vid_enc_trigger_; //条件变量

	// RTMP
	std::shared_ptr<RtmpPusher> rtmp_pusher_ = nullptr;
	string rtmp_push_url_;		// 推流链接
	bool rtmp_push_enable_ = false;	

	// Recoder
	std::shared_ptr<RecoderPusher> recod_pusher_ = nullptr;
	string recod_push_url_;		// 推流链接
	bool record_push_enable_ = false;			// 是否推流中
};