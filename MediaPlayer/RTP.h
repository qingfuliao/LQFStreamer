#pragma once

#include <stdint.h>

#include "rtp_header.h"
#include "rtp_util.h"
#include "rtp_packet.h"

#define RTP_HEADER_LEN		12		// 
#define RTP_PACKET_MAX_SIZE	1300	// 每个RTP包最大程度，
#define RTP_VERSION			2
#define RTP_H264_FRAME_MAX_SIZE (1024 * 500)

// 定义payload type
enum RTP_PAYLOAD_TYPE
{
	RTP_PAYLOAD_TYPE_PCMU = 0,
	RTP_PAYLOAD_TYPE_PCMA = 8,
	RTP_PAYLOAD_TYPE_JPEG = 26,
	RTP_PAYLOAD_TYPE_H264 = 96,
	RTP_PAYLOAD_TYPE_H265 = 97,
	RTP_PAYLOAD_TYPE_OPUS = 98,
	RTP_PAYLOAD_TYPE_AAC = 99,	// https://tools.ietf.org/html/rfc6416
	RTP_PAYLOAD_TYPE_G726 = 100,
	RTP_PAYLOAD_TYPE_G726_16 = 101,
	RTP_PAYLOAD_TYPE_G726_24 = 102,
	RTP_PAYLOAD_TYPE_G726_32 = 103,
	RTP_PAYLOAD_TYPE_G726_40 = 104,
	RTP_PAYLOAD_TYPE_SPEEX = 105,
};


typedef struct _rtp_hdr
{
#ifdef RTP_BIG_ENDIAN
	uint8_t version : 2;
	uint8_t padding : 1;
	uint8_t extension : 1;
	uint8_t csrccount : 4;

	uint8_t marker : 1;
	uint8_t payloadtype : 7;
#else // little endian
	uint8_t   csrccount : 4;	/* CSRC count                 */
	uint8_t   extension : 1;	/* header extension flag      */
	uint8_t   padding : 1;		/* padding flag               */
	uint8_t   version : 2;		/* packet type                */

	uint8_t   payloadtype : 7;	/* payload type               */
	uint8_t   marker : 1;		/* marker bit                 */
#endif // RTP_BIG_ENDIAN

	// 小端模式
	uint16_t    sequencenumber;		/* sequence number            */
	uint32_t	timestamp;			/* timestamp                  */
	uint32_t    ssrc;				/* synchronization source     */

	void set_mark(bool m)
	{
		marker = (m ? 1 : 0);
	}

	void set_type(uint8_t pt)
	{
		payloadtype = pt;
	}

	void set_version(uint8_t ver)
	{
		version = ver;
	}

	void set_seq_num(uint16_t seqnum)
	{
		sequencenumber = ((seqnum & 0xff00u) >> 8) | ((seqnum & 0x00ffu) << 8);
	}

	void set_time_stamp(uint32_t ts)
	{
		timestamp = ((ts & 0x000000ffu) << 24) |
			((ts & 0x0000ff00u) << 8) |
			((ts & 0x00ff0000u) >> 8) |
			((ts & 0xff000000u) >> 24);
	}

	void set_ssrc(uint32_t num)
	{
		ssrc = ((num & 0x000000ffu) << 24) |
			((num & 0x0000ff00u) << 8) |
			((num & 0x00ff0000u) >> 8) |
			((num & 0xff000000u) >> 24);
	}
} RTP_HDR_T;

typedef struct rtp_connect_param 
{
	uint8_t			listen_ip[4];		// 接收数据需要连接的IP
	uint16_t		listen_port;		// 接收数据需要监听的端口
	uint8_t			dest_ip[4];			// 发送数据时远程的IP
	uint16_t		dest_port;			// 发送数据时远程的端口
	int				payload_type;
	uint8_t			*p_mime_type;
	double			timestamp_unit;
	bool			enable_rtp_send;
	bool			enable_rtp_recv;
}RTP_CONNECT_PARAM_T;







