#pragma once
#include <iostream>
#include <mp4v2/mp4v2.h> 
#include "Media.h"
class MP4Muxer
{
public:
	MP4Muxer(const std::string &file_name);
	virtual ~MP4Muxer();
	bool Start(const int width, const int height, int fps);
	bool Stop();
	bool WriteH264Data(const uint8_t* data, int size, uint64_t timestamp);
	bool WriteAACData(const uint8_t* data, int size, uint64_t timestamp);
	
private:
	//  ”∆µ–≈œ¢
	std::string sps_;
	std::string pps_;
	std::string sei_;
	std::string file_name_;
	MP4FileHandle mp4_handle_;
	int time_scale_ = 1000;

	MP4TrackId video_track_id_ = MP4_INVALID_TRACK_ID;
	MP4TrackId audio_track_id_ = MP4_INVALID_TRACK_ID;
	int width_ = 1920;
	int height_ = 1080;
	int fps_ = 15;

	int64_t video_cur_time_ = -1;
	int64_t video_pre_time_ = -1;

	int64_t audio_cur_time_ = -1;
	int64_t audio_pre_time_ = -1;
};

