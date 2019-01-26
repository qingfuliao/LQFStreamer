#pragma once

#include "librtmp/rtmp.h"
class RTMPPackager
{
public:
	virtual void Pack(RTMPPacket *packet, char* buf, const char* data, int length) const = 0;
	virtual void Metadata(RTMPPacket *packet, char* buf, const char* data, int length) const = 0;
};

