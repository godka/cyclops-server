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
	int Init();
private:
	ShareMemory* m_sharememory;
	char* m_sharebuffer; 
	char* m_listbuffer;
	PacketQueue *ListPacket[AVFRAMECOUNT];
	int listwrite, listread;
	int* tmpwrite;
	unsigned char* putcore(unsigned char* data, unsigned int datasize);
	//unsigned char* totalbuffer;
	unsigned int totalptr;
};

