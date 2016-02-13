#pragma once
#include "MythConfig.hh"
typedef struct PacketQueue {
	unsigned char* h264Packet;
	unsigned int h264PacketLength;
} PacketQueue;
class mythVirtualList
{
public:	
	static mythVirtualList* CreateNew();
	~mythVirtualList(void);
	virtual PacketQueue *get(int freePacket = 0);
	virtual int put(unsigned char* data, unsigned int length);
	virtual int put(unsigned char** dataline, unsigned int *datasize, unsigned int width, unsigned int height);
	virtual int release(PacketQueue *pack);
	virtual int free();
protected:
	mythVirtualList();
};

