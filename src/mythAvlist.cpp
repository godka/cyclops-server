#include "mythAvlist.hh"
#include <stdio.h>
#include <memory.h>
mythAvlist::mythAvlist(void)
	:mythVirtualList()
{
	listcount = 0;
	this->startread = false;
	this->totalptr = 0;
	mBufferSize = AVBUFFERSIZE;
	InitalList();
}
int mythAvlist::InitalList(){
	//inital list
	totalbuffer = new unsigned char[mBufferSize * 1024 * 1024];
	ListPacket = new PacketQueue[AVFRAMECOUNT];
	for (int i = 0; i < AVFRAMECOUNT; i++){
		ListPacket[i].Packet = nullptr;
		ListPacket[i].PacketLength = 0;

		for (int j = 0; j < 3; j++){
			ListPacket[i].yuvPacket[j] = nullptr;
			ListPacket[i].yuvPacketLength[j] = 0;
		}
	}
	listwrite = 0;
	listread = 0;
	return 0;
}
mythAvlist::mythAvlist(int BufferSize)
{
	listcount = 0;
	this->startread = false;
	mBufferSize = BufferSize;
	//totalbuffer = (unsigned char*)SDL_malloc(BufferSize * 1024 * 1024);
	totalptr = 0;
	InitalList();
}
mythAvlist *mythAvlist::CreateNew(int BufferSize){
	if(BufferSize == 0)
		return new mythAvlist();
	else
		return new mythAvlist(BufferSize);
}
mythAvlist::~mythAvlist(void)
{
	free();
}
PacketQueue *mythAvlist::get(int freePacket){
	PacketQueue *tmp;
	if (this->listwrite - this->listread == 1 ||
		this->listwrite - this->listread == 0 ||
		(this->listwrite == 0 && this->listread == AVFRAMECOUNT)){
		tmp = NULL;
	}
	else{
		tmp = &this->ListPacket[this->listread];
		if (tmp->Packet == NULL && tmp->yuvPacket[0] == NULL){
			tmp = NULL;
		}
		else{
			if (freePacket == 0){
				if (listwrite - listread > 10){
					LOGE("skip frames");
					LOGE(" read = %d,write = %d,minus = %d\n", listread, listwrite, listwrite - listread);
					listread += 9;
				}
				else
					listread++;
			}
		}
	}
	listread %= AVFRAMECOUNT;
	return tmp;
}
unsigned char* mythAvlist::putcore(unsigned char* data,unsigned int datasize){
	if(totalptr + datasize > (unsigned int)(mBufferSize * 1024 * 1024))
		totalptr = 0;
	memcpy(totalbuffer + totalptr, data, datasize);
	totalptr += datasize;
	//printf("totalptr = %d\n",totalptr);
	return (totalbuffer + totalptr - datasize);
}

int mythAvlist::put(unsigned char** dataline, unsigned int *datasize, int width, int height, unsigned int timestamp,char* oridata,int orilen){
	if(listwrite >= AVFRAMECOUNT)listwrite = 0;
	PacketQueue *tmp = &ListPacket[listwrite];
	tmp->Packet = NULL;
	unsigned char* YY = (unsigned char*)putcore(dataline[0],datasize[0] * height);
	unsigned int Ydatasize = datasize[0];
	unsigned char* UU = (unsigned char*)this->putcore(dataline[1], datasize[1] * height / 2);
	unsigned int Udatasize = datasize[1];
	unsigned char* VV = (unsigned char*)this->putcore(dataline[2], datasize[2] * height / 2);
	unsigned int Vdatasize = datasize[2];
	tmp->yuvPacket[0] = YY; tmp->yuvPacket[1] = UU; tmp->yuvPacket[2] = VV;
	tmp->yuvPacketLength[0] = Ydatasize; tmp->yuvPacketLength[1] = Udatasize; tmp->yuvPacketLength[2] = Vdatasize;
	tmp->TimeStamp = timestamp;
	tmp->width = width;
	tmp->height = height;
	if (oridata)
		tmp->Packet = (unsigned char*)oridata;
	if (orilen > 0)
		tmp->PacketLength = orilen;
	listwrite++;
	return 0;
}

int mythAvlist::release(PacketQueue *pack)
{
	return 0;
}

int mythAvlist::put(unsigned char* data, unsigned int length, unsigned int timestamp){
	if(listwrite >= AVFRAMECOUNT)listwrite = 0;
	PacketQueue *tmp = &ListPacket[listwrite];
	tmp->PacketLength = length;
	tmp->Packet = putcore(data, length);
	tmp->isIframe = IsIframe(tmp);
	tmp->TimeStamp = timestamp;
	listwrite++;
	return 0;
}
int mythAvlist::free(){
	if (ListPacket)
		delete [] ListPacket;
	ListPacket = NULL;
	if (totalbuffer)
		delete [] totalbuffer;
	totalbuffer = NULL;
	return 0;
}
