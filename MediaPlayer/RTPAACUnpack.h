#pragma once
#include <cstdint>
#include <memory>
#include <vector>

#include "RTP.h"

#define AU_HEADER_LEN     2
#define AU_HEADER_SIZE    2

#define AAC_SINGLE_AU     1

typedef struct _RTP_AAC_FRAM_T
{
	_RTP_AAC_FRAM_T()
	{
		size = 0;
		flags = 0;
		timestamp = 0;
	}

	_RTP_AAC_FRAM_T(uint32_t size)
		:data(new uint8_t[size])
	{
		this->size = size;
		flags = 0;
		timestamp = 0;
	}

	std::shared_ptr<uint8_t> data; /* 帧数据 */
	int		size;
	int		flags;
	uint32_t timestamp;
}RTP_AAC_FRAM_T;


// AAC拆包
class RTPAACUnpack
{
public:
	RTPAACUnpack(const uint8_t profile = 2, 
		const uint8_t sampling_frequency_index = 4, 
		const uint8_t channel_configuration = 2);
	~RTPAACUnpack();
	int RTPAACUnpackInput(std::vector<RTP_AAC_FRAM_T> &rtp_h264_frames, const void* packet, int bytes);
	///@return 0-ok, other-error
	int RTPPacketDeserialize(RTP_PACKET_T *pkt, const void* data, int bytes);
	int rtp_payload_check(const RTP_PACKET_T * pkt);
	int rtp_payload_onframe(std::vector<RTP_AAC_FRAM_T>& rtp_aac_frames);
	int rtp_payload_write(const RTP_PACKET_T * pkt);
	void pack(std::vector<RTP_AAC_FRAM_T> &rtp_aac_frames,
		const uint8_t *packet, int bytes, uint32_t timestamp, int flags);
private:
	int lost_; // wait for next frame
	int flags_; // lost packet

	uint16_t seq_; // rtp seq
	uint32_t timestamp_;

	uint8_t* ptr_;
	int size_;
	int capacity_;

	// AAC参数
	uint8_t profile_ = 2;
	uint8_t sampling_frequency_index_ = 4;	// 这里写死只是为了测试 4: 44100 Hz
	uint8_t channel_configuration_ = 2;
};