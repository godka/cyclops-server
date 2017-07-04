#pragma once
#include "MythConfig.hh"
#include "mythVirtualList.hh"
#include "mythNodeList.hh"
#include "mythAvlist.hh"
class mythListFactory
{
public:
	static mythListFactory* CreateNew(int BufferSize = 0);
	PacketQueue * getIframe();
	PacketQueue *get(int freePacket = 0);
	int put(unsigned char* data, unsigned int length, unsigned int timestamp = ~0);
	//int put(unsigned char** dataline,unsigned int *datasize,unsigned int width,unsigned int height);
	int release(PacketQueue *pack);
	~mythListFactory();
	int SetMagic(void* value);
protected:
	mythListFactory();
private:
	mythVirtualList* vlist;
	//void* magic;
	int list_type;
};

