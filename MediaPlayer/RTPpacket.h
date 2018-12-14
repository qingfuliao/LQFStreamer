#pragma once

#include "RTPHeader.h"

#define RTP_FIXED_HEADER 12

typedef struct _rtp_packet
{
	RTP_HEADER_T rtp;
	uint32_t csrc[16];
	const void* extension; // extension(valid only if rtp.x = 1)
	uint16_t extlen; // extension length in bytes
	uint16_t reserved; // extension reserved
	const void* payload; // payload
	int payloadlen; // payload length in bytes
}RTP_PACKET_T;