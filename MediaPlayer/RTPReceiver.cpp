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

	h264_rtp_unpack_.reset(new RTPH264Unpack());

	vid_recv_pkt_q_.reset(new RingBuffer<LQF::AVPacket>(10));

	m_file_vrx = fopen("vrx.h264", "wb+");
}

RTPReceiver::~RTPReceiver()
{
	jrtp_sess_video_.BYEDestroy(RTPTime(10, 0), 0, 0);
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
	int status = jrtp_sess_video_.Create(sessparams, &transparams);
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
		status = jrtp_sess_video_.AddDestination(addr);
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
	RTPPacket *pack;
	int status;
	uint16_t seq = 0;
	uint16_t pre_seq = 0;
	LogDebug("Run........");
	while (true)
	{
		if (requet_abort_)
		{
			break;
		}
		jrtp_sess_video_.BeginDataAccess();
		if (jrtp_sess_video_.GotoFirstSourceWithData())
		{
			do
			{
				while ((pack = jrtp_sess_video_.GetNextPacket()) != NULL)
				{
					if (pack->GetPayloadType() == RTP_PAYLOAD_TYPE_H264) //H264
					{
						std::vector<RTP_H264_FRAM_T> rtp_h264_frames;
						if(h264_rtp_unpack_->RTPH264UnpackInput(rtp_h264_frames,
							pack->GetPacketData(), pack->GetPacketLength()) == 1)
						{ 
							for (int i = 0; i < rtp_h264_frames.size(); i++)
							{
								uint8_t *nalu_data = rtp_h264_frames.at(i).data.get();
								int nalu_size = rtp_h264_frames.at(i).size;
								uint32_t timestamp = rtp_h264_frames.at(i).timestamp;
								int flags = rtp_h264_frames.at(i).flags;
								if (RTP_PAYLOAD_FLAG_PACKET_LOST == flags)
								{
									LogDebug("Lost packet");
								}
								if (m_file_vrx != NULL)
								{
									fwrite(nalu_data, 1, nalu_size, m_file_vrx);
								}

								LQF::AVPacket rtp_packet(nalu_size);
								rtp_packet.size = nalu_size;
								rtp_packet.type = VIDEO_PACKET;
								rtp_packet.timestamp = (uint32_t)timestamp;
								memcpy(rtp_packet.buffer.get(), nalu_data, nalu_size);
								vid_recv_pkt_q_->Push(std::move(rtp_packet));
							}
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
		Sleep(5);
	} // end of while(true)

	LogDebug("exit");
}

#endif
