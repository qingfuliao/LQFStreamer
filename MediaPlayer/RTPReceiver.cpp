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

RTPReceiver::RTPReceiver(const int queue_size)
{
	
	rtp_recv_pkt_q_.reset(new RingBuffer<LQF::AVPacket>(queue_size));
}

RTPReceiver::~RTPReceiver()
{
	jrtp_sess_.BYEDestroy(RTPTime(10, 0), 0, 0);
}

bool RTPReceiver::Init(RTP_CONNECT_PARAM_T & connect_param)
{
	payload_type_ = connect_param.payload_type;
	if (RTP_PAYLOAD_TYPE_H264 == payload_type_)
	{
		h264_rtp_unpack_.reset(new RTPH264Unpack());
		rtp_rx_file = fopen("rtp_rx.h264", "wb+");
	}
	else if (RTP_PAYLOAD_TYPE_AAC == payload_type_)
	{
		aac_rtp_unpack_.reset(new RTPAACUnpack(connect_param.audio_param.profile,
			connect_param.audio_param.frequency_index, 
			connect_param.audio_param.channel_configuration));
		rtp_rx_file = fopen("rtp_rx.aac", "wb+");
	}
	else
	{
		LogError("unsport payload type = %d", payload_type_);
		return false;
	}


	sessparams.SetOwnTimestampUnit(connect_param.timestamp_unit);
	sessparams.SetAcceptOwnPackets(true);

	if (connect_param.enable_rtp_recv)
	{
		transparams.SetPortbase(connect_param.listen_port);	// 监听的端口
		enable_rtp_recv_ = true;
	}
	int status = jrtp_sess_.Create(sessparams, &transparams);
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
		status = jrtp_sess_.AddDestination(addr);
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
	return rtp_recv_pkt_q_->Pop(pkt);
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
		jrtp_sess_.BeginDataAccess();
		if (jrtp_sess_.GotoFirstSourceWithData())
		{
			do
			{
				while ((pack = jrtp_sess_.GetNextPacket()) != NULL)
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
								if (rtp_rx_file != NULL)
								{
									fwrite(nalu_data, 1, nalu_size, rtp_rx_file);
								}

								LQF::AVPacket rtp_packet(nalu_size);
								rtp_packet.size = nalu_size;
								rtp_packet.type = VIDEO_PACKET;
								rtp_packet.timestamp = (uint32_t)timestamp;
								memcpy(rtp_packet.buffer.get(), nalu_data, nalu_size);
								if (!rtp_recv_pkt_q_->Push(std::move(rtp_packet)))
								{
									LogError("push h264 failed");
								}
							}
						}
					}
					else if(pack->GetPayloadType() == RTP_PAYLOAD_TYPE_AAC) //
					{
						std::vector<RTP_AAC_FRAM_T> rtp_aac_frames;
						if (aac_rtp_unpack_->RTPAACUnpackInput(rtp_aac_frames,
							pack->GetPacketData(), pack->GetPacketLength()) == 1)
						{
							for (int i = 0; i < rtp_aac_frames.size(); i++)
							{
								uint8_t *aac_data = rtp_aac_frames.at(i).data.get();
								int aac_size = rtp_aac_frames.at(i).size;
								uint32_t timestamp = rtp_aac_frames.at(i).timestamp;
								int flags = rtp_aac_frames.at(i).flags;
								if (RTP_PAYLOAD_FLAG_PACKET_LOST == flags)
								{
									LogDebug("Lost packet");
								}
								if (rtp_rx_file != NULL)
								{
									fwrite(aac_data, 1, aac_size, rtp_rx_file);
								}

								LQF::AVPacket rtp_packet(aac_size);
								rtp_packet.size = aac_size;
								rtp_packet.type = AUDIO_PACKET;
								rtp_packet.timestamp = (uint32_t)timestamp;
								memcpy(rtp_packet.buffer.get(), aac_data, aac_size);
								if (!rtp_recv_pkt_q_->Push(std::move(rtp_packet)))
								{
									LogError("push aac failed");
								}
							}
						}
					}
					else
					{
						LogDebug("!!!  GetPayloadType = %d !!!! \n ", pack->GetPayloadType());
					}

					jrtp_sess_.DeletePacket(pack);
				}
			} while (jrtp_sess_.GotoNextSourceWithData());
		}
				
#ifndef RTP_SUPPORT_THREAD
			status = jrtp_sess_.Poll();
			checkerror(status);
#endif // !RTP_SUPPORT_THREAD
		jrtp_sess_.EndDataAccess();
		Sleep(5);
	} // end of while(true)

	LogDebug("exit");
}

#endif
