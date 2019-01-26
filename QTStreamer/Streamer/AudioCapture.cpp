#include "AudioCapture.h"
#include "LogUtil.h"
#include "TimerManager.h"


#pragma comment(lib, "portaudio_static_x86.lib")

#define PA_SAMPLE_TYPE  paInt16  
typedef short SAMPLE;

AudioCapture::AudioCapture()
	: frame_buffer_(new LQFRingBuffer<PCMFrame>(10))
{
	Pa_Initialize();
}

AudioCapture::~AudioCapture()
{
	Exit();
	Pa_Terminate();
}

AudioCapture& AudioCapture::Instance()
{
	static AudioCapture s_ac;
	return s_ac;
}

void paPrintDevices()
{
	int     num_devices, default_displayed, my_device = 0;
	const   PaDeviceInfo *deviceInfo;

	num_devices = Pa_GetDeviceCount();
	if (num_devices < 0)
		return;

	printf("Number of devices found = %d\n", num_devices);

	for (int i = 0; i < num_devices; i++) {
		deviceInfo = Pa_GetDeviceInfo(i);
		printf("--------------------------------------- device #%d\n", i);
		/* Mark global and API specific default devices */
		default_displayed = 0;
		if (i == Pa_GetDefaultInputDevice())
		{
			my_device = i;
			printf("[ Default Input");
			default_displayed = 1;
		}
		else if (i == Pa_GetHostApiInfo(deviceInfo->hostApi)->defaultInputDevice)
		{
			const PaHostApiInfo *hostInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);
			printf("[ Default %s Input", hostInfo->name);
			default_displayed = 1;
		}

		if (i == Pa_GetDefaultOutputDevice())
		{
			printf((default_displayed ? "," : "["));
			printf(" Default Output");
			default_displayed = 1;
		}
		else if (i == Pa_GetHostApiInfo(deviceInfo->hostApi)->defaultOutputDevice)
		{
			const PaHostApiInfo *hostInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);
			printf((default_displayed ? "," : "["));
			printf(" Default %s Output", hostInfo->name);
			default_displayed = 1;
		}
		if (default_displayed)
			printf(" ]\n");

		/* print device info fields */
		printf("Name                        = %s\n", deviceInfo->name);
		printf("Host API                    = %s\n", Pa_GetHostApiInfo(deviceInfo->hostApi)->name);
		printf("Max inputs = %d", deviceInfo->maxInputChannels);
		printf(", Max outputs = %d\n", deviceInfo->maxOutputChannels);

		printf("Default low input latency   = %8.3f\n", deviceInfo->defaultLowInputLatency);
		printf("Default low output latency  = %8.3f\n", deviceInfo->defaultLowOutputLatency);
		printf("Default high input latency  = %8.3f\n", deviceInfo->defaultHighInputLatency);
		printf("Default high output latency = %8.3f\n", deviceInfo->defaultHighOutputLatency);
	}
}
static int FrameCallbackExt(const void *input_buffer, void *output_buffer,
	unsigned long frames_per_buffer,
	const PaStreamCallbackTimeInfo* time_info,
	PaStreamCallbackFlags status_flags, void *user_data)
{
	AudioCapture *capture = (AudioCapture *)user_data;
	return capture->FrameCallback(input_buffer, 
		output_buffer, 
		frames_per_buffer,
		time_info,
		status_flags,
		user_data);
}

bool AudioCapture::Init(uint32_t samplerate, uint32_t channels)
{
	if (is_initialized_)
		return false;
	samplerate_ = samplerate;
	channels_ = channels;
	input_parameters_.device =  Pa_GetDefaultInputDevice();	// 先默认为2 只录制声卡
	if (input_parameters_.device == paNoDevice)
	{
		LogError("Default input device not found.\n");
		return false;
	}
	input_parameters_.channelCount = channels_;
	input_parameters_.sampleFormat = PA_SAMPLE_TYPE;
	input_parameters_.suggestedLatency = Pa_GetDeviceInfo(input_parameters_.device)->defaultLowInputLatency;
	LQF::AVTimer::GetInstance()->SetAudioLatency((uint64_t)input_parameters_.suggestedLatency);

	input_parameters_.hostApiSpecificStreamInfo = NULL;

	PaError error;
	error = Pa_OpenStream(&stream_, &input_parameters_, NULL, samplerate_,
		AUDIO_LENGTH_PER_FRAME, paClipOff, FrameCallbackExt, this);
	if (error != paNoError)
	{
		LogError("Pa_OpenStream() failed.");
	}

	is_initialized_ = true;

	return true;
}


void AudioCapture::Exit()
{
	if (is_initialized_)
	{
		is_initialized_ = false;
		if (IsCapturing())
			Stop();

		if (stream_)
		{
			Pa_CloseStream(stream_);
			stream_ = nullptr;
		}
	}
}

int AudioCapture::FrameCallback(const void *input_buffer, void *output_buffer,
	unsigned long frames_per_buffer,
	const PaStreamCallbackTimeInfo* time_info,
	PaStreamCallbackFlags status_flags, void *user_data)
{
	if (frame_buffer_->IsFull())
	{
		return paContinue;
	}

	int frame_size = channels_ * sizeof(SAMPLE) * frames_per_buffer;
	PCMFrame frame(frame_size);
	memcpy(frame.data.get(), input_buffer, frame_size);

	frame.timpestamp = LQF::AVTimer::GetInstance()->GetTime() 
		+ LQF::AVTimer::GetInstance()->GetVideoLatency();		// 音频在回调函数里面

	frame_buffer_->Push(std::move(frame));
	return paContinue;
}

bool AudioCapture::Start()
{
	if (IsCapturing())
	{
		return false;
	}

	if (Pa_StartStream(stream_) == paNoError)
		return true;
	else
		return false;
}

void AudioCapture::Stop()
{
	if (IsCapturing())
	{
		Pa_StopStream(stream_);
	}
}

bool AudioCapture::GetFrame(PCMFrame& frame)
{
	if (frame_buffer_->IsEmpty())
	{
		return false;
	}

	return frame_buffer_->Pop(frame);
}
