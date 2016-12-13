#include "mythBaseClient.hh"
#include <iostream>

mythBaseClient::mythBaseClient(MythSocket* people, char* protocol)
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

mythBaseClient* mythBaseClient::CreateNew(MythSocket* people,char* protocol)
{
	return new mythBaseClient(people,protocol);
}

int mythBaseClient::mythSendMessage(void* data, int length)
{
	if (length == -1)
		length = strlen((char*) data);
	int tmplength = 0;

	if (mpeople){
		tmplength = mpeople->socket_SendStr((char*) data, length);
		if (tmplength < length){
			mythLog::GetInstance()->printf("socket %d send error:len=%d,size=%d\n", mpeople, tmplength, length);
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

int mythBaseClient::SetProtocol(const char* protocol)
{
	if (strcmp(protocol, "tcp") == 0){
		_mythprotocol = MYTHPROTOCOL_TCP;
	}
	else{
		_mythprotocol = MYTHPROTOCOL_HTTP;
	}
	return 0;
}
