#include "VideoCapture.h"
#include "LogUtil.h"
#include "TimeUtil.h"

#include "TimerManager.h"

// screencapture
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "screencapture.lib")

using namespace std;

static bool greatThanOrEqualtoWin8(void)
{
	bool greatThanOrEqualtoWin8 = false;		// 默认是win7及以下的系统，以增强兼容性
	string           stOSysetemVerion;
	int               iVer = 0;
	OSVERSIONINFOW    OsVersionInfo;
	OsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);
	GetVersionExW(&OsVersionInfo);
	if (OsVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		if (OsVersionInfo.dwMinorVersion < 10)
		{
			stOSysetemVerion = "Windows95";
			iVer = 1;
		}
		else if (OsVersionInfo.dwMinorVersion == 10)
		{
			stOSysetemVerion = "Windows98";
			iVer = 2;
		}
		else
		{
			stOSysetemVerion = "WindowsMe";
			iVer = 3;
		}
	}
	else if (OsVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		if (OsVersionInfo.dwMajorVersion == 3)
		{
			stOSysetemVerion = "WindowsNT 3.51";
			iVer = 4;
		}
		else if (OsVersionInfo.dwMajorVersion == 4)
		{
			stOSysetemVerion = "WindowsNT 4";
			iVer = 5;
		}
		else if (OsVersionInfo.dwMajorVersion == 5)
		{
			if (OsVersionInfo.dwMinorVersion == 0)
			{
				stOSysetemVerion = "Windows 2000";

				iVer = 6;
			}
			else if (OsVersionInfo.dwMinorVersion >= 1)
			{
				stOSysetemVerion = "Windows XP";
				iVer = 7;//windows xp;
			}
			else if (OsVersionInfo.dwMinorVersion == 2)
			{
				stOSysetemVerion = "Windows 2003";
				iVer = 12;//windows xp;
			}
		}
	}
	else if (OsVersionInfo.dwPlatformId == 3)
	{
		stOSysetemVerion = "Windows CE";
		iVer = 8;
	}
	else
	{
		stOSysetemVerion = "No Name";
		iVer = 9;
	}
	if (OsVersionInfo.dwMajorVersion == 6 && OsVersionInfo.dwMinorVersion == 0)
	{
		stOSysetemVerion = "vista";
		iVer = 10;
	}
	if (OsVersionInfo.dwMajorVersion == 6 && OsVersionInfo.dwMinorVersion == 1)
	{
		stOSysetemVerion = "win7";
		iVer = 11;
	}
	if ((OsVersionInfo.dwMajorVersion == 6) && (OsVersionInfo.dwMinorVersion == 2))
	{
		stOSysetemVerion = "win8";
		iVer = 13;
		greatThanOrEqualtoWin8 = true;
	}
	if ((OsVersionInfo.dwMajorVersion == 6) && (OsVersionInfo.dwMinorVersion == 3))
	{
		stOSysetemVerion = "win8.1";
		iVer = 14;
		greatThanOrEqualtoWin8 = true;
	}
	if ((OsVersionInfo.dwMajorVersion == 10) && (OsVersionInfo.dwMinorVersion == 0))
	{
		stOSysetemVerion = "win10";
		iVer = 15;
		greatThanOrEqualtoWin8 = true;
	}
	LogDebug("Windows version:%s", stOSysetemVerion.c_str());

	return  greatThanOrEqualtoWin8;
}

static bool win7InitCaptureScreen(WIN7_CAPTURE_SCREEN_T *sc)
{
	DEVMODE devmode;
	BOOL bRet;
	BITMAPINFOHEADER bi;

	memset(&devmode, 0, sizeof(DEVMODE));
	devmode.dmSize = sizeof(DEVMODE);
	devmode.dmDriverExtra = 0;
	bRet = EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devmode);
	sc->width = devmode.dmPelsWidth;
	sc->height = devmode.dmPelsHeight;
	sc->bitcount = devmode.dmBitsPerPel;
	memset(&bi, 0, sizeof(bi));
	bi.biSize = sizeof(bi);
	bi.biWidth = sc->width;
	bi.biHeight = -sc->height; //从上朝下扫描
	bi.biPlanes = 1;
	bi.biBitCount = sc->bitcount; //RGB
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	HDC hdc = GetDC(NULL); //屏幕DC
	sc->memdc = CreateCompatibleDC(hdc);
	sc->buffer = NULL;
	sc->hbmp = CreateDIBSection(hdc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, (void**)&sc->buffer, NULL, 0);
	ReleaseDC(NULL, hdc);
	SelectObject(sc->memdc, sc->hbmp); ///
	sc->length = sc->height* (((sc->width*sc->bitcount / 8) + 3) / 4 * 4);
	return true;
}

void DeInit(WIN7_CAPTURE_SCREEN_T *sc)
{
	DeleteObject(sc->hbmp);
	DeleteDC(sc->memdc);
}

static bool bitBltCapture(WIN7_CAPTURE_SCREEN_T *sc)	// win7及以下使用的方法
{
	HDC hdc = GetDC(NULL);
	BitBlt(sc->memdc, 0, 0, sc->width, sc->height, hdc, 0, 0, SRCCOPY | CAPTUREBLT);
	DeleteDC(hdc);
	ReleaseDC(NULL, hdc);
	return 0;
}

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

	b_win8_above = greatThanOrEqualtoWin8();
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

	if(b_win8_above)		// win8及以上
	{
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

		getDisplaySetting(displays[0]->name);

		settings.pixel_format = SC_BGRA;
		settings.display = 0;
		settings.output_width = width_;
		settings.output_height = height_;

		if (0 != capture_->configure(settings))
		{
			LogError("win8OrAboveInitCaptureScreen failed");
			return false;
		}
	}
	else   // win7及以下
	{
		if (!win7InitCaptureScreen(&win7_capture_))
		{
			LogError("win7InitCaptureScreen failed");
			return false;
		}
		width_ = win7_capture_.width;
		height_ = win7_capture_.height;
	}

	is_initialized_ = true;
	thread_ = std::thread(&VideoCapture::run, this);

	return true;
}

void VideoCapture::Exit()
{
	if (is_initialized_)
	{
		is_initialized_ = false;
		thread_.join();
		if(b_win8_above)	// win8及以上
		{ 
			if (capture_->isStarted())
				Stop();
			capture_->shutdown();
		}
	}
}

bool VideoCapture::Start()
{
	if (!is_initialized_)
	{
		return false;
	}

	if(b_win8_above)
	{
		if (0 != capture_->start())
		{
			return false;
		}
	}
	return true;
}

void VideoCapture::Stop()
{
	if (b_win8_above)
	{
		capture_->stop();
	}
}

bool VideoCapture::getDisplaySetting(std::string& name)
{
	width_ = GetSystemMetrics(SM_CXSCREEN);
	height_ = GetSystemMetrics(SM_CYSCREEN);

	return true;
}

void VideoCapture::waitTriggerCapture()
{
	std::unique_lock<std::mutex> lock(mutex_trigger_);
	cond_var_trigger_.wait(lock);
}

void VideoCapture::triggerCapture()
{
	std::unique_lock<std::mutex> lock(mutex_trigger_);
	cond_var_trigger_.notify_one();
}

void VideoCapture::win7Capture()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	// 将触发时间加入到定时器
	LQF::Timer trigger_timer(*LQF::TimerManager::GetInstance());
	trigger_timer.Start(TriggerCaptureCallback, this, 1000 / frame_rate_);
	while (is_initialized_)
	{
		//uint64_t cur_time = LQF::AVTimer::GetInstance()->GetTime();
		/**bitBltCapture函数耗时,大概在33ms，效率没有win8新接口高
		 *	bitBltCapture t:25ms
			bitBltCapture t:39ms
			bitBltCapture t:29ms
		 */
		bitBltCapture(&win7_capture_);
		//LogDebug("bitBltCapture t:%llums", LQF::AVTimer::GetInstance()->GetTime() - cur_time);
		if (!frame_buffer_->IsFull())
		{
			get_frame_count_++;
			int frameSize = win7_capture_.height * win7_capture_.width * 4;
			RGBAFrame frame(frameSize);
			frame.width = win7_capture_.width;
			frame.height = win7_capture_.height;
			memcpy(frame.data.get(), win7_capture_.buffer, frameSize);
			frame_buffer_->Push(std::move(frame));
		}
		else
		{
			LogDebug("warning -> frame buffer full\n");
		}
		// 等待trigger
		waitTriggerCapture();
	}
	trigger_timer.Stop();
}

void VideoCapture::win8OrAboveCapture()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	// 将触发时间加入到定时器
	LQF::Timer trigger_timer(*LQF::TimerManager::GetInstance());
	trigger_timer.Start(TriggerCaptureCallback, this, 1000 / frame_rate_);
	while (is_initialized_)
	{
		capture_->update();
		// 等待trigger
		waitTriggerCapture();
	}
	trigger_timer.Stop();
}

void VideoCapture::run()
{
	FunEntry();
	if (b_win8_above)
	{
		win8OrAboveCapture();
	}
	else
	{
		win7Capture();
	}
	FunExit();
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
