#pragma once
#include "MythConfig.hh"
#include "mythVirtualList.hh"
#include "redis/hiredis.h"
class mythRedisList
	:public mythVirtualList
{
public:
	static mythRedisList* CreateNew(){
		return new mythRedisList();
	}
	PacketQueue *get(int freePacket = 0);
	int put(unsigned char* data, unsigned int length);
	//int put(unsigned char** dataline,unsigned int *datasize,unsigned int width,unsigned int height);
	int release(PacketQueue *pack);
	//int free();
	~mythRedisList();
private:
	unsigned int listwrite;
	unsigned int listread;
	redisContext* context;
	//int magic_cameraid;
protected:
	mythRedisList();
};

