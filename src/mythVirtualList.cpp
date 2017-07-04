#include "mythVirtualList.hh"
#include <memory.h>

mythVirtualList::mythVirtualList()
{
	//do nothing
	pps_cache = nullptr;
	sps_cache = nullptr;
	iframe_cache = nullptr;
	pps_len = 0;
	sps_len = 0;
	iframe_len = 0;
}


mythVirtualList::~mythVirtualList()
{
	if (pps_cache)
		delete [] pps_cache;
	if (sps_cache)
		delete [] sps_cache;
	if (iframe_cache)
		delete [] iframe_cache;
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

PacketQueue* mythVirtualList::getIframe()
{
	if (iframe_len > 0 && sps_len > 0){
		auto total_len = iframe_len + sps_len + pps_len;
		unsigned char* total_buf = new unsigned char[total_len];
		int index = 0;
		memcpy(total_buf, iframe_cache, iframe_len);
		index += iframe_len;
		memcpy(total_buf + index, sps_cache, sps_len);
		index += sps_len;
		if (pps_len > 0){
			memcpy(total_buf + index, pps_cache, pps_len);
		}
		PacketQueue* pkt = new PacketQueue();
		pkt->Packet = total_buf;
		pkt->PacketLength = total_len;
		return pkt;
	}
	else{
		return nullptr;
	}
}

int mythVirtualList::GetNalu(PacketQueue* pack)
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
			return tmpnal;
		}
	}
	return 0;
}

int mythVirtualList::put(unsigned char** dataline, unsigned int *datasize, unsigned int width, unsigned int height, unsigned int timestamp)
{
	return 0;
}

void mythVirtualList::before_put(PacketQueue *pack)
{
	if (pack){
		auto _nalu = GetNalu(pack);
		switch (_nalu)
		{
		case 5:
			//iframe
			if (iframe_len > 0){
				delete [] iframe_cache;
			}
			iframe_cache = new char[pack->PacketLength];
			memcpy(iframe_cache, pack->Packet, pack->PacketLength);
			iframe_len = pack->PacketLength; 
			break;
		case 7:
			if (sps_len == 0){
				sps_cache = new char[pack->PacketLength];
				memcpy(sps_cache, pack->Packet, pack->PacketLength);
				sps_len = pack->PacketLength;
			}
			break;
		case 8:
			if (pps_len == 0){
				pps_cache = new char[pack->PacketLength];
				memcpy(pps_cache, pack->Packet, pack->PacketLength);
				pps_len = pack->PacketLength;
			}
			break;
		default:
			break;
		}
	}
}

int mythVirtualList::put(unsigned char* data, unsigned int length, unsigned int timestamp)
{
	return 0;
}

PacketQueue * mythVirtualList::get(int freePacket)
{
	return 0;
}
