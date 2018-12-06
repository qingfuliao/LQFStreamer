#pragma once

#include <stdint.h>

#include "RTPHeader.h"
#include "RTPUtil.h"
#include "RTPpacket.h"

#define RTP_HEADER_LEN		12		// 
#define RTP_PACKET_MAX_SIZE	1400	// 每个RTP包最大程度，
#define RTP_VERSION			2
#define RTP_H264_FRAME_MAX_SIZE (1024 * 500)

// 定义payload type			// 这里的payload type不一定都是对的
enum RTP_PAYLOAD_TYPE
{
	RTP_PAYLOAD_TYPE_PCMU = 0,
	RTP_PAYLOAD_TYPE_PCMA = 8,
	RTP_PAYLOAD_TYPE_JPEG = 26,
	RTP_PAYLOAD_TYPE_H264 = 96,
	RTP_PAYLOAD_TYPE_AAC = 97,	// https://tools.ietf.org/html/rfc6416
// 	RTP_PAYLOAD_TYPE_H265 = 97,
	RTP_PAYLOAD_TYPE_OPUS = 98,
	RTP_PAYLOAD_TYPE_G726 = 100,
	RTP_PAYLOAD_TYPE_G726_16 = 101,
	RTP_PAYLOAD_TYPE_G726_24 = 102,
	RTP_PAYLOAD_TYPE_G726_32 = 103,
	RTP_PAYLOAD_TYPE_G726_40 = 104,
	RTP_PAYLOAD_TYPE_SPEEX = 105,
};


typedef struct rtp_audio_param
{
	// AAC参数
	uint8_t profile = 2;
	uint8_t frequency_index = 4;	//  4: 44100 Hz
	uint8_t channel_configuration = 2;
}RTP_AUDIO_PARAM_T;

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
	RTP_AUDIO_PARAM_T audio_param;
}RTP_CONNECT_PARAM_T;







