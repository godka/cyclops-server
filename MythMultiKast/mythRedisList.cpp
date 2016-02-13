#include "mythRedisList.hh"
#include "redis/hiredis.h"

mythRedisList::mythRedisList()
	:mythVirtualList()
{
	listread = 0;
	listwrite = 0
}


mythRedisList::~mythRedisList()
{
}

PacketQueue * mythRedisList::get(int freePacket /*= 0*/)
{

}

int mythRedisList::put(unsigned char* data, unsigned int length)
{

}

int mythRedisList::release(PacketQueue *pack)
{

}

int mythRedisList::free()
{

}
