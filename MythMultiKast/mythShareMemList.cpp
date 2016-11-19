#include "mythShareMemList.hh"
#include <memory.h>

mythShareMemList::mythShareMemList()
{
	m_sharememory = NULL;
	m_sharebuffer = NULL;
	m_listbuffer = NULL;
}

int mythShareMemList::Init(){
	if (m_sharememory)return 0;
	if (magic){
		printf("%d\n", (int) magic);
		//create new share memory with AVBUFFERSIZE(M)
		/*
		listwrite|listbuffer                 |sharebuffer
		*/
		m_sharememory = new ShareMemory((int) magic,
			AVBUFFERSIZE * 1024 * 1024 + AVFRAMECOUNT * sizeof(PacketQueue) + sizeof(int),
			NULL);
		if (m_sharememory){
			tmpwrite = (int*) m_sharememory->GetBuffer();
			m_listbuffer = (char*) m_sharememory->GetBuffer() + sizeof(int);
			m_sharebuffer = (char*) m_sharememory->GetBuffer() + +sizeof(int) + AVFRAMECOUNT * sizeof(PacketQueue);
			//if the share memory is first create, initalize list is essential.
			unsigned char* m_buf = (unsigned char*) m_listbuffer;
			for (int i = 0; i < AVFRAMECOUNT; i++){
				ListPacket[i] = (PacketQueue*) m_buf;
				if (m_sharememory->isCreated()){
					ListPacket[i]->h264Packet = NULL;
					ListPacket[i]->h264PacketLength = 0;
					ListPacket[i]->magic = NULL;
				}
				m_buf += sizeof(PacketQueue);
			}
			listwrite = 0;
			listread = 0;
			totalptr = 0;
		}
	}
	return 0;
}
PacketQueue * mythShareMemList::get(int freePacket /*= 0*/)
{
	Init();
	if (!m_sharememory){ return NULL; }
	//SDL_LockMutex(this->mutex);
	PacketQueue *tmp;
	if (*tmpwrite - listread == 1 ||
		*tmpwrite - listread == 0 ||
		(*tmpwrite == 0 && listread == AVFRAMECOUNT)){
		tmp = NULL;
	}
	else{
		//ListPacket[listread]->h264Packet += (unsigned char*)m_sharebuffer;
		tmp = ListPacket[listread];
		if (tmp->h264Packet == NULL){
			tmp = NULL;
		}
		else{
			unsigned char* p = tmp->h264Packet;
			unsigned long t = (unsigned long) p + (unsigned long) m_sharebuffer;
			tmp->h264Packet = (unsigned char*) t;
			if (freePacket == 0){
				if (*tmpwrite - listread > 10){
					LOGE("skip frames");
					LOGE(" read = %d,write = %d,minus = %d\n", listread, *tmpwrite, *tmpwrite - listread);
					listread += 9;
				}
				else
					listread++;
			}
		}
	}
	listread %= AVFRAMECOUNT;
	//SDL_UnlockMutex(this->mutex);

	return tmp;
}

unsigned char* mythShareMemList::putcore(unsigned char* data, unsigned int datasize){
	if (totalptr + datasize > (unsigned int) (AVBUFFERSIZE * 1024 * 1024))
		totalptr = 0;
	memcpy(m_sharebuffer + totalptr, data, datasize);
	totalptr += datasize;
	return (unsigned char*)(totalptr - datasize);
}
int mythShareMemList::put(unsigned char* data, unsigned int length)
{
	Init();
	if (!m_sharememory){ return 1; }
	//SDL_LockMutex(mutex);
	if (listwrite >= AVFRAMECOUNT)listwrite = 0;
	PacketQueue *tmp = ListPacket[listwrite];
	tmp->h264PacketLength = length;
	tmp->h264Packet = putcore(data, length);
	listwrite++;
	*tmpwrite = listwrite;
	//SDL_UnlockMutex(mutex);
	return 0;
}

int mythShareMemList::release(PacketQueue *pack)
{
	return 0;
}

mythShareMemList::~mythShareMemList()
{
}
