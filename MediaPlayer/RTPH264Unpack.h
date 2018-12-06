#pragma once
#include "RTP.h"
#include <rtppacket.h>
#include <vector>
#include <memory>
using namespace std;

using namespace jrtplib;

/*
1、RTP Header解析
1)	V：RTP协议的版本号，占2位，当前协议版本号为2

2)	P：填充标志，占1位，如果P=1，则在该报文的尾部填充一个或多个额外的八位组，它们不是有效载荷的一部分。

3)	X：扩展标志，占1位，如果X=1，则在RTP报头后跟有一个扩展报头

4)	CC：CSRC计数器，占4位，指示CSRC 标识符的个数

5)	M: 标记，占1位，不同的有效载荷有不同的含义，对于视频，标记一帧的结束；对于音频，标记会话的开始。

6)	PT: 有效荷载类型，占7位，用于说明RTP报文中有效载荷的类型，如GSM音频、JPEM图像等,在流媒体中大部分是用来区分音频流和视频流的，这样便于客户端进行解析。

7)	序列号：占16位，用于标识发送者所发送的RTP报文的序列号，每发送一个报文，序列号增1。这个字段当下层的承载协议用UDP的时候，网络状况不好的时候可以用来检查丢包。同时出现网络抖动的情况可以用来对数据进行重新排序，序列号的初始值是随机的，同时音频包和视频包的sequence是分别记数的。

8)	时戳(Timestamp)：占32位，必须使用90 kHz 时钟频率。时戳反映了该RTP报文的第一个八位组的采样时刻。接收者使用时戳来计算延迟和延迟抖动，并进行同步控制。

9)	同步信源(SSRC)标识符：占32位，用于标识同步信源。该标识符是随机选择的，参加同一视频会议的两个同步信源不能有相同的SSRC。

10)	特约信源(CSRC)标识符：每个CSRC标识符占32位，可以有0～15个。每个CSRC标识了包含在该RTP报文有效载荷中的所有特约信源。
*/

// 具体见rfc6184

typedef struct _RTP_H264_FRAM_T
{
	_RTP_H264_FRAM_T()
	{
		size = 0;
		flags = 0;
		timestamp = 0;
	}

	_RTP_H264_FRAM_T(uint32_t size)
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
}RTP_H264_FRAM_T;


/**
 * 解释到一帧完整的数据才会返回
 */
class RTPH264Unpack
{
public:
	RTPH264Unpack(void);
	~RTPH264Unpack(void);
	int RTPH264UnpackInput(std::vector<RTP_H264_FRAM_T> &rtp_h264_frames, const void* packet, int bytes);
	///@return 0-ok, other-error
	int RTPPacketDeserialize(RTP_PACKET_T *pkt, const void* data, int bytes);
private:
	int rtpH264UnpackSTAP(std::vector<RTP_H264_FRAM_T> &rtp_h264_frames, const uint8_t* ptr,
		int bytes, uint32_t timestamp, int stap_b);
	int rtpH264UnpackMTAP(std::vector<RTP_H264_FRAM_T> &rtp_h264_frames,
		const uint8_t* ptr, int bytes, uint32_t timestamp, int n);
	int rtpH264UnpackFU(std::vector<RTP_H264_FRAM_T> &rtp_h264_frames,
		const uint8_t* ptr, int bytes, uint32_t timestamp, int fu_b);
	void pack(std::vector<RTP_H264_FRAM_T> &rtp_h264_frames, 
		const uint8_t *packet, int bytes, uint32_t timestamp, int flags);

	uint16_t seq_ = 0; // rtp seq
	uint8_t* ptr_ = NULL;
	int size_ = 0;
	int capacity_ = 0;

	int flags_ = -1;
};


