#include "VideoCapture.h"
#include "TimeUtil.h"

#include "TimerManager.h"

// screencapture
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "screencapture.lib")

using namespace std;

void FrameCallbackExt(sc::PixelBuffer & buf)
{
	VideoCapture *capture = (VideoCapture *)buf.user;
	capture->FrameCallback(buf);
}

VideoCapture::VideoCapture()
	: frame_buffer_(new RingBuffer<RGBAFrame>(5))
{
	last_frame_.size = 0;
	last_frame_.data = NULL;
}

VideoCapture::~VideoCapture()
{
	Exit();
}

VideoCapture& VideoCapture::Instance()
{
	static VideoCapture s_vc;
	return s_vc;
}

bool VideoCapture::Init(uint32_t frame_rate)
{
	if (is_initialized_)
		return true;
	frame_rate_ = frame_rate;
	if (frame_rate_ < 10)
	{
		frame_rate_ = 15;
	}

	std::vector<sc::Display*> displays;
	sc::Settings settings;

	capture_.reset(new sc::ScreenCapture(FrameCallbackExt, this));

	if (0 != capture_->init())
	{
		return false;
	}

	if (0 != capture_->getDisplays(displays))
	{
		return false;
	}

	GetDisplaySetting(displays[0]->name);

	settings.pixel_format = SC_BGRA;
	settings.display = 0;
	settings.output_width = width_;
	settings.output_height = height_;

	if (0 != capture_->configure(settings))
	{
		return false;
	}

	is_initialized_ = true;
	thread_ = std::thread(&VideoCapture::Capture, this);

	return true;
}

void VideoCapture::Exit()
{
	if (is_initialized_)
	{
		is_initialized_ = false;
		thread_.join();
		if (capture_->isStarted())
			Stop();
		capture_->shutdown();
	}
}

bool VideoCapture::Start()
{
	if (!is_initialized_)
	{
		return false;
	}

	if (0 != capture_->start())
	{
		return false;
	}

	return true;
}

void VideoCapture::Stop()
{
	capture_->stop();
}

bool VideoCapture::GetDisplaySetting(std::string& name)
{
	//DEVMODEW devMode;
	//EnumDisplaySettingsW(NULL, ENUM_CURRENT_SETTINGS, &devMode);

	width_ = GetSystemMetrics(SM_CXSCREEN);
	height_ = GetSystemMetrics(SM_CYSCREEN);

	return true;
}

void VideoCapture::WaitTriggerCapture()
{
	std::unique_lock<std::mutex> lock(mutex_trigger_);
	cond_var_trigger_.wait(lock);
}

void VideoCapture::TriggerCapture()
{
	std::unique_lock<std::mutex> lock(mutex_trigger_);
	cond_var_trigger_.notify_one();
}

void VideoCapture::Capture()
{

	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	// 将触发时间加入到定时器
	LQF::Timer trigger_timer(*LQF::TimerManager::GetInstance());
	trigger_timer.Start(TriggerCaptureCallback, this, 1000 / frame_rate_);
	while (is_initialized_)
	{
		capture_->update();
		// 等待trigger
		WaitTriggerCapture();
	}
	trigger_timer.Stop();
}

void VideoCapture::FrameCallback(sc::PixelBuffer& buf)
{
	if (frame_buffer_->IsFull())
	{
		printf("warning -> frame buffer full\n");
		return;
	}
	get_frame_count_++;
	int frameSize = buf.height * buf.width * 4;
	RGBAFrame frame(frameSize);
	frame.width = buf.width;
	frame.height = buf.height;
	memcpy(frame.data.get(), buf.plane[0], frameSize);
	frame_buffer_->Push(std::move(frame));
}

bool VideoCapture::GetFrame(RGBAFrame& frame)
{
	
	if (frame_buffer_->IsEmpty() && last_frame_.size != 0)
	{
		frame = last_frame_;
		frame.timestamp = LQF::AVTimer::GetInstance()->GetTime() 
			+ LQF::AVTimer::GetInstance()->GetAudioLatency();

		return true;
	}
	else
	{
		if (frame_buffer_->Pop(last_frame_))
		{
			frame = last_frame_;
			frame.timestamp = LQF::AVTimer::GetInstance()->GetTime() 
				+ LQF::AVTimer::GetInstance()->GetAudioLatency();
			return true;
		}
	}

	return false;
}
