//#define __RTP_STREAM_H__
#ifndef __RTP_STREAM_H__
#define __RTP_STREAM_H__


#include "Media.h"
#include "RingBuffer.h"
#include "RtpH264Pack.h"
#include "Thread.h"
#include <rtpsession.h>
#include <rtpudpv4transmitter.h>
#include <rtpipv4address.h>
#include <rtpsessionparams.h>
#include <rtperrors.h>
#include <rtplibraryversion.h>
using namespace jrtplib;

typedef struct video_rtp_connect_param {
	const char		*p_local_ip;
	int				local_port;
	const char		*p_remote_ip;
	int				remote_port;
	int				payload_index;
	const char		*p_mime_type;
	int				sample_rate;
	bool			enable_rtp_send;
	bool			enable_rtp_recv;
	const char		*p_user_name;				 
	const char		*p_callee_name;			 
} VIDEO_RTP_CONNECT_PARAM_T;
/**
 * 提供RTP码流的接收和发送
 */
class RTPStream : public Thread
{
public:
#define VIDEO_FORMAT_H264			0
#define PAYLOAD_TYPE_H264			99
	const  char *video_type_h264 = "H264";
public:
	RTPStream();
	~RTPStream();
	// 初始化音视频推送参数
	bool Init(RTP_CONNECT_PARAM_T &video_param, RTP_CONNECT_PARAM_T &audio_param);
	bool Connect();		// 开始启动推送RTP码流
	bool Disconnect();	// 停止推送RTP码流
// 	bool RTPSendConnect(VIDEO_RTP_CONNECT_PARAM_T * p_param);	
// 	bool RTPRecvConnect(VIDEO_RTP_CONNECT_PARAM_T *p_param);
// 	bool RTPUnconnect(void);
	// 把音视频数据写入的发送队列，此时还没有直接发送，需要线程进行调度
	bool PushPacket(LQF::AVPacket &pkt);	// RTP还是分开来操作好些
	// 读取音视频数据
	bool PopPacket(LQF::AVPacket &pkt) ;
	void Run(void) override;
public:
	// 音视频队列分开存放
	std::shared_ptr<RingBuffer<LQF::AVPacket>> aud_send_pkt_q_;	// 音频发送队列
	std::shared_ptr<RingBuffer<LQF::AVPacket>> vid_send_pkt_q_;	// 视频发送队列

	std::shared_ptr<RingBuffer<LQF::AVPacket>> aud_recv_pkt_q_;	// 音频接收队列
	std::shared_ptr<RingBuffer<LQF::AVPacket>> vid_recv_pkt_q_;	// 视频接收队列

	bool rtp_vid_is_connect_ = false;
	bool rtp_aud_is_connect_ = false;
	int	video_format_;

	// video
	RTPSession jrtp_sess_video_;
	//uint16_t portbase = 8666;
	//uint16_t destport = 8664;
	RTPUDPv4TransmissionParams jrtp_transparams_video_;
	RTPSessionParams jrtp_sessparams_video_;

	uint32_t				rtp_video_ts_ = 0;
	bool					enable_rtp_send_ = false;
	bool					enable_rtp_recv_ = false;

	std::shared_ptr<RTPH264Unpack> h264_rtp_unpack_;
	std::shared_ptr<RTPH264Unpack> h264_rtp_orig_unpack_;
	std::shared_ptr<RTPH264Pack> h264_rtp_pack_;
	FILE		*m_file_rtp_vrx = NULL;
	FILE		*m_file_vrx = NULL;
	FILE		*m_file_vtx = NULL;
};

#endif // !__RTP_STREAM_H__