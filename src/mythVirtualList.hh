#pragma once
#include "MythConfig.hh"
class mythVirtualList
{
public:	
	static mythVirtualList* CreateNew();
	~mythVirtualList(void);
	virtual PacketQueue *get(int freePacket = 0);
	virtual int put(unsigned char* data, unsigned int length,unsigned int timestamp = ~0);
	virtual int put(unsigned char** dataline, unsigned int *datasize, unsigned int width, unsigned int height, unsigned int timestamp = ~0);
	virtual void before_put(PacketQueue *pack);
	virtual int release(PacketQueue *pack);
	bool IsIframe(PacketQueue *pack);
	virtual PacketQueue* getIframe();
	int GetNalu(PacketQueue* pack);
	void* magic;
protected:
	mythVirtualList();
	char* pps_cache,* sps_cache,* iframe_cache;
	int pps_len, sps_len, iframe_len;
};

