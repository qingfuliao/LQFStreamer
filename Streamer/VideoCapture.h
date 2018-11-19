#pragma once
#include <thread>
#include <iostream>
#include <cstdio>
#include <cstdint>
#include <memory>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "screencapture/ScreenCapture.h" // 第三方的库，这里直接集成库文件，但是也提供了源码
#include "RingBuffer.h"
using namespace std;


typedef struct VideoCaptureConfig_
{
	int width;
	int height;
}VideoCaptureConfig;

struct RGBAFrame
{
	RGBAFrame(uint32_t size = 100)
		: data(new uint8_t[size + 1024])
	{
		this->size = size;
	}

	uint32_t size = 0;
	uint32_t width;
	uint32_t height;
	uint64_t timestamp;
	std::shared_ptr<uint8_t> data;
};

class VideoCapture
{
public:
	VideoCapture();
	VideoCapture & operator=(const VideoCapture &) = delete;
	VideoCapture(const VideoCapture &) = delete;
	static VideoCapture& Instance();
	void FrameCallback(sc::PixelBuffer& buf);
	~VideoCapture();

	bool Init(uint32_t frame_rate = 15);
	void Exit();
	
	bool Start(); // 16ms
	void Stop();

	bool GetFrame(RGBAFrame& frame);

	bool IsCapturing()
	{
		if (is_initialized_ && capture_ && capture_->isStarted() == 0)
			return true;
		return false;
	}

	uint32_t Width() const
	{
		return width_;
	}

	uint32_t Height() const
	{
		return height_;
	}
	friend void TriggerCaptureCallback(void *user)
	{
		VideoCapture *capture = (VideoCapture *)user;
		capture->TriggerCapture();
	}

private:

	
	bool GetDisplaySetting(std::string& name);
	void WaitTriggerCapture();
	void TriggerCapture();
	void Capture();

	std::thread thread_;
	std::shared_ptr<sc::ScreenCapture> capture_;
	bool is_initialized_ = false;
	uint32_t frame_rate_ = 15;
	uint32_t width_ = 0;
	uint32_t height_ = 0;
	int triger_count_ = 0;
	int get_frame_count_ = 0;
	int64_t get_frame_time = 0;
	RGBAFrame last_frame_;
	std::shared_ptr<RingBuffer<RGBAFrame>> frame_buffer_;

	std::mutex mutex_trigger_;
	std::condition_variable cond_var_trigger_; //条件变量
};
