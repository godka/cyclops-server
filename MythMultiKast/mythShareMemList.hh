#pragma once
#include "mythVirtualList.hh"
class mythShareMemList :
	public mythVirtualList
{
public:
	static mythShareMemList* CreateNew(){
		return new mythShareMemList();
	}
	PacketQueue *get(int freePacket = 0);
	int put(unsigned char* data, unsigned int length);
	//int put(unsigned char** dataline,unsigned int *datasize,unsigned int width,unsigned int height);
	int release(PacketQueue *pack);
	~mythShareMemList();
protected:
	mythShareMemList();
};

