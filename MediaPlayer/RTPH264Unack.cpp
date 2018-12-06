#include "RtpH264Unpack.h"
#include <string.h>
#include <stdio.h>
#include <vector>
#include "LogUtil.h"

#define H264_NAL(v)	(v & 0x1F)
#define FU_START(v) (v & 0x80)
#define FU_END(v)	(v & 0x40)
#define FU_NAL(v)	(v & 0x1F)

#define  assert(x) {if(0==x){ LogError("assert failed"); return -1; }}
using namespace std;

RTPH264Unpack::RTPH264Unpack(void):
	seq_( 0 ),
	ptr_(NULL),
	size_(0),
	capacity_(0),
	flags_(-1)
{
	
}


RTPH264Unpack::~RTPH264Unpack(void)
{
	if (ptr_)
	{
		free(ptr_);
		ptr_ = NULL;
	 }
}
// 5.7.1. Single-Time Aggregation Packet (STAP) (p23)
/*
0               1               2               3
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                           RTP Header                          |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|STAP-B NAL HDR |            DON                |  NALU 1 Size  |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
| NALU 1 Size   | NALU 1 HDR    |         NALU 1 Data           |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               +
:                                                               :
+               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|               | NALU 2 Size                   |   NALU 2 HDR  |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                            NALU 2 Data                        |
:                                                               :
|                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                               :    ...OPTIONAL RTP padding    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
int RTPH264Unpack::rtpH264UnpackSTAP(std::vector<RTP_H264_FRAM_T>& rtp_h264_frames, const uint8_t * ptr, int bytes, uint32_t timestamp, int stap_b)
{
	int n;
	uint16_t len;
	uint16_t don;

	n = stap_b ? 3 : 1;
	don = stap_b ? nbo_r16(ptr + 1) : 0;
	ptr += n; // STAP-A / STAP-B HDR + DON

	for (bytes -= n; bytes > 2; bytes -= len + 2)
	{
		len = nbo_r16(ptr);
		if (len + 2 > bytes)
		{
			assert(0);
			flags_ = RTP_PAYLOAD_FLAG_PACKET_LOST;
			size_ = 0;
			return -EINVAL; // error
		}

		assert(H264_NAL(ptr[2]) > 0 && H264_NAL(ptr[2]) < 24);
		pack(rtp_h264_frames, ptr + 2, len, timestamp, flags_);
		flags_ = 0;
		size_ = 0;

		ptr += len + 2; // next NALU
		don = (don + 1) % 65536;
	}

	return 1; // packet handled
}
// 5.7.2. Multi-Time Aggregation Packets (MTAPs) (p27)
/*
0               1               2               3
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                          RTP Header                           |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|MTAP16 NAL HDR |   decoding order number base  |  NALU 1 Size  |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
| NALU 1 Size   | NALU 1 DOND   |         NALU 1 TS offset      |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
| NALU 1 HDR    |                NALU 1 DATA                    |
+-+-+-+-+-+-+-+-+                                               +
:                                                               :
+               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|               | NALU 2 SIZE                   |   NALU 2 DOND |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
| NALU 2 TS offset              | NALU 2 HDR    |  NALU 2 DATA  |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+               |
:                                                               :
|                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                               :    ...OPTIONAL RTP padding    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
int RTPH264Unpack::rtpH264UnpackMTAP(std::vector<RTP_H264_FRAM_T>& rtp_h264_frames, const uint8_t * ptr, int bytes, uint32_t timestamp, int n)
{
	uint16_t dond;
	uint16_t donb;
	uint16_t len;
	uint32_t ts;

	donb = nbo_r16(ptr + 1);
	ptr += 3; // MTAP16/MTAP24 HDR + DONB

	for (bytes -= 3; bytes > 3 + n; bytes -= len + 2)
	{
		len = nbo_r16(ptr);
		if (len + 2 > bytes || len < 1 /*DOND*/ + n /*TS offset*/ + 1 /*NALU*/)
		{
			assert(0);
			flags_ = RTP_PAYLOAD_FLAG_PACKET_LOST;
			size_ = 0;
			return -EINVAL; // error
		}

		dond = (ptr[2] + donb) % 65536;
		ts = (uint16_t)nbo_r16(ptr + 3);
		if (3 == n) ts = (ts << 16) | ptr[5]; // MTAP24

											  // if the NALU-time is larger than or equal to the RTP timestamp of the packet, 
											  // then the timestamp offset equals (the NALU - time of the NAL unit - the RTP timestamp of the packet).
											  // If the NALU - time is smaller than the RTP timestamp of the packet,
											  // then the timestamp offset is equal to the NALU - time + (2 ^ 32 - the RTP timestamp of the packet).
		ts += timestamp; // wrap 1 << 32

		assert(H264_NAL(ptr[n + 3]) > 0 && H264_NAL(ptr[n + 3]) < 24);
		pack(rtp_h264_frames, ptr + 1 + n, len - 1 - n, ts, flags_);
		flags_ = 0;
		size_ = 0;

		ptr += len + 1 + n; // next NALU
	}

	return 1; // packet handled
}
// 5.8. Fragmentation Units (FUs) (p29)
/*
0               1               2               3
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  FU indicator |   FU header   |              DON              |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-|
|                                                               |
|                          FU payload                           |
|                                                               |
|                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                               :   ...OPTIONAL RTP padding     |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
int RTPH264Unpack::rtpH264UnpackFU(std::vector<RTP_H264_FRAM_T>& rtp_h264_frames, const uint8_t * ptr, int bytes, uint32_t timestamp, int fu_b)
{
	int n;
	uint8_t fuheader;
	//uint16_t don;

	n = fu_b ? 4 : 2;
	if (bytes < n)
		return -EINVAL; // error

	if (size_ + bytes - n + 1 /*NALU*/ > capacity_)
	{
		void* p = NULL;
		int l_size = size_ + bytes + 128000 + 1;
		p = realloc((void *)ptr_, l_size);
		if (!p)
		{
			// set packet lost flag
			flags_ = RTP_PAYLOAD_FLAG_PACKET_LOST;
			size_ = 0;
			return -ENOMEM; // error
		}
		ptr_ = (uint8_t*)p;
		capacity_ = l_size;
	}

	fuheader = ptr[1];
	//don = nbo_r16(ptr + 2);
	if (FU_START(fuheader))
	{
		assert(0 == size_);
		size_ = 1; // NAL unit type byte
		ptr_[0] = (ptr[0]/*indicator*/ & 0xE0) | (fuheader & 0x1F);
		assert(H264_NAL(ptr_[0]) > 0 && H264_NAL(ptr_[0]) < 24);
	}
	else
	{
		if (0 == size_)
		{
			assert(0);
			flags_ = RTP_PAYLOAD_FLAG_PACKET_LOST;
			return 0; // packet discard
		}
		assert(size_ > 0);
	}

	if (bytes > n)
	{
		assert(capacity_ >= size_ + bytes - n);
		memmove((void*)(ptr_ + size_), ptr + n, bytes - n);
		size_ += bytes - n;
	}

	if (FU_END(fuheader))
	{
		pack(rtp_h264_frames, ptr_, size_, timestamp, flags_);
		flags_ = 0;
		size_ = 0; // reset
	}

	return 1; // packet handled
}

int RTPH264Unpack::RTPH264UnpackInput(std::vector<RTP_H264_FRAM_T>& rtp_h264_frames, const void * packet, int bytes)
{
	unsigned char nal;
	RTP_PACKET_T pkt;
	rtp_h264_frames.clear();

	if (0 != RTPPacketDeserialize(&pkt, packet, bytes) || pkt.payloadlen < 1)
	{
		LogError("no memory");
		return -EINVAL;
	}
	if (-1 == flags_)
	{
		flags_ = 0;
		seq_ = (uint16_t)(pkt.rtp.seq - 1); // disable packet lost
	}

	if ((uint16_t)pkt.rtp.seq != (uint16_t)(seq_ + 1))
	{
		flags_ = RTP_PAYLOAD_FLAG_PACKET_LOST;
		size_ = 0; // discard previous packets
	}
	seq_ = (uint16_t)pkt.rtp.seq;

	assert(pkt.payloadlen > 0);
	nal = ((unsigned char *)pkt.payload)[0];

	switch (nal & 0x1F)
	{
	case 0: // reserved
	case 31: // reserved
		assert(0);
		return 0; // packet discard

	case 24: // STAP-A
		return rtpH264UnpackSTAP(rtp_h264_frames, (const uint8_t*)pkt.payload, pkt.payloadlen, pkt.rtp.timestamp, 0);
	case 25: // STAP-B
		return rtpH264UnpackSTAP(rtp_h264_frames, (const uint8_t*)pkt.payload, pkt.payloadlen, pkt.rtp.timestamp, 1);
	case 26: // MTAP16
		return rtpH264UnpackMTAP(rtp_h264_frames, (const uint8_t*)pkt.payload, pkt.payloadlen, pkt.rtp.timestamp, 2);
	case 27: // MTAP24
		return rtpH264UnpackMTAP(rtp_h264_frames, (const uint8_t*)pkt.payload, pkt.payloadlen, pkt.rtp.timestamp, 3);
	case 28: // FU-A
		return rtpH264UnpackFU(rtp_h264_frames, (const uint8_t*)pkt.payload, pkt.payloadlen, pkt.rtp.timestamp, 0);
	case 29: // FU-B
		return rtpH264UnpackFU(rtp_h264_frames, (const uint8_t*)pkt.payload, pkt.payloadlen, pkt.rtp.timestamp, 1);

	default: // 1-23 NAL unit
		pack(rtp_h264_frames, (const uint8_t*)pkt.payload, pkt.payloadlen, pkt.rtp.timestamp, flags_);
		flags_ = 0;
		size_ = 0;
		return 1; // packet handled
	}
}

int RTPH264Unpack::RTPPacketDeserialize(RTP_PACKET_T * pkt, const void * data, int bytes)
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

void RTPH264Unpack::pack(std::vector<RTP_H264_FRAM_T>& rtp_h264_frames, const uint8_t * packet, int bytes, uint32_t timestamp, int flags)
{
	const uint8_t start_code[] = { 0x0, 0x0, 0x0, 0x1 };
	RTP_H264_FRAM_T rtp_h264_frame(bytes + 4);	// º”…œstart code
	rtp_h264_frame.size = bytes + 4;
	rtp_h264_frame.flags = flags;
	rtp_h264_frame.timestamp = timestamp;
	memcpy(rtp_h264_frame.data.get(), start_code, 4);
	memcpy(rtp_h264_frame.data.get() + 4, packet, bytes);
	rtp_h264_frames.push_back(rtp_h264_frame);
	uint8_t nalu_type = packet[0] & 0x1f;
	if (5 == nalu_type || 7 == nalu_type || 8 == nalu_type)
	{
		
		if (5 == nalu_type)
		{
			LogDebug("nalu_type = %d, timestamp = %u", nalu_type, timestamp);
		}
		else
		{
			LogDebug("nalu_type = %d", nalu_type);
		}
	}
}

