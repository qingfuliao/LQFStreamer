/*!
 * \file AACRTMPPackager.h
 * \date 2018/11/15 11:34
 *
 * \author Liao Qingfu
 * Contact:	QQ群：782508536	
 *
 * \brief 封装AAC RTMP packet
 *
*/
#pragma once
#include "RTMPPackager.h"
#include "AACEncode.h"
/*
* Audio Specific Config
5 bits: object type
	if (object type == 31)
		6 bits + 32: object type
4 bits: frequency index
	if (frequency index == 15)
		24 bits: frequency
4 bits: channel configuration
var bits: AOT Specific Config
这里只讨论固定部分，不讨论可变部分
*/
typedef struct tagAudioSpecificConfig
{
	//little endian
		
	uint8_t type : 5;		/* 编码结构类型 5bit
							1: AAC Main							2 : AAC LC(Low Complexity)
							3 : AAC SSR(Scalable Sample Rate)	4 : AAC LTP(Long Term Prediction)
							5 : SBR(Spectral Band Replication)  6 : AAC Scalable
							*/
	uint8_t samplerate : 4;/*采样率 4bit 
							1: 88200 Hz			2 : 64000 Hz
							3 : 48000 Hz		4 : 44100 Hz
							5 : 32000 Hz		6 : 24000 Hz
							7 : 22050 Hz		8 : 16000 Hz
							9 : 12000 Hz		10 : 11025 Hz
							11 : 8000 Hz		12 : 7350 Hz
							*/
	uint8_t channel_num : 4;/*声道数
							0: Defined in AOT Specifc Config
							1: 1 channel: front-center
							2: 2 channels: front-left, front-right
							3: 3 channels: front-center, front-left, front-right
							4: 4 channels: front-center, front-left, front-right, back-center
							5: 5 channels: front-center, front-left, front-right, back-left, back-right
							6: 6 channels: front-center, front-left, front-right, back-left, back-right, LFE-channel
							7: 8 channels: front-center, front-left, front-right, side-left, side-right, back-left, back-right, LFE-channel
							8-15: Reserved
							*/
	uint8_t tail : 3;//最后3位固定为0
}AudioSpecificConfig, *AudioSpecificConfigPtr;

class AACRTMPPackager : public RTMPPackager 
{
public:
	static const int AAC_MAIN	= 1;
	static const int AAC_LC		= 2;
	static const int AAC_SSR	= 3;
	virtual void Pack(RTMPPacket *packet, char* buf, const char* data, int length)  const override;
	virtual void Metadata(RTMPPacket *packet, char* buf, const char* data, int length) const override;

	static int GetAudioSpecificConfig(uint8_t* data, const uint32_t profile, const uint32_t samplerate, const uint32_t channel_num);
	static void PacketADTSHeader(uint8_t *packet, int packet_len, int profile, int freq_idx, int channel_config);
};

