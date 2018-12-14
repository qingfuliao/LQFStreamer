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

typedef struct RGBAFrame
{
	RGBAFrame(uint32_t size = 100)
		: data(new uint8_t[size])
	{
		this->size = size;
	}

	uint32_t size = 0;
	uint32_t width;
	uint32_t height;
	uint64_t timestamp;
	std::shared_ptr<uint8_t> data;
}RGBA_FRAME_T;

//	win7及以前版本的录制
typedef struct cap_screen_
{
	HDC memdc;
	HBITMAP hbmp;
	uint8_t* buffer;
	int            length;

	int width;
	int height;
	int bitcount;
}WIN7_CAPTURE_SCREEN_T;

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
		if(b_win8_above)
		{ 
			if (is_initialized_ && capture_ && capture_->isStarted() == 0)
				return true;
			else
				return false;
		}
		else
		{
			return is_initialized_;
		}
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
		capture->triggerCapture();
	}

private:
	bool getDisplaySetting(std::string& name);
	void waitTriggerCapture();
	void triggerCapture();
	void win7Capture();
	void win8OrAboveCapture();
	void run();

	std::thread thread_;

	bool b_win8_above = true;		//win8及以上版本
	// WIN8及以上版本使用
	std::shared_ptr<sc::ScreenCapture> capture_;
	// WIN7及以下版本以下使用
	WIN7_CAPTURE_SCREEN_T win7_capture_;

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
