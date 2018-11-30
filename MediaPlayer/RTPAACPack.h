#pragma once
#include <cstdint>
#include "RTP.h"

#define AU_HEADER_LEN     2
#define AU_HEADER_SIZE    2

#define AAC_SINGLE_AU     1

// 参考 ffmpeg rtpenc_aac.c， 具体见RFC3640
/**
 * 需要将aac的ADTS去掉；
 * 1.添加12字节的rtp报头；
 * 2.添加2字节的AU_HEADER_LENGTH；
 * 3.添加2字节的AU_HEADER；
 * 3.从第17字节开始就是payload（去掉ADTS的aac数据）数据了
 */
class RTPAACPack
{
public:
	
// 		const uint16_t rtp_packet_max_size
	RTPAACPack(const uint32_t ssrc = 1,
				const uint8_t payload_type = RTP_PAYLOAD_TYPE_AAC,
				const int rtp_packet_max_size = RTP_PACKET_MAX_SIZE, 
				const int max_frames_per_packet = 1);
	~RTPAACPack();
	bool Pack(uint8_t *buf, uint32_t buf_size,
		uint32_t timestamp, bool end_of_frame);

	//循环调用Get获取RTP包，直到返回值为NULL
	uint8_t* GetPacket(int &out_packet_size);
private:
	RTP_HDR_T	rtp_hdr;    // RTP header is assembled here
	uint8_t sendbuf_[RTP_PACKET_MAX_SIZE];
	uint32_t sendbuf_len_ = 0;
	uint16_t seq_num_ = 0;
	 
	int max_frames_per_packet_ = 1;	// 默认为1，每个IP包发一帧AAC数据
	int rtp_packet_max_size_ = RTP_PACKET_MAX_SIZE;
};

// AAC拆包
class RTPAACUnpack
{
};