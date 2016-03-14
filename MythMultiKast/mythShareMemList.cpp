#include "mythShareMemList.hh"


mythShareMemList::mythShareMemList()
{
	if (magic){
		//create new share memory with AVBUFFERSIZE(M)
		m_sharememory = new ShareMemory((int) magic, 
			AVBUFFERSIZE * 1024 * 1024 + AVFRAMECOUNT * sizeof(PacketQueue), 
			NULL);
		if (m_sharememory){
			m_sharebuffer = (char*) m_sharememory->GetBuffer() + AVFRAMECOUNT * sizeof(PacketQueue);
			m_listbuffer = (char*) m_sharememory->GetBuffer();
			if (m_sharememory->isCreated()){
				//if the share memory is first create, initalize list is essential.
				unsigned char* m_buf = (unsigned char*) m_listbuffer;
				for (int i = 0; i < AVFRAMECOUNT; i++){
					ListPacket[i] = (PacketQueue*) m_buf;
					ListPacket[i]->h264Packet = NULL;
					ListPacket[i]->h264PacketLength = NULL;
					ListPacket[i]->magic = NULL;
					m_buf += sizeof(PacketQueue);
				}
				printf("I'm your fucking father.\n");
			}
			else{
				printf("I'm a good son.\n");
			}
			listwrite = 0;
			listread = 0;
			totalptr = 0;
		}
		mutex = SDL_CreateMutex();
	}
}


PacketQueue * mythShareMemList::get(int freePacket /*= 0*/)
{
	if (!m_sharebuffer){ return NULL; }
	//SDL_LockMutex(this->mutex);
	PacketQueue *tmp;
	if (listwrite - listread == 1 ||
		listwrite - listread == 0 ||
		(listwrite == 0 && listread == AVFRAMECOUNT)){
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
	//SDL_UnlockMutex(this->mutex);

	return tmp;
}

unsigned char* mythShareMemList::putcore(unsigned char* data, unsigned int datasize){
	if (totalptr + datasize > (unsigned int) (AVBUFFERSIZE * 1024 * 1024))
		totalptr = 0;
	SDL_memcpy(m_sharebuffer + totalptr, data, datasize);
	totalptr += datasize;
	return (unsigned char*)(totalptr - datasize);
}
int mythShareMemList::put(unsigned char* data, unsigned int length)
{
	if (!m_sharebuffer){ return 1; }
	if (!mutex){ return 1; }
	//SDL_LockMutex(mutex);
	if (listwrite >= AVFRAMECOUNT)listwrite = 0;
	PacketQueue *tmp = ListPacket[listwrite];
	tmp->h264PacketLength = length;
	tmp->h264Packet = putcore(data, length);
	listwrite++;
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
