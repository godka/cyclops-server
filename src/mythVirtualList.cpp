#include "mythVirtualList.hh"


mythVirtualList::mythVirtualList()
{
	//do nothing
}


mythVirtualList::~mythVirtualList()
{
}

mythVirtualList* mythVirtualList::CreateNew()
{
	return new mythVirtualList();
}

int mythVirtualList::release(PacketQueue *pack)
{
	return 0;
}

bool mythVirtualList::IsIframe(PacketQueue *pack)
{
	if (pack){
		if (pack->PacketLength > 4){
			unsigned char* tmp = pack->Packet;
			int Nalu = 0;
			if (tmp[0] == 0 && tmp[1] == 0 && tmp[2] == 0 && tmp[3] == 1){
				Nalu = tmp[4];
			}
			else if (tmp[0] == 0 && tmp[1] == 0 && tmp[2] == 1){
				Nalu = tmp[3];
			}
			int tmpnal = Nalu & 0x1f;
			if (tmpnal == 5 || tmpnal == 7 || tmpnal == 8){
				return true;
			}
		}
	}
	return false;
}

long long mythVirtualList::mythTickCount(){
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

int mythVirtualList::put(unsigned char** dataline, unsigned int *datasize, unsigned int width, unsigned int height, unsigned int timestamp)
{
	return 0;
}

int mythVirtualList::put(unsigned char* data, unsigned int length, unsigned int timestamp)
{
	return 0;
}

PacketQueue * mythVirtualList::get(int freePacket)
{
	return 0;
}
