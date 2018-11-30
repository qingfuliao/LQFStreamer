#if 1
#include "RTPReceiver.h"
#include "LogUtil.h"

#include <rtppacket.h>

#define checkerror(rtperr) { \
	if (rtperr < 0) {	\
		LogError("ERROR: %s", RTPGetErrorString(rtperr).c_str() );	\
		system("pause");	\
		exit(-1);	\
	}	\
}

RTPReceiver::RTPReceiver()
{
	rtp_video_ts_ = 0;

	h264_rtp_unpack_.reset(new RTPH264Unpack(96));

	vid_recv_pkt_q_.reset(new RingBuffer<LQF::AVPacket>(10));

	m_file_vrx = fopen("vrx.h264", "wb+");
}

RTPReceiver::~RTPReceiver()
{
}

bool RTPReceiver::Init(const uint16_t payload_type, const uint16_t base_port)
{
	sessparams.SetOwnTimestampUnit(1.0 / 1000.0);
	sessparams.SetAcceptOwnPackets(true);

	transparams.SetPortbase(base_port);	// 监听的端口
	int status = sess.Create(sessparams, &transparams);
	if (status < 0)
	{
		LogError("Create failed, %s", RTPGetErrorString(status).c_str());
		return false;
	}
	uint8_t dest_ip[] = { 127,0,0,1 };	// 对方IP和端口
	RTPIPv4Address addr(dest_ip, 8000);
	status = sess.AddDestination(addr);
	if (status < 0)
	{
		LogError("AddDestination failed, %s", RTPGetErrorString(status).c_str());
		return false;
	}

	enable_rtp_recv_ = true;
	return true;
}

bool RTPReceiver::Init(RTP_CONNECT_PARAM_T & connect_param)
{
	sessparams.SetOwnTimestampUnit(connect_param.timestamp_unit);
	sessparams.SetAcceptOwnPackets(true);

	if (connect_param.enable_rtp_recv)
	{
		transparams.SetPortbase(connect_param.listen_port);	// 监听的端口
		enable_rtp_recv_ = true;
	}
	int status = sess.Create(sessparams, &transparams);
	if (status < 0)
	{
		LogError("Create failed, %s", RTPGetErrorString(status).c_str());
		return false;
	}
	if (connect_param.enable_rtp_send)
	{
		uint8_t dest_ip[] = { connect_param.dest_ip[0], connect_param.dest_ip[1],	\
			connect_param.dest_ip[2],connect_param.dest_ip[3] };	// 对方IP和端口

		RTPIPv4Address addr(dest_ip, connect_param.dest_port);
		status = sess.AddDestination(addr);
		if (status < 0)
		{
			LogError("AddDestination failed, %s", RTPGetErrorString(status).c_str());
			return false;
		}
		
	}
	
	return true;
}

bool RTPReceiver::Start()
{
	return Thread::Start();
}

bool RTPReceiver::Stop()
{
	return Thread::Stop();;
}


bool RTPReceiver::PopPacket(LQF::AVPacket & pkt)
{
	return vid_recv_pkt_q_->Pop(pkt);
}

void RTPReceiver::Run(void)
{
	int			get_data_len;
	uint8_t		*p_recv_buf = NULL;		// 不需要请求内存
	uint8_t		*p_frame_buf = NULL;
	int			h264_len = 0;
	int			frame_len = 0;
										/* Create a set */
	uint32_t recv_ts = 0;

	uint8_t *loaddata;
	RTPPacket *pack;
	int status;
	int len;
	uint16_t seq = 0;
	uint16_t pre_seq = 0;
	LogDebug("Run........");
	while (true)
	{
		if (requet_abort_)
		{
			break;
		}

		get_data_len = 0;
		sess.BeginDataAccess();
		if (sess.GotoFirstSourceWithData())
		{
			do
			{
				while ((pack = sess.GetNextPacket()) != NULL)
				{
					seq = pack->GetSequenceNumber();
					if ((pre_seq + 1) != seq)
					{
						LogDebug("----------pre_seq:%d,seq:%d\n", pre_seq, seq);
					}
					pre_seq = seq;

					loaddata =  pack->GetPayloadData();
					len = pack->GetPayloadLength();
					static uint32_t s_rtp_recv_count = 0;
					//if(s_rtp_recv_count++ %90 == 0)
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
							fwrite(p_recv_buf, 1, get_data_len, m_file_vrx);
							if (s_rtp_recv_count++ % 90 == 0)
							LogDebug("Recv get_data_len = %d, timestamp = %u, rtp", get_data_len, rtp_packet.timestamp);
						}
					}
					else
					{
						LogDebug("!!!  GetPayloadType = %d !!!! \n ", pack->GetPayloadType());
					}

					sess.DeletePacket(pack);
				}
			} while (sess.GotoNextSourceWithData());
		}
				
#ifndef RTP_SUPPORT_THREAD
			status = sess.Poll();
			checkerror(status);
#endif // !RTP_SUPPORT_THREAD
		sess.EndDataAccess();
		Sleep(5);
	} // end of while(true)

	LogDebug("exit");
}

#endif
