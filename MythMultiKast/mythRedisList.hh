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
	unsigned long readListIndex(int cameraid);
	//int free();
	~mythRedisList();
private:
	unsigned long listwrite;
	unsigned long listread;
	unsigned long maxlistread;
	redisContext* context;
	PacketQueue tmp;// = { 0 };// (PacketQueue*) SDL_malloc(sizeof(PacketQueue));
	//int magic_cameraid;
protected:
	mythRedisList();
};

