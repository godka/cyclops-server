#include "mythNodeList.hh"
#include <stdlib.h>
#include <memory.h>

mythNodeList::mythNodeList(void)
{
	header.next = &header;
	header.prev = &header;
}

mythNodeList *mythNodeList::CreateNew(int BufferSize){
	return new mythNodeList();
}

mythNodeList::~mythNodeList(void)
{
	freeMemory();
}

PacketQueue *mythNodeList::get(int freepacket){
	PacketQueue *tmp;
	//_mutex.lock();
	if (header.next == header.prev){
		tmp = nullptr;
	}
	else{
		tmp = list_entry(header.next, struct PacketQueue, list);
		list_del(&tmp->list);
	}
	//_mutex.unlock();
	return tmp;
}
int mythNodeList::put(unsigned char* data, unsigned int length){
	PacketQueue *tmp = (PacketQueue*) malloc(sizeof(PacketQueue));
	tmp->h264Packet = (unsigned char*) malloc(length);
	for (int i = 0; i < 3; i++){
		tmp->yuvPacket[i] = nullptr;
	}
	if (tmp->h264Packet == nullptr)
	{
		mythLog::GetInstance()->printf("malloc failed in mythNodeList::put\n");
		return 1;
	}
	memcpy(tmp->h264Packet, data, length);
	tmp->h264PacketLength = length;
	tmp->PacketCount++;
	//_mutex.lock();
	list_add_tail(&tmp->list, &header);
	//_mutex.unlock();
	return 0;
}
unsigned char* mythNodeList::putcore(unsigned char* data, unsigned int datasize){
	unsigned char* tmpbuf = (unsigned char*) malloc(datasize);
	if (tmpbuf == nullptr)
	{
		mythLog::GetInstance()->printf("malloc failed in mythNodeList::putcore\n");
		return nullptr;
	}
	memcpy(tmpbuf, data, datasize);
	return tmpbuf;
}
int mythNodeList::put(unsigned char** dataline, unsigned int *datasize, int width, int height)
{
	PacketQueue *tmp = (PacketQueue*) malloc(sizeof(PacketQueue));
	tmp->h264Packet = nullptr;
	unsigned char* YY = (unsigned char*)putcore(dataline[0], datasize[0] * height);
	unsigned int Ydatasize = datasize[0];
	unsigned char* UU = (unsigned char*)putcore(dataline[1], datasize[1] * height / 2);
	unsigned int Udatasize = datasize[1];
	unsigned char* VV = (unsigned char*)putcore(dataline[2], datasize[2] * height / 2);
	unsigned int Vdatasize = datasize[2];
	tmp->yuvPacket[0] = YY; tmp->yuvPacket[1] = UU; tmp->yuvPacket[2] = VV;
	tmp->yuvPacketLength[0] = Ydatasize; tmp->yuvPacketLength[1] = Udatasize; tmp->yuvPacketLength[2] = Vdatasize;
	return 0;
}

int mythNodeList::release(PacketQueue *pack)
{
	if (pack->h264Packet)
		free(pack->h264Packet);
	if (pack)
		free(pack);
	return 0;
}

int mythNodeList::freeMemory(){
	//_mutex.lock();
	for (;;){
		if (header.next == header.prev){
			break;
		}
		PacketQueue *tmp = list_entry(header.next, struct PacketQueue, list);
		list_del(&tmp->list);
	}
	//_mutex.unlock();
	return 0;
}
