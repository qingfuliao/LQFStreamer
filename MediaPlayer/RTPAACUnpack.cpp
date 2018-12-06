#include "RTPAACUnpack.h"
#include <string.h>

#include "LogUtil.h"

#define  assert(x) {if(0==x){ LogError("assert failed"); }}


RTPAACUnpack::RTPAACUnpack(const uint8_t profile,
	const uint8_t sampling_frequency_index,
	const uint8_t channel_configuration) :
	profile_(profile),
	sampling_frequency_index_(sampling_frequency_index),
	channel_configuration_(channel_configuration),
	flags_(-1),
	lost_(0),
	seq_(0),
	timestamp_(0),
	ptr_(NULL),
	size_(0),
	capacity_(0)
{
}

RTPAACUnpack::~RTPAACUnpack()
{
	if (ptr_)
	{
		free(ptr_);
		ptr_ = NULL;
	}
}

int RTPAACUnpack::RTPAACUnpackInput(std::vector<RTP_AAC_FRAM_T>& rtp_aac_frames, const void * packet, int bytes)
{
	int i, size;
	int au_size;
	int au_numbers;
	int au_header_length;
	const uint8_t *ptr, *pau, *pend;
	RTP_PACKET_T pkt;

	if (0 != RTPPacketDeserialize(&pkt, packet, bytes) || pkt.payloadlen < 4)
	{
		LogError("no memory");
		return -EINVAL;
	}
	rtp_payload_check(&pkt);

	if (lost_)
	{
		assert(0 == size_);
		return 0; // packet discard
	}

	// save payload
	ptr = (const uint8_t *)pkt.payload;
	pend = ptr + pkt.payloadlen;

	// AU-headers-length
	au_header_length = (ptr[0] << 8) + ptr[1];
	au_header_length = (au_header_length + 7) / 8; // bit -> byte

	if (ptr + au_header_length /*AU-size*/ > pend || au_header_length < 2)
	{
		assert(0);
		size_ = 0;
		lost_ = 1;
		flags_ |= RTP_PAYLOAD_FLAG_PACKET_LOST;
		return -1; // invalid packet
	}

	// 3.3.6. High Bit-rate AAC
	// SDP fmtp: sizeLength=13; indexLength=3; indexDeltaLength=3;
	au_size = 2; // only AU-size
	au_numbers = au_header_length / au_size;
	assert(0 == au_header_length % au_size);
	ptr += 2; // skip AU headers length section 2-bytes
	pau = ptr + au_header_length; // point to Access Unit

	for (i = 0; i < au_numbers; i++)
	{
		size = (ptr[0] << 8) | (ptr[1] & 0xF8);
		size = size >> 3; // bit -> byte
		if (pau + size > pend)
		{
			assert(0);
			size_ = 0;
			lost_ = 1;
			flags_ |= RTP_PAYLOAD_FLAG_PACKET_LOST;
			return -1; // invalid packet
		}

		// TODO: add ADTS/ASC ???
		pkt.payload = pau;
		pkt.payloadlen = size;
		rtp_payload_write(&pkt);

		ptr += au_size;
		pau += size;

		if (au_numbers > 1 || pkt.rtp.m)
		{
			rtp_payload_onframe(rtp_aac_frames);
		}
	}

	return lost_ ? 0 : 1; // packet handled
}

int RTPAACUnpack::RTPPacketDeserialize(RTP_PACKET_T * pkt, const void * data, int bytes)
{
	uint32_t i, v;
	int hdrlen;
	const uint8_t *ptr;

	if (bytes < RTP_FIXED_HEADER) // RFC3550 5.1 RTP Fixed Header Fields(p12)
		return -1;
	ptr = (const unsigned char *)data;
	memset(pkt, 0, sizeof(RTP_PACKET_T));

	// pkt header
	v = nbo_r32(ptr);
	pkt->rtp.v = RTP_V(v);
	pkt->rtp.p = RTP_P(v);
	pkt->rtp.x = RTP_X(v);
	pkt->rtp.cc = RTP_CC(v);
	pkt->rtp.m = RTP_M(v);
	pkt->rtp.pt = RTP_PT(v);
	pkt->rtp.seq = RTP_SEQ(v);
	pkt->rtp.timestamp = nbo_r32(ptr + 4);
	pkt->rtp.ssrc = nbo_r32(ptr + 8);
	assert(RTP_VERSION == pkt->rtp.v);

	hdrlen = RTP_FIXED_HEADER + pkt->rtp.cc * 4;
	if (RTP_VERSION != pkt->rtp.v || bytes < hdrlen + (pkt->rtp.x ? 4 : 0) + (pkt->rtp.p ? 1 : 0))
		return -1;

	// pkt contributing source
	for (i = 0; i < pkt->rtp.cc; i++)
	{
		pkt->csrc[i] = nbo_r32(ptr + 12 + i * 4);
	}

	assert(bytes >= hdrlen);
	pkt->payload = (uint8_t*)ptr + hdrlen;
	pkt->payloadlen = bytes - hdrlen;

	// pkt header extension
	if (1 == pkt->rtp.x)
	{
		const uint8_t *rtpext = ptr + hdrlen;
		assert(pkt->payloadlen >= 4);
		pkt->extension = rtpext + 4;
		pkt->reserved = nbo_r16(rtpext);
		pkt->extlen = nbo_r16(rtpext + 2) * 4;
		if (pkt->extlen + 4 > pkt->payloadlen)
		{
			assert(0);
			return -1;
		}
		else
		{
			pkt->payload = rtpext + pkt->extlen + 4;
			pkt->payloadlen -= pkt->extlen + 4;
		}
	}

	// padding
	if (1 == pkt->rtp.p)
	{
		uint8_t padding = ptr[bytes - 1];
		if (pkt->payloadlen < padding)
		{
			assert(0);
			return -1;
		}
		else
		{
			pkt->payloadlen -= padding;
		}
	}

	return 0;
}

int RTPAACUnpack::rtp_payload_check(const RTP_PACKET_T* pkt)
{
	// first packet only
	if (-1 == flags_)
	{
		flags_ = 0;
		seq_ = (uint16_t)(pkt->rtp.seq - 1); // disable packet lost
		timestamp_ = pkt->rtp.timestamp + 1; // flag for new frame
	}

	// check sequence number
	if ((uint16_t)pkt->rtp.seq != (uint16_t)(seq_ + 1))
	{
		size_ = 0;
		lost_ = 1;
		flags_ |= RTP_PAYLOAD_FLAG_PACKET_LOST;
		timestamp_ = pkt->rtp.timestamp;
	}
	seq_ = (uint16_t)pkt->rtp.seq;

	// check timestamp
	if (pkt->rtp.timestamp != timestamp_)
	{
		//rtp_payload_onframe();
	}
	timestamp_ = pkt->rtp.timestamp;

	return 0;
}

int RTPAACUnpack::rtp_payload_onframe(std::vector<RTP_AAC_FRAM_T>& rtp_aac_frames)
{
	if (size_ > 0)
	{
		// previous packet done
		assert(!lost_);
		pack(rtp_aac_frames, ptr_, size_, timestamp_, flags_);
		flags_ &= ~RTP_PAYLOAD_FLAG_PACKET_LOST; // clear packet lost flag	
	}

	// new frame start
	lost_ = 0;
	size_ = 0;
	return 0;
}

int RTPAACUnpack::rtp_payload_write(const RTP_PACKET_T* pkt)
{
	if (size_ + pkt->payloadlen > capacity_)
	{
		void *ptr;
		size_t temp_size;

		temp_size = size_ + pkt->payloadlen + 8000;
		ptr = realloc(ptr_, temp_size);
		if (!ptr)
		{
			flags_ |= RTP_PAYLOAD_FLAG_PACKET_LOST;
			lost_ = 1;
			size_ = 0;
			return -ENOMEM;
		}

		ptr_ = (uint8_t*)ptr;
		capacity_ = temp_size;
	}

	assert(capacity_ >= size_ + pkt->payloadlen);
	memcpy(ptr_ + size_, pkt->payload, pkt->payloadlen);
	size_ += pkt->payloadlen;
	return 0;
}

void RTPAACUnpack::pack(std::vector<RTP_AAC_FRAM_T>& rtp_aac_frames, const uint8_t * packet, 
	int bytes, uint32_t timestamp, int flags)
{
	uint8_t *buffer;
	int len = bytes + 7;

	
	RTP_AAC_FRAM_T rtp_aac_frame(len);	// º”…œstart code
	buffer = rtp_aac_frame.data.get();
	buffer[0] = 0xFF; /* 12-syncword */
	buffer[1] = 0xF0 /* 12-syncword */ | (0 << 3)/*1-ID*/ | (0x00 << 2) /*2-layer*/ | 0x01 /*1-protection_absent*/;
	buffer[2] = ((profile_ - 1) << 6) | ((sampling_frequency_index_ & 0x0F) << 2) | ((channel_configuration_ >> 2) & 0x01);
	buffer[3] = ((channel_configuration_ & 0x03) << 6) | ((len >> 11) & 0x03); /*0-original_copy*/ /*0-home*/ /*0-copyright_identification_bit*/ /*0-copyright_identification_start*/
	buffer[4] = (uint8_t)(len >> 3);
	buffer[5] = ((len & 0x07) << 5) | 0x1F;
	buffer[6] = 0xFC | ((len / 1024) & 0x03);
	
	memcpy(&buffer[7], packet, bytes);
	rtp_aac_frame.flags = flags;
	rtp_aac_frame.size = len;
	rtp_aac_frame.timestamp = timestamp;
	rtp_aac_frames.push_back(rtp_aac_frame);
}
