#pragma once
#include "RTP.h"
#include "RTPPack.h"
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

typedef enum
{
	RTP_H264_IDR = 5,
	RTP_H264_SPS = 7,
	RTP_H264_PPS = 8,
	RTP_H264_FU_A = 28,
}RTP_H264_NAL_TYPE_T;

class RTPH264Pack
{
public:

	typedef struct nal_msg_
	{
		bool b_end_of_frame;
		uint8_t type;		// NAL type
		uint8_t *start;	// pointer to first location in the send buffer
		uint8_t *end;	// pointer to last location in send buffer
		uint32_t size;
	} NAL_MSG_T;
	

	typedef struct rtp_info_
	{
		NAL_MSG_T	nal;	    // NAL information
		RTP_HDR_T	rtp_hdr;    // RTP header is assembled here
		int hdr_len;			// length of RTP header

		uint8_t *p_rtp;		// pointer to where RTP packet has beem assembled
		uint8_t *start;		// pointer to start of payload
		uint8_t *end;		// pointer to end of payload

		uint32_t s_bit;		// bit in the FU header
		uint32_t e_bit;		// bit in the FU header
		bool FU_flag;		// fragmented NAL Unit flag
	} RTP_INFO_T;
public:
	RTPH264Pack(const uint32_t ssrc = 0,
		const uint8_t payload_type = RTP_PAYLOAD_TYPE_H264,
		const uint16_t rtp_packet_max_size = RTP_PACKET_MAX_SIZE);
	~RTPH264Pack();

	bool Pack(uint8_t *nal_buf, uint32_t nal_size,
		uint32_t timestamp, bool end_of_frame);

	//循环调用Get获取RTP包，直到返回值为NULL
	uint8_t* GetPacket(int &out_packet_size);

	RTP_INFO_T GetRtpInfo() const;
	void SetRtpInfo(const RTP_INFO_T &RTP_Info);
private:
	unsigned int StartCode(uint8_t *cp);

	RTP_INFO_T rtp_info_;
	bool b_begin_nal_;
	uint16_t rtp_packet_max_size_;
};

/**
 * 解释到一帧完整的数据才会返回
 */
class RTPH264Unpack
{

#define RTP_VERSION 2
#define BUF_SIZE (1024 * 500)

	typedef struct
	{
		//LITTLE_ENDIAN
		uint16_t   cc : 4;		/* CSRC count                 */
		uint16_t   x : 1;		/* header extension flag      */
		uint16_t   p : 1;		/* padding flag               */
		uint16_t   v : 2;		/* packet type                */
		uint16_t   pt : 7;		/* payload type               */
		uint16_t   m : 1;		/* marker bit                 */

		uint16_t    seq;		/* sequence number            */
		uint32_t     ts;		/* timestamp                  */
		uint32_t     ssrc;		/* synchronization source     */
	} RTP_HDR_T;
public:
	RTPH264Unpack() {}
	RTPH264Unpack(uint8_t h264_playload_type = 96);
	~RTPH264Unpack(void);

	//pBuf为H264 RTP视频数据包，nSize为RTP视频数据包字节长度，outSize为输出视频数据帧字节长度。
	//返回值为指向视频数据帧的指针。输入数据可能被破坏。
	uint8_t* ParseRtpPacket(uint8_t * p_buf, uint16_t buf_size, int &out_size, uint32_t &timestamp);

	void resetPacket();

private:
	RTP_HDR_T rtp_header_;

	uint8_t *receive_buf_data_;

	bool b_sps_found_;
	bool b_found_key_frame_;
	bool m_bAssemblingFrame;
	bool b_pre_frame_finish_;
	uint8_t *receive_buf_start_;
	uint8_t *receive_buf_end_;
	uint32_t cur_receive_size_;

	uint16_t seq_num_;
	bool resync_ = true;

	uint8_t h264_playload_type_ = 96;
	uint32_t ssrc_;
};


