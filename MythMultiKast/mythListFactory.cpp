#include "mythListFactory.hh"


mythListFactory::mythListFactory()
{
	list_type = read_profile_int("config", "list_type", 0, MYTH_INFORMATIONINI_FILE);
	switch (list_type)
	{
	case 0:
		vlist = mythAvlist::CreateNew();
		break;
	case 1:
		vlist = mythRedisList::CreateNew();
		break;
	default:
		vlist = NULL;
		break;
	}
}


mythListFactory::~mythListFactory()
{
}

mythListFactory* mythListFactory::CreateNew(int BufferSize /*= 0*/)
{
	return new mythListFactory();
}

PacketQueue * mythListFactory::get(int freePacket /*= 0*/)
{
	if (vlist)
		return vlist->get(freePacket);
	else
		return NULL;
}

int mythListFactory::put(unsigned char* data, unsigned int length)
{
	if (vlist)
		return vlist->put(data, length);
	else
		return NULL;
}

int mythListFactory::release(PacketQueue *pack)
{
	if (vlist)
		return vlist->release(pack);
	else
		return NULL;
}

int mythListFactory::free()
{
	if (vlist)
		return vlist->free();
	else
		return NULL;
}

int mythListFactory::SetMagic(void* value)
{
	vlist->magic = value;
	return 0;
}
