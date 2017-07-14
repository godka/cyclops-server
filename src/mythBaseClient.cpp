#include "mythBaseClient.hh"
#include <iostream>
#include <chrono>
mythBaseClient::mythBaseClient(MythSocket* people, char* protocol)
{
	_basictick = ~0;
	mpeople = people;
	isfirst = true;
#ifdef USETHREAD
	_avlist = mythAvlist::CreateNew(5);
	_thread = new std::thread(&mythBaseClient::mainthread, this);
	_isrunning = true;
#endif
}
#ifdef USETHREAD
void mythBaseClient::mainthread(){
	while (_isrunning){
		auto pkt = _avlist->get();
		if (pkt){
			int tmplength = 0;

			if (mpeople){
				tmplength = mpeople->socket_SendStr((char*) pkt->Packet, pkt->PacketLength);
				if (tmplength <  pkt->PacketLength){
					mythLog::GetInstance()->printf("socket %d send error:len=%d,size=%d\n", mpeople, tmplength, pkt->PacketLength);
					break;
					//return;
				}
			}
			_avlist->release(pkt);
		}
		else{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
	delete _avlist;
	_avlist = nullptr;
}
#endif
mythBaseClient::~mythBaseClient(void)
{
}
int mythBaseClient::GetSockID(){
	if (mpeople){
		return mpeople->GetSockID();
	}
	else{
		return -1;
	}
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
#ifdef USETHREAD
	if (_thread){
		if (_avlist){
			_avlist->put((unsigned char*) data, length);
			return length;//always ok;
		}
		else{
			return -1;
		}
	}
	else{
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
#else
	int tmplength = 0;

	if (mpeople){
		tmplength = mpeople->socket_SendStr((char*) data, length);
		if (tmplength < length){
			mythLog::GetInstance()->printf("socket %d send error:len=%d,size=%d\n", mpeople, tmplength, length);
			return -1;
		}
	}
	return tmplength;
#endif
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
