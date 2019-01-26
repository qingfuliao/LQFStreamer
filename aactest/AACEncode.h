#ifndef _STREAMER_AAC_ENCODE_H_
#define _STREAMER_AAC_ENCODE_H_

#include <stdint.h>
#include "aacenc_lib.h"
#include "FDK_audio.h"
// 对应
#define PROFILE_AAC_LC		2				// AOT_AAC_LC
#define PROFILE_AAC_HE		5				// AOT_SBR
#define PROFILE_AAC_HE_v2	29				// AOT_PS PS, Parametric Stereo (includes SBR)  
#define PROFILE_AAC_LD		23				// AOT_ER_AAC_LD Error Resilient(ER) AAC LowDelay object
#define PROFILE_AAC_ELD		39				// AOT_ER_AAC_ELD AAC Enhanced Low Delay

typedef struct AudioEncoderConfig_
{
	int sample_rate;
	int channels;
	int bitrate;
	int profile;
}AudioEncoderConfig;


/*
支持的采样率sample_rate：8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000, 64000, 88200, 96000
*/
class AACEncode
{
public:
	AACEncode();
// 	int Init(const int sample_rate, const int channels, const int bit_rate, const int profile_aac);
// 	int Encode(const uint8_t * input, const int input_len, uint8_t * output, int &output_len);
// 	int GetPcmFrameLength();
// 	void DeInit();

private:
// 	HANDLE_AACENCODER fdk_aac_handle_;		// fdk-aac
// 	AACENC_InfoStruct fdk_aac_info_;		// fdk-aac
	int pcm_frame_len_;				// 每次送pcm的字节数
};
#endif // !__AAC_ENCODER_H__

