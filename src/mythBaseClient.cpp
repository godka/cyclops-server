#include "mythBaseClient.hh"
#include <iostream>

mythBaseClient::mythBaseClient(MythSocket* people)
{
	_basictick = ~0;
	mpeople = people;
	isfirst = true;
}

mythBaseClient::~mythBaseClient(void)
{
}

int mythBaseClient::DataCallBack(PacketQueue* pkt)
{
	return 0;
}

mythBaseClient* mythBaseClient::CreateNew(MythSocket* people)
{
	return new mythBaseClient(people);
}

int mythBaseClient::mythSendMessage(void* data, int length)
{
	if (length == -1)
		length = strlen((char*) data);
	int tmplength = 0;

	if (mpeople){
		tmplength = mpeople->socket_SendStr((char*) data, length);
		if (tmplength < length){
			mythLog::GetInstance()->printf("socket %d send error:%d\n", mpeople, tmplength);
			return -1;
		}
	}
	return tmplength;
}

void mythBaseClient::CloseClient()
{
	if (mpeople){
		mpeople->socket_CloseSocket();
	}
}
