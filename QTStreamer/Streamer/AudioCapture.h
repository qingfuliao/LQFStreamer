#pragma once
#include <iostream>
#include <Mmdeviceapi.h>
#include <Audioclient.h>
#include <memory>
#include "LQFRingBuffer.h"
#include "portaudio.h"  


#define AUDIO_LENGTH_PER_FRAME 1024
struct PCMFrame
{
	PCMFrame(uint32_t size = 100)
		: data(new uint8_t[size + 1024])
	{
		this->size = size;
	}
	uint32_t size = 0;
	uint32_t channels = 2;
	uint32_t samplerate = 44100;
	std::shared_ptr<uint8_t> data;
	int64_t timpestamp;
};

typedef struct AudioCaptureConfig_
{
	int sample_rate;
	int channels;
}AudioCaptureConfig;

class AudioCapture
{
public:
	AudioCapture();
	AudioCapture & operator=(const AudioCapture &) = delete;
	AudioCapture(const AudioCapture &) = delete;
	static AudioCapture& Instance();
	~AudioCapture();

	int FrameCallback(const void *input_buffer, void *output_buffer,
		unsigned long frames_per_buffer,
		const PaStreamCallbackTimeInfo* time_info,
		PaStreamCallbackFlags status_flags, void *user_data);

	bool Init(uint32_t samplerate = 44100, uint32_t channels = 2);
	void Exit();

	bool Start();
	void Stop();

	bool GetFrame(PCMFrame& frame);

	bool IsCapturing()
	{
		return (is_initialized_ && Pa_IsStreamActive(stream_));
	}

private:
	PaStreamParameters input_parameters_;
	PaStream* stream_ = nullptr;
	bool is_initialized_ = false;
	uint32_t channels_ = 2;
	uint32_t samplerate_ = 44100;
	std::shared_ptr<LQFRingBuffer<PCMFrame>> frame_buffer_;
};

