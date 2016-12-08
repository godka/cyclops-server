#pragma once
#include "MythConfig.hh"
extern "C"{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
};
#include "MythSocket.hh"
#include <thread>
#include <chrono>
#include <memory.h>

#include <stdint.h>
#include <fcntl.h>
#include <math.h>

typedef unsigned int UINT;
typedef unsigned char BYTE;
typedef unsigned long DWORD;

#define FLV_TAG_HEAD_LEN 11
#define FLV_PRE_TAG_LEN 4

class mythBaseClient
{
public:
	bool isfirst;
	static mythBaseClient* CreateNew(MythSocket* people);
	virtual int DataCallBack(PacketQueue* pkt);
	virtual void CloseClient();
	~mythBaseClient(void);
private:
	MythSocket* mpeople;
	//void OnFlvFrame(unsigned char* data, int len);
protected:
	long long _basictick;
	mythBaseClient(MythSocket* people);
	int mythSendMessage(void* data, int length = -1);
};

