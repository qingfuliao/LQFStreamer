#pragma once
class MediaPlayer;
class AudioOutput
{
public:
	AudioOutput(MediaPlayer *player);
	~AudioOutput();
	int audio_open(int wanted_channel_layout,
		int wanted_nb_channels, int wanted_sample_rate,
		struct AudioParams *audio_hw_params);
	void audio_stop();
	void update_volume(int sign, double step);
	MediaPlayer *player_;
};

