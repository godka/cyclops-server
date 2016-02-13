#pragma once
#include "MythConfig.hh"
#include "mythVirtualList.hh"
class mythRedisList
	:mythVirtualList
{
public:
	static mythRedisList* CreateNew(){
		return new mythRedisList();
	}
	PacketQueue *get(int freePacket = 0);
	int put(unsigned char* data, unsigned int length);
	//int put(unsigned char** dataline,unsigned int *datasize,unsigned int width,unsigned int height);
	int release(PacketQueue *pack);
	int free();
	~mythRedisList();
protected:
	mythRedisList();
private:
	unsigned int listwrite;
	unsigned int listread;
};

