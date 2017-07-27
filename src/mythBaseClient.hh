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
typedef unsigned int UINT;
typedef unsigned char BYTE;
typedef unsigned long DWORD;
class mythBaseClient
{
public:
	bool isfirst;
	static mythBaseClient* CreateNew(MythSocket* people, char* protocol = "tcp");
	virtual int DataCallBack(PacketQueue* pkt);
	virtual void CloseClient();
	int SetProtocol(const char* protocol);
	~mythBaseClient(void);
	int GetSockID();
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
	inline uint32_t find_start_code(uint8_t *buf){
		auto nal = *(unsigned int*) &buf[0];
		if ((nal & 0x00ffffff) == 0x00010000){
			return 3;
		}
		else if (nal == 0x01000000){
			return 4;
		}
		else{
			return 0;
		}
	}
	uint8_t * get_nal(uint32_t *len, uint8_t **offset, uint8_t *start, uint32_t total);
};

