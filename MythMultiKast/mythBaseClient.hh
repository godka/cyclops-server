#pragma once
#include "MythConfig.hh"
extern "C"{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
};
#include "MythSocket.hh"
#include "mythAvlist.hh"
#include <thread>
#include <chrono>
#include <memory.h>
#include <string.h>
#include <stdio.h>
class mythBaseClient
{
public:
	bool isfirst;
	static mythBaseClient* CreateNew(MythSocket* people,const char* CameraType = NULL);
	int DataCallBack(void* data, int len);
	~mythBaseClient(void);
private:
	MythSocket* mpeople;
	int mythSendMessage(void* data, int length = -1);
	int AddBuffer(void* data, int length = -1);
	int FlushBuffer();
	int iFrameCount;
	int m_cameratype;
	int _mode;
	char* sendBuffer;
	int sendbufferptr;
protected:
	mythBaseClient(MythSocket* people, const char* CameraType);
};

