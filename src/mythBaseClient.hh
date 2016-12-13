#pragma once
#include "MythConfig.hh"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MythSocket.hh"
#include "mythAvlist.hh"
#ifdef USETHREAD
	#include <thread>
#endif
#define MYTHPROTOCOL_TCP 0
#define MYTHPROTOCOL_HTTP 1
class mythBaseClient
{
public:
	bool isfirst;
	static mythBaseClient* CreateNew(MythSocket* people, char* protocol = "tcp");
	virtual int DataCallBack(PacketQueue* pkt);
	virtual void CloseClient();
	int SetProtocol(const char* protocol);
	~mythBaseClient(void);
private:
	MythSocket* mpeople;
	//void OnFlvFrame(unsigned char* data, int len);
protected:
	long long _basictick;
	mythBaseClient(MythSocket* people, char* protocol = "tcp");
	int mythSendMessage(void* data, int length = -1);
	int _mythprotocol;
#ifdef USETHREAD
	void mainthread();
	mythAvlist* _avlist;
	std::thread* _thread;
	bool _isrunning;
#endif
};

