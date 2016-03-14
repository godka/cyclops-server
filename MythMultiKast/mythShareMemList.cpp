#include "mythShareMemList.hh"


mythShareMemList::mythShareMemList()
{
}


PacketQueue * mythShareMemList::get(int freePacket /*= 0*/)
{
	return NULL;
}

int mythShareMemList::put(unsigned char* data, unsigned int length)
{
	return 0;
}

int mythShareMemList::release(PacketQueue *pack)
{
	return 0;
}

mythShareMemList::~mythShareMemList()
{
}
