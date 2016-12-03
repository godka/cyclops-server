#pragma once
#include "mythVirtualList.hh"
#include <mutex>
class mythNodeList :
	public mythVirtualList
{
public:
	static mythNodeList* CreateNew(int BufferSize = 0);
	~mythNodeList();
	PacketQueue *get(int freePacket = 0);
	int put(unsigned char* data, unsigned int length);
	int put(unsigned char** dataline, unsigned int *datasize, int width, int height);
	unsigned char* putcore(unsigned char* data, unsigned int datasize);
	int release(PacketQueue *pack);
	int freeMemory();
protected:
	mythNodeList();
	struct list_head header;
	std::mutex _mutex;
};

