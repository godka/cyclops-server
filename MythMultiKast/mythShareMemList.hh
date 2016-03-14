#pragma once
#include "mythVirtualList.hh"
#include "ShareMemory.hh"
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
private:
	ShareMemory* m_sharememory;
	char* m_sharebuffer; 
	char* m_listbuffer;
	PacketQueue *ListPacket[AVFRAMECOUNT];
	SDL_mutex *mutex;
	int listwrite, listread;
	unsigned char* putcore(unsigned char* data, unsigned int datasize);
	//unsigned char* totalbuffer;
	unsigned int totalptr;
};

