#ifndef __H264_RTMP_PACKAGER_H__
#define __H264_RTMP_PACKAGER_H__
#include "RTMPPackager.h"

#define RTMP_HEAD_SIZE   (sizeof(RTMPPacket) + RTMP_MAX_HEADER_SIZE)
typedef enum
{
	NAL_SLICE = 1,
	NAL_SLICE_DPA = 2,
	NAL_SLICE_DPB = 3,
	NAL_SLICE_DPC = 4,
	NAL_SLICE_IDR = 5,
	NAL_SEI = 6,
	NAL_SPS = 7,
	NAL_PPS = 8,
	NAL_AUD = 9,
	NAL_FILLER = 12,
	NAL_INVALID = -1
}NAL_TYPE;

class H264RTMPPackager : public RTMPPackager
{
public:
	virtual void Pack(RTMPPacket *packet, char* buf, const char* data, int length) const override;
	virtual void Metadata(RTMPPacket *packet, char * buf, uint8_t * sps, int sps_len, uint8_t * pps, int pps_len) const ;
	virtual void Metadata(RTMPPacket *packet, char* buf, const char* data, int length) const override;
	static bool IsKeyFrame(char* data) { return (data[4] & 0x1f) == 0x5; }	// I帧
	static bool IsSPSFrame(char* data) { return (data[4] & 0x1f) == 0x7; }	// 序列参数集 （sps帧）
	static bool IsPPSFrame(char* data) { return (data[4] & 0x1f) == 0x8; }	//  图像参数集 

	static NAL_TYPE FindStartCode4Bytes(unsigned char* data, int length, int& offset)
	{
		if (5 > length)
			return NAL_INVALID;

		for (int i = 0; i < length - 3; ++i)
		{
			if (0 == data[i] && 0 == data[i + 1] && 0 == data[i + 2] && 1 == data[i + 3])
			{
				offset = i;
				return (NAL_TYPE)(data[i + 4] & 0x1f);
			}
		}

		return NAL_INVALID;
	}

	static NAL_TYPE FindStartCode3Bytes(unsigned char* data, int length, int& offset)
	{
		if (4 > length)
			return NAL_INVALID;

		for (int i = 0; i < length - 3; ++i)
		{
			if (0 == data[i] && 0 == data[i + 1] && 1 == data[i + 2])
			{
				offset = i;
				return (NAL_TYPE)(data[i + 3] & 0x1f);
			}
		}

		return NAL_INVALID;
	}
	static NAL_TYPE GetVideoPacketType(unsigned char *data, int length, int &offset)
	{
		offset = 0;

		if (3 > length)
			return NAL_INVALID;

		if (0 != data[0] || 0 != data[1])
			return NAL_INVALID;

		if (1 == data[2])
		{
			offset = 3;
		}
		else if (0 == data[2] && 1 == data[3])
		{
			offset = 4;
		}
		else
		{
			return NAL_INVALID;
		}

		return (NAL_TYPE)(data[offset] & 0x1f);
	}
};

#endif // !__H264_RTMP_PACKAGER_H__