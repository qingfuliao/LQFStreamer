#if 1
#include "RTPStream.h"
#include "LogUtil.h"
#include "TimerManager.h"
#include <rtppacket.h>

#define PATH_MAX 255
#define VIDEO_RTP_FRAME_LEN_MAX		(1024*500)
#define Y_PLOAD_TYPE 96 

#define checkerror(rtperr) { \
	if (rtperr < 0) {	\
		LogError("ERROR: %s", RTPGetErrorString(rtperr).c_str() );	\
		return false;	\
	}	\
}

RTPStream::RTPStream()
{
	video_format_ = VIDEO_FORMAT_H264;

	// rtp
	rtp_vid_is_connect_ = FALSE;
	rtp_video_ts_ = 0;
	enable_rtp_recv_ = enable_rtp_send_ = false;

	h264_rtp_unpack_.reset(new RTPH264Unpack(RTP_PAYLOAD_TYPE_H264));
	h264_rtp_orig_unpack_.reset(new RTPH264Unpack(RTP_PAYLOAD_TYPE_H264));
	h264_rtp_pack_.reset(new RTPH264Pack(0));

	vid_send_pkt_q_.reset(new LQFRingBuffer<LQF::AVPacket>(30));
	vid_recv_pkt_q_.reset(new LQFRingBuffer<LQF::AVPacket>(30));
	m_file_rtp_vrx = fopen("rtp_vrx.h264", "wb+");	// 用来调试
	m_file_vtx = fopen("vtv.h264", "wb+");	// 用来调试
	m_file_vrx = fopen("vrx.h264", "wb+");
}

RTPStream::~RTPStream()
{
}

bool RTPStream::Init(RTP_CONNECT_PARAM_T & video_param, 
	RTP_CONNECT_PARAM_T & audio_param)
{
	// 这里目前处理发送
	int status;
	
	jrtp_sessparams_video_.SetUsePredefinedSSRC(true);  //设置使用预先定义的SSRC    
	jrtp_sessparams_video_.SetOwnTimestampUnit(video_param.timestamp_unit); /* 设置采样间隔 */
	jrtp_sessparams_video_.SetAcceptOwnPackets(true);   //接收自己发送的数据包  
	jrtp_transparams_video_.SetPortbase(video_param.listen_port);

	// 创建RTP会话
	status = jrtp_sess_video_.Create(jrtp_sessparams_video_, &jrtp_transparams_video_);
	checkerror(status);
	uint8_t destip[] = { video_param.dest_ip[0], video_param.dest_ip[1],
		video_param.dest_ip[2],  video_param.dest_ip[3] };
	RTPIPv4Address addr(destip, video_param.dest_port);
	// 指定RTP数据接收端
	status = jrtp_sess_video_.AddDestination(addr);
	checkerror(status);

	// 设置RTP会话默认参数
	jrtp_sess_video_.SetTimestampUnit(video_param.timestamp_unit);
	jrtp_sess_video_.SetDefaultTimestampIncrement(65);/* 设置时间戳增加间隔 */
	jrtp_sess_video_.SetDefaultPayloadType(video_param.payload_type);
	jrtp_sess_video_.SetDefaultMark(false);

	enable_rtp_send_ = true;
	rtp_vid_is_connect_ = true;

	return true;
}

bool RTPStream::Connect()
{
	return Thread::Start();
}

bool RTPStream::Disconnect()
{
	rtp_vid_is_connect_ = false;
	jrtp_sess_video_.BYEDestroy(RTPTime(10, 0), 0, 0);

	return Thread::Stop();
}

bool RTPStream::PushPacket(LQF::AVPacket & pkt)
{
	return vid_send_pkt_q_->Push(std::move(pkt));
}

bool RTPStream::PopPacket(LQF::AVPacket & pkt)
{
	return false;
}
uint32_t g_time_stamp = 0;
void RTPStream::Run(void)
{
	int			get_data_len;
	uint8_t		*p_codec_buf = NULL;
	uint8_t		*p_send_buf = NULL;		// 需要申请一段buffer
	uint8_t		*p_rtp_pkt_buf = NULL;	// 和p_send_buf共用
	uint8_t		*p_recv_buf = NULL;		// 不需要请求内存
	uint8_t		*p_frame_buf = NULL;
	int			h264_len = 0;
	int			frame_len = 0;


	p_send_buf = new uint8_t[VIDEO_RTP_FRAME_LEN_MAX];
	p_rtp_pkt_buf = p_send_buf + 14;	// 留取部分空间给RTP头部
										/* Create a set */
	uint32_t recv_ts = 0;

	uint8_t *loaddata;
	RTPPacket *pack;
	int status;
	int len;
	uint16_t seq = 0;
	uint16_t pre_seq = 0;
	while (true)
	{
		if (requet_abort_)
		{
			break;
		}

		if (rtp_vid_is_connect_)
		{
			// 接收
			if (enable_rtp_recv_)
			{
				get_data_len = 0;
				jrtp_sess_video_.BeginDataAccess();
				if (jrtp_sess_video_.GotoFirstSourceWithData())
				{
					do
					{
						while ((pack = jrtp_sess_video_.GetNextPacket()) != NULL)
						{
							seq = pack->GetSequenceNumber();
							if ((pre_seq + 1) != seq)
							{
								LogDebug("pre_seq:%d,seq:%d\n", pre_seq, seq);
							}
							pre_seq = seq;

							loaddata =  pack->GetPayloadData();
							len = pack->GetPayloadLength();
							//LogDebug("Recv len:%d, seq:%d", len, seq);
							if (pack->GetPayloadType() == 96) //H264
							{
								uint32_t cur_ts = 0;
								
								p_recv_buf = h264_rtp_unpack_->ParseRtpPacket(loaddata,
									len, get_data_len, cur_ts);
								if (m_file_vrx != NULL && p_recv_buf)
									fwrite(p_recv_buf, 1, get_data_len, m_file_vrx);
								if (p_recv_buf != NULL)
								{
									LQF::AVPacket rtp_packet(get_data_len);
									rtp_packet.size = get_data_len;
									rtp_packet.type = VIDEO_PACKET;
									rtp_packet.timestamp = (uint32_t)cur_ts;
									memcpy(rtp_packet.buffer.get(), p_recv_buf, get_data_len);
									vid_recv_pkt_q_->Push(std::move(rtp_packet));
								//	if (m_file_rtp_vrx != NULL)
										fwrite(p_recv_buf, 1, get_data_len, m_file_rtp_vrx);
									
									LogDebug("Recv get_data_len = %d, cur_ts = %u, rtp", get_data_len, rtp_packet.timestamp);
								}
							}
							else
							{
								LogDebug("!!!  GetPayloadType = %d !!!! \n ", pack->GetPayloadType());
							}

							jrtp_sess_video_.DeletePacket(pack);
						}
					} while (jrtp_sess_video_.GotoNextSourceWithData());
				}
				
#ifndef RTP_SUPPORT_THREAD
				status = jrtp_sess_video_.Poll();
				checkerror(status);
#endif // !RTP_SUPPORT_THREAD
				jrtp_sess_video_.EndDataAccess();
			}
			// 发送
			if (enable_rtp_send_)
			{
				// 取一帧数据进行发送
				LQF::AVPacket rtp_pkt;
				bool b_get_packet = false;

				// 发送视频包
				if (vid_send_pkt_q_->Pop(rtp_pkt))
				{
					h264_len = rtp_pkt.size;
					memcpy(p_rtp_pkt_buf, rtp_pkt.buffer.get(), h264_len);
					//if (m_file_vtx != NULL)
					//	fwrite(p_rtp_pkt_buf, 1, h264_len, m_file_vtx);
 					//LogDebug("Send h264_len = %d, timestamp = %u", h264_len, rtp_pkt.timestamp);
					uint64_t cur_time = LQF::AVTimer::GetInstance()->GetTime();
					uint8_t nal_type = p_rtp_pkt_buf[4];
					uint32_t send_count = 0;
					if (h264_len > 4)
					{
						// FU-A
						if (h264_rtp_pack_->Pack(p_rtp_pkt_buf, h264_len, rtp_pkt.timestamp, true))
						{
							int i_send = 0;
							do
							{
								p_frame_buf = h264_rtp_pack_->GetPacket(frame_len);
								if (p_frame_buf != NULL && frame_len > 12)
								{
									//status = jrtp_sess_video_.SendPacket((void *)p_frame_buf, frame_len);
									RTP_HDR_T rtp_header;
									memcpy(&rtp_header, p_frame_buf, 12);
									status = jrtp_sess_video_.SendPacketEx((void *)(p_frame_buf + 12), 
										frame_len - 12, 
										rtp_header.payloadtype, rtp_header.marker, 65,0, NULL, 0);
									//status = jrtp_sess_video_.SendRTPData((void *)p_frame_buf, frame_len,);

									if(status < 0)
										LogError("ERROR: %s", RTPGetErrorString(status).c_str());
								}
								//if (nal_type == 0x65)
								{
									if(send_count++ % 12 == 0)
										Sleep(8);
								}
							} while (p_frame_buf != NULL);
						}
					}
					if (nal_type == 0x65) // 计算每次发I帧的耗时，发送I帧时容易拥塞丢帧
					{
						LogDebug("send I frame size:%d, need time :%llu", h264_len,
							LQF::AVTimer::GetInstance()->GetTime() - cur_time);
					}
				}	
				else
				{
					Sleep(5);
				}
			}
			//m_rtp_video_ts += VIDEO_SAMPLE_RATE / m_resolution_ratio;
		}
	} // end of while(true)

	LogDebug("exit");
	if (p_send_buf)
	{
		delete[] p_send_buf;
	}
}

#endif
