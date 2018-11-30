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


/**
 * 提供RTP码流的接收和发送
 */
class RTPReceiver : public Thread
{
public:
	const  char *video_type_h264 = "H264";
public:
	RTPReceiver();
	~RTPReceiver();
	/* 初始化
	* payload_type 处理的负载类型
	* base_port	   监听的端口
	*/
	bool Init(const uint16_t payload_type, const uint16_t base_port);
	bool Init(RTP_CONNECT_PARAM_T &connect_param);
	bool Start();
	bool Stop();
	bool PopPacket(LQF::AVPacket &pkt) ;
	void Run(void) override;
private:

	std::shared_ptr<RingBuffer<LQF::AVPacket>> vid_recv_pkt_q_;	// 视频接收队列

	int	video_format_;

	// video
	RTPSession sess;
	RTPUDPv4TransmissionParams transparams;
	RTPSessionParams sessparams;

	uint32_t				rtp_video_ts_ = 0;
	bool					enable_rtp_send_ = false;
	bool					enable_rtp_recv_ = false;

	std::shared_ptr<RTPH264Unpack> h264_rtp_unpack_;
	std::shared_ptr<RTPH264Pack> h264_rtp_pack_;
	FILE		*m_file_vrx = NULL;
	FILE		*m_file_vtx = NULL;
};

#endif // !__RTP_STREAM_H__