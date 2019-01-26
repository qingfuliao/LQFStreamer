#include "MP4Muxer.h"
#include "LogUtil.h"
#include "TimerManager.h"
// NALU单元    
typedef struct _NaluUnit
{
	int type;
	int size;
	int start_code_bytes;
	uint8_t *data;
}NaluUnit;

static const char start_code_4bytes[4] = { 0x00, 0x00, 0x00, 0x01 };
static const char start_code_3bytes[3] = { 0x00, 0x00, 0x01 };

static bool readNaluFromBuf(const uint8_t * buffer, uint32_t nBufferSize, NaluUnit & nalu)
{
	if (nBufferSize < 4)
	{
		return false;
	}
	int start_code_len = 4;

	if (memcmp(buffer, start_code_4bytes, 4) == 0)
	{
		start_code_len = 4;
	}
	else if (memcmp(buffer, start_code_3bytes, 3) == 0)
	{
		start_code_len = 3;
	}
	else
	{
		return false;
	}
	nalu.start_code_bytes = start_code_len;
	nalu.type = buffer[start_code_len] & 0x1f;
	nalu.data = (uint8_t*)&buffer[start_code_len];
	nalu.size = nBufferSize - start_code_len;

	return true;
}


MP4Muxer::MP4Muxer(const std::string & file_name):
	file_name_(file_name),
	mp4_handle_(NULL)
{
}

MP4Muxer::~MP4Muxer()
{
	Stop();
}

bool MP4Muxer::Start(const int width, const int height, int fps)
{
	bool ret = true;
	
	mp4_handle_ = MP4Create(file_name_.c_str());

	if (mp4_handle_ == MP4_INVALID_FILE_HANDLE)
	{
		LogError("ERROR:Open file fialed.");
		return false;
	}
	time_scale_ = 1000;
	ret = MP4SetTimeScale(mp4_handle_, time_scale_);
	if (!ret)
	{
		LogError("MP4SetTimeScale fialed.");
	}

	width_ = width;
	height_ = height;
	fps_ = fps;

	return ret;
}

bool MP4Muxer::Stop()
{
	if (mp4_handle_)
	{
		MP4Close(mp4_handle_);
		mp4_handle_ = NULL;
		LogDebug("MP4Close ok");
	}
	else
	{
		LogDebug("file has been close");
	}
	return true;
}

bool MP4Muxer::WriteH264Data(const uint8_t * data, int size, uint64_t timestamp)
{
	NaluUnit nalu;
	int pos = 0, len = 0;

	if(readNaluFromBuf(data, size, nalu))
	{
		// 添加h264 track     
		if (video_track_id_ == MP4_INVALID_TRACK_ID)
		{
			video_track_id_ = MP4AddH264VideoTrack
				(mp4_handle_,
				time_scale_,
				time_scale_ / fps_,
				width_,     // width    
				height_,    // height    
				nalu.data[1], // sps[1] AVCProfileIndication    
				nalu.data[2], // sps[2] profile_compat    
				nalu.data[3], // sps[3] AVCLevelIndication    
				3);           // 4 bytes length before each NAL unit    
			if (video_track_id_ == MP4_INVALID_TRACK_ID)
			{
				LogError("add video track failed.\n");
				return false;
			}
			MP4SetVideoProfileLevel(mp4_handle_, 0x7F); //  Simple Profile @ Level 3    
		}

		if (nalu.type == 0x07) // sps    			// 不加SPS PPS在mediaplayer不能正常播放
		{
			MP4AddH264SequenceParameterSet(mp4_handle_, video_track_id_, nalu.data, nalu.size);
			LogDebug("sps frame, len = %d\n",  nalu.size);
			sps_.clear();
			sps_.append(nalu.data, nalu.data+nalu.size);
		}
		else if (nalu.type == 0x08) // pps    
		{
			MP4AddH264PictureParameterSet(mp4_handle_, video_track_id_, nalu.data, nalu.size);
			LogDebug("pps frame, len = %d\n", nalu.size);
			pps_.clear();
			pps_.append(nalu.data, nalu.data+nalu.size);
		}
		else
		{
			int64_t duration = 0;
			video_cur_time_ = timestamp;
			if (video_pre_time_ == -1)
			{
				duration = timestamp;
			}
			else
			{
				duration = video_cur_time_ - video_pre_time_;
			}
			video_pre_time_ = video_cur_time_;
			//LogDebug("timestamp:%llu", timestamp);
			if (!MP4WriteSample(mp4_handle_, video_track_id_, 
				nalu.data - nalu.start_code_bytes,
				nalu.size + nalu.start_code_bytes,
				duration, 0, nalu.type == 0x5 ? 1:0))
			{
				LogDebug(" MP4WriteSample failed");
				return false;
			}
			
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool MP4Muxer::WriteAACData(const uint8_t * data, int size, uint64_t timestamp)
{
	if (mp4_handle_ == NULL)
	{
		return false;
	}

	//printf("%s(%d) nalu.type = 0x%02x\n", __FUNCTION__, __LINE__, nalu.type);
	// 添加h264 track     
	if (audio_track_id_ == MP4_INVALID_TRACK_ID)
	{
		audio_track_id_ = MP4AddAudioTrack
			(mp4_handle_,
				1000,
				23,
				MP4_MPEG2_AAC_LC_AUDIO_TYPE);           // 4 bytes length before each NAL unit    
		if (audio_track_id_ == MP4_INVALID_TRACK_ID)
		{
			LogError("add audio track failed.\n");
			return false;
		}
	}

	

	int64_t duration = 0;
	audio_cur_time_ = timestamp;
	if (audio_pre_time_ == -1)
	{
		duration = timestamp;
	}
	else
	{
		duration = audio_cur_time_ - audio_pre_time_;
	}
	audio_pre_time_ = audio_cur_time_;
	//LogDebug("timestamp:%llu", timestamp);
	if(!MP4WriteSample(mp4_handle_, audio_track_id_,
		(unsigned char*)data, size, duration, 0, 1))
	{
		LogDebug(" MP4WriteSample failed");
		return false;
	}
	return true;
}
