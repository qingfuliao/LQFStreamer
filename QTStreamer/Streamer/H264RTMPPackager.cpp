#include "H264RTMPPackager.h"
#include <iostream>
#include "LogUtil.h"

//************************************
// Method:    Pack
// FullName:  H264RTMPPackager::Pack
// Access:    virtual public 
// Returns:   RTMPPacket &
// Qualifier: const
// Parameter: char * buf
// Parameter: const char * data H264的数据，不包括start code
// Parameter: int length
//************************************
void H264RTMPPackager::Pack(RTMPPacket *packet, char* buf, const char* data, int length) const
{
	char *body = buf + RTMP_MAX_HEADER_SIZE;

	RTMPPacket_Reset(packet);
	packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
	packet->m_packetType = RTMP_PACKET_TYPE_VIDEO;
	packet->m_nChannel = 0x04;
	packet->m_hasAbsTimestamp = 0;
	packet->m_nBodySize = length + 9;	// + 9，因为出了H264的数据，还包含了比如 frame type, data length等信息
	packet->m_body = body;

	// 固定2字节
	*(body++) = (data[0] & 0x1f) == 0x05 ? 0x17 : 0x27;	// frame type : 1关键帧、2非关键帧 (4 bit)  + CodecID : 7表示AVC  (4 bit)
	*(body++) = 0x01;									// fixed : 0x01（NALU） 

	*(body++) = 0x00;					// 固定3字节
	*(body++) = 0x00;
	*(body++) = 0x00;
	
	*(body++) = (length >> 24) & 0xff;	// 固定4字节 data length : 长度信息（4 byte）
	*(body++) = (length >> 16) & 0xff;
	*(body++) = (length >> 8) & 0xff;
	*(body++) = (length) & 0xff;

	memcpy(body, data, length);
}

// 一般第一个视频Tag会封装视频编码的总体描述信息(AVC sequence header),
// 就是AVCDecoderConfigurationRecord结构(ISO/IEC 14496-15 AVC file format中规定).
void H264RTMPPackager::Metadata(RTMPPacket *packet, char * buf, uint8_t * sps, int sps_len, uint8_t * pps, int pps_len) const
{
	char *body = buf + RTMP_MAX_HEADER_SIZE;

	RTMPPacket_Reset(packet);
	packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
	packet->m_packetType = RTMP_PACKET_TYPE_VIDEO;
	packet->m_nChannel = 0x04;		// 音视频用04通道
	packet->m_hasAbsTimestamp = 0;

	packet->m_body = body;
	/*AVC head*/
	int i = 0;
	body[i++] = 0x17;	// frame type : 1关键帧、2非关键帧 (4 bit)  + CodecID : 7表示AVC  (4 bit)
	body[i++] = 0x00;	// fixed : 0x00（AVCDecoderConfigurationRecord） 

	body[i++] = 0x00;	// 固定3字节
	body[i++] = 0x00;
	body[i++] = 0x00;

	// AVC sequence header其实就是AVCDecoderConfigurationRecord
	// AVCDecoderConfigurationRecord
	body[i++] = 0x01;			// configurationVersion (1 byte)	0x01 版本
	body[i++] = *(sps + 1);		// AVCProfileIndication(1 byte)		sps[1] Profile
	body[i++] = *(sps + 2);		// profile_compatibility (1 byte)	sps[2] 兼容性
	body[i++] = *(sps + 3);		// AVCLevelIndication (1 byte)		sps[3] Profile level
	body[i++] = 0xff;			// lengthSizeMinusOne(低2bit,高6bit默认为111111)是H264视频中NALU的长度,计算方法是1+(lengthSizeMinusOne & 3), 
								//	NALU的长度怎么会一直都是4呢? 其实这不是NALU的长度, 而是NALU中, 表示长度的那个字段的长度是4字节

	// sps
	body[i++] = 0xe1;			//‭SPS个数（低5bit表示,高3bit默认为111），通常为0xe1个数= numOfSequenceParameterSets & 01F
	body[i++] = (sps_len >> 8) & 0xff;	// sps data length (2 byte)			sps长度
	body[i++] = sps_len & 0xff;
	memcpy(&body[i], sps, sps_len);		// sps实际内容
	i += sps_len;

	/*pps*/
	body[i++] = 0x01;					// pps number (1 byte)			0x01 pps的个数
	body[i++] = (pps_len >> 8) & 0xff;	// pps data length (2 byte)		pps长度
	body[i++] = (pps_len) & 0xff;
	memcpy(&body[i], pps, pps_len);		// pps data						pps内容
	i += pps_len;

	packet->m_nBodySize = i;				
}
// 都不带start code
// data[0] data[1] sps_len长度，data[2]~data[sps_len+2] sps内容;
// data[2 + sps_len] data[2 + sps_len +1] pps_len长度，data[data + 2 + sps_len + 2]~data[结束] pps内容;
void H264RTMPPackager::Metadata(RTMPPacket *packet, char* buf, const char* data, int length) const
{
	char *body = buf + RTMP_MAX_HEADER_SIZE;

	uint8_t * sps;
	int sps_len;
	uint8_t * pps;
	int pps_len;

	sps = (uint8_t *)(data +2);				// 
	sps_len = (data[0] << 8) | data[1];

	pps = (uint8_t *)(data + 2 + sps_len + 2);
	pps_len = (data[2 + sps_len] << 8) | data[2 + sps_len + 1];

	if (length != 2 + sps_len + 2 + pps_len)
	{
		LogError("length is error");
	}

	Metadata(packet, buf, sps, sps_len, pps, pps_len);
}

