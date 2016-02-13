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

int mythVirtualList::put(unsigned char** dataline, unsigned int *datasize, unsigned int width, unsigned int height)
{
	return 0;
}

int mythVirtualList::put(unsigned char* data, unsigned int length)
{
	return 0;
}

PacketQueue * mythVirtualList::get(int freePacket /*= 0*/)
{
	return 0;
}

int mythVirtualList::free()
{
	return 0;
}
