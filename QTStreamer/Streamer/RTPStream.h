//#define __RTP_STREAM_H__
#ifndef __RTP_STREAM_H__
#define __RTP_STREAM_H__


#include "Media.h"
#include "LQFRingBuffer.h"
#include "RtpH264Pack.h"
#include "Thread.h"
#include <rtpsession.h>
#include <rtpudpv4transmitter.h>
#include <rtpipv4address.h>
#include <rtpsessionparams.h>
#include <rtperrors.h>
#include <rtplibraryversion.h>
#include <rtcpsrpacket.h>
#include <rtpsources.h>
#include <rtpsourcedata.h>
#include <rtcprrpacket.h>
#include <rtcpbyepacket.h>
#include "LogUtil.h"
using namespace jrtplib;
class CRTPRTPSession : public RTPSession
{
protected:
	void OnRTCPCompoundPacket(RTCPCompoundPacket *pack, const RTPTime &receivetime, \
		const RTPAddress *senderaddress)
	{
		std::cout << "OnRTCPCompoundPacket: data:" << pack->GetCompoundPacketData() << std::endl;
		RTCPPacket *rtcppack;
		pack->GotoFirstPacket();
		while ((rtcppack = pack->GetNextPacket()) != 0)
		{
			if (rtcppack->IsKnownFormat())
			{
				switch (rtcppack->GetPacketType())
				{
				case RTCPPacket::SR:
				{
					RTCPSRPacket *p = (RTCPSRPacket *)rtcppack;
					// NTP timestamp
					RTPNTPTime ntp = p->GetNTPTimestamp();
					int64_t ttime = ntp.GetMSW();
					ttime -= 0x83aa7e80;
					struct tm *tt = localtime(&ttime);
					int64_t ms = (int64_t)ntp.GetLSW() * 1000000 / 0x100000000;
					char sz_time[64];
					sprintf(sz_time, "%04d/%02d/%02d %02d:%02d:%02d %lldms", tt->tm_year + 1900,
						tt->tm_mon + 1, tt->tm_mday, tt->tm_hour, tt->tm_min, tt->tm_sec, ms / 1000);
					LogDebug("SR:NTPTimestamp = %s", sz_time);

					// RTPTimestamp
					LogDebug("SR:RTPTimestamp = %u", p->GetRTPTimestamp());
					// Sender's packet count 
					LogDebug("SR:SenderPacketCount = %u", p->GetSenderPacketCount());
					// Sender's octet count
					LogDebug("SR:SenderOctetCount = %u", p->GetSenderOctetCount());
				}
				break;
				case RTCPPacket::RR:
				{
					RTCPRRPacket *p = (RTCPRRPacket *)rtcppack;
					int report_count = p->GetReceptionReportCount();
					for (int i = 0; i < report_count; i++)
					{
						LogDebug("RR[%d]:SSRC = %u", i, p->GetSSRC(i));
						LogDebug("RR[%d]:FractionLost = %d", i, p->GetFractionLost(i));
						LogDebug("RR[%d]:LostPacketCount = %u", i, p->GetLostPacketCount(i));
						LogDebug("RR[%d]:Jitter = %u", i, p->GetJitter(i));
						LogDebug("RR[%d]:ExtendedHighestSequenceNumber = %u", i,
							p->GetExtendedHighestSequenceNumber(i));
						LogDebug("RR[%d]:LSR = %u", i, p->GetLSR(i));
						LogDebug("RR[%d]:DLSR = %u", i, p->GetDLSR(i));
					}
				}
				break;
				case RTCPPacket::SDES:
				{
					RTCPSDESPacket *p = (RTCPSDESPacket *)rtcppack;
					char str[1024];

					if (!p->GotoFirstChunk())
						return;

					do
					{
						LogDebug("Chunk:");
						LogDebug("SSRC: %u", p->GetChunkSSRC());
						if (p->GotoFirstItem())
						{
							do
							{
								switch (p->GetItemType())
								{
								case RTCPSDESPacket::None:
									strcpy(str, "None    ");
									break;
								case RTCPSDESPacket::CNAME:
									strcpy(str, "CNAME:  ");
									break;
								case RTCPSDESPacket::NAME:
									strcpy(str, "NAME:   ");
									break;
								case RTCPSDESPacket::EMAIL:
									strcpy(str, "EMAIL:  ");
									break;
								case RTCPSDESPacket::PHONE:
									strcpy(str, "PHONE:  ");
									break;
								case RTCPSDESPacket::LOC:
									strcpy(str, "LOC:    ");
									break;
								case RTCPSDESPacket::TOOL:
									strcpy(str, "TOOL:   ");
									break;
								case RTCPSDESPacket::NOTE:
									strcpy(str, "NOTE:   ");
									break;
								case RTCPSDESPacket::PRIV:
									strcpy(str, "PRIV:   ");
									break;
								case RTCPSDESPacket::Unknown:
								default:
									strcpy(str, "Unknown ");
								}
								LogDebug("%s", str);

								if (p->GetItemType() != RTCPSDESPacket::PRIV)
								{
									char str[1024];
									memcpy(str, p->GetItemData(), p->GetItemLength());
									str[p->GetItemLength()] = 0;
									LogDebug("%s", str);
								}
							} while (p->GotoNextItem());
						}
					} while (p->GotoNextChunk());
				}
				break;
				case RTCPPacket::BYE:
				{
					RTCPBYEPacket *p = (RTCPBYEPacket *)rtcppack;
					int ssrc_count = p->GetSSRCCount();
					LogDebug("BYE: SSRCCount:%d", ssrc_count);
					for (int i = 0; i < ssrc_count; i++)
					{

					}
					
				}
				break;
				case RTCPPacket::APP:
				{
					RTCPAPPPacket *p = (RTCPAPPPacket *)rtcppack;
					LogDebug("APP ");
				}
				break;
				default:
					LogDebug("RTCPPacket::default ");
					break;
				}
			}
		}
	}
	void OnRTCPSDESItem(RTPSourceData *srcdat, RTCPSDESPacket::ItemType t, const void *itemdata, size_t itemlength)
	{
		char msg[1024];

		memset(msg, 0, sizeof(msg));
		if (itemlength >= sizeof(msg))
			itemlength = sizeof(msg) - 1;

		memcpy(msg, itemdata, itemlength);
		LogDebug("Received SDES item (%d): %s", (int)t, msg);
	}

	void OnBYEPacket(RTPSourceData *dat)
	{
		if (dat->IsOwnSSRC())
			return;

		uint32_t ip;
		uint16_t port;

		if (dat->GetRTPDataAddress() != 0)
		{
			const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTPDataAddress());
			ip = addr->GetIP();
			port = addr->GetPort();
		}
		else if (dat->GetRTCPDataAddress() != 0)
		{
			const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTCPDataAddress());
			ip = addr->GetIP();
			port = addr->GetPort() - 1;
		}
		else
			return;

		RTPIPv4Address dest(ip, port);
		DeleteDestination(dest);

		struct in_addr inaddr;
		inaddr.s_addr = htonl(ip);
		std::cout << "Deleting destination " << std::string(inet_ntoa(inaddr)) << ":" << port << std::endl;
	}
};
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
	std::shared_ptr<LQFRingBuffer<LQF::AVPacket>> aud_send_pkt_q_;	// 音频发送队列
	std::shared_ptr<LQFRingBuffer<LQF::AVPacket>> vid_send_pkt_q_;	// 视频发送队列

	std::shared_ptr<LQFRingBuffer<LQF::AVPacket>> aud_recv_pkt_q_;	// 音频接收队列
	std::shared_ptr<LQFRingBuffer<LQF::AVPacket>> vid_recv_pkt_q_;	// 视频接收队列

	bool rtp_vid_is_connect_ = false;
	bool rtp_aud_is_connect_ = false;
	int	video_format_;

	// video
	CRTPRTPSession jrtp_sess_video_;
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