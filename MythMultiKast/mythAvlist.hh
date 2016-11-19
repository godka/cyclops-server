#pragma once
#include "mythVirtualList.hh"
#include <mutex>
//#include "SDL2/SDL.h"

class mythAvlist :
	public mythVirtualList
{
public:
	static mythAvlist* CreateNew(int BufferSize = 0);
	~mythAvlist(void);
	PacketQueue *get(int freePacket = 0);
	int put(unsigned char* data,unsigned int length);
	int put(unsigned char** dataline, unsigned int *datasize, int width, int height);
	int release(PacketQueue *pack);
	int free();
private:
	int InitalList();
	int mBufferSize;
protected:
	bool startread;
	mythAvlist(void);
	mythAvlist(int BufferSize);
	//int InitalList(void);
	int abort_request;
    std::mutex _mutex;
	unsigned char* totalbuffer;
	unsigned int totalptr;
	unsigned int listcount;
	PacketQueue* ListPacket;
	int listwrite,listread;
	unsigned char* putcore(unsigned char* data,unsigned int datasize);
};

