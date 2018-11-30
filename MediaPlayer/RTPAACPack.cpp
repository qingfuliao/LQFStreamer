#include "RTPAACPack.h"
#include <string.h>


/**
 * rtp承载aac的格式由两部分组成:
* 2个字节的AU-headers-length
* n个AU-header，每个2字节
* n个AU，是aac去掉adts的载荷
 */

RTPAACPack::RTPAACPack(const uint32_t ssrc, const uint8_t payload_type, 
	const int rtp_packet_max_size, const int max_frames_per_packet):
	rtp_packet_max_size_(rtp_packet_max_size),
	max_frames_per_packet_(max_frames_per_packet),
	seq_num_(0)
{
	if (rtp_packet_max_size_ > RTP_PACKET_MAX_SIZE)
	{
		rtp_packet_max_size_ = RTP_PACKET_MAX_SIZE;		// 最大限制
	}
	memset(&rtp_hdr, 0, sizeof(rtp_hdr));
	rtp_hdr.set_type(payload_type);
	rtp_hdr.set_version(RTP_VERSION);
}

RTPAACPack::~RTPAACPack()
{

}

// 目前只支持每个packet 单frame的情况
bool RTPAACPack::Pack(uint8_t * buf, uint32_t buf_size, uint32_t timestamp, bool end_of_frame)
{
	// max_frames_per_packet_当前为1，只支持单帧每packet的情况
	const int max_au_headers_size = 2 + 2 * max_frames_per_packet_;
	uint8_t au_header_len[2];
	uint8_t au_header[2];

	// 跳过ADTS
	buf_size -= 7;
	buf += 7;
	if (RTP_HEADER_LEN + max_au_headers_size + buf_size > rtp_packet_max_size_)
	{
		return false;			// 一个包不够空间发送一帧AAC数据
	}

	// max_frames_per_packet_ = 1， 这里目前只考虑一帧的情况，从传输效率的角度看
	// 可以多个aac包封装一起再传输
	au_header_len[0] = (((AU_HEADER_SIZE * max_frames_per_packet_) * 8) >> 8) & 0xff;
	au_header_len[1] = ((AU_HEADER_SIZE * max_frames_per_packet_) * 8) & 0xff;
	// 高13bit存储
	au_header[0] = (buf_size & 0x1fe0) >> 5;	// 存储高8位
	au_header[1] = (buf_size & 0x1f) << 3;		// 存储低5位

	sendbuf_len_ = RTP_HEADER_LEN + max_au_headers_size + buf_size;

	rtp_hdr.set_mark(end_of_frame);
	rtp_hdr.set_seq_num(seq_num_++);
	rtp_hdr.set_time_stamp(timestamp);


	memcpy(sendbuf_, &rtp_hdr, RTP_HEADER_LEN);
	sendbuf_[RTP_HEADER_LEN + 0] = au_header_len[0];
	sendbuf_[RTP_HEADER_LEN + 1] = au_header_len[1];
	sendbuf_[RTP_HEADER_LEN + 2] = au_header[0];
	sendbuf_[RTP_HEADER_LEN + 3] = au_header[1];

	return true;
}

uint8_t * RTPAACPack::GetPacket(int & out_packet_size)
{
	if(0 == sendbuf_len_)
	{
		return nullptr;
	}
	out_packet_size = sendbuf_len_;
	sendbuf_len_ = 0;
	return sendbuf_;
}
