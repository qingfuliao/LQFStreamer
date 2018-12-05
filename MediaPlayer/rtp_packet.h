#pragma once

#include "rtp_header.h"

#define RTP_FIXED_HEADER 12

struct rtp_packet_t
{
	rtp_header_t rtp;
	uint32_t csrc[16];
	const void* extension; // extension(valid only if rtp.x = 1)
	uint16_t extlen; // extension length in bytes
	uint16_t reserved; // extension reserved
	const void* payload; // payload
	int payloadlen; // payload length in bytes
};