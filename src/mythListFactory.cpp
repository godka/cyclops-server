#include "mythListFactory.hh"


mythListFactory::mythListFactory()
{
	list_type = mythIniFile::GetInstance()->GetInt("config", "list_type");
	switch (list_type)
	{
	case 1:
		vlist = mythNodeList::CreateNew();
		break;
	case 0:
		vlist = mythAvlist::CreateNew();
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
PacketQueue * mythListFactory::getIframe(){
	if (vlist)
		return vlist->getIframe();
	else
		return NULL;
}
PacketQueue * mythListFactory::get(int freePacket /*= 0*/)
{
	if (vlist)
		return vlist->get(freePacket);
	else
		return NULL;
}

int mythListFactory::put(unsigned char* data, unsigned int length, unsigned int timestamp)
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

int mythListFactory::SetMagic(void* value)
{
	vlist->magic = value;
	return 0;
}
