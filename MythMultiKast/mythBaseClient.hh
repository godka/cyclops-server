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
class mythBaseClient:
	public mythAvlist
{
public:
	static int SendThreadStatic(void* data){
		mythBaseClient* cli = (mythBaseClient*) data;
		if (cli)
			return cli->SendThread();
		else
			return 0;
	}
	int SendThread();
	static mythBaseClient* CreateNew(MythSocket* people, int usethread = 0,const char* CameraType = NULL);
	int mythSendMessage(void* data, int length = -1);
	int DataCallBack(void* data, int len);
	~mythBaseClient(void);
private:
	int musethread;		//use thread or not
	bool isfirst;
	MythSocket* mpeople;
	int isrunning;
	SDL_Thread* mainthreadhandle;
	SDL_mutex* mymutex;
	int generate(char* data,int length);
	int iFrameCount;
	int m_cameratype;
protected:
	mythBaseClient(MythSocket* people, int usethread, const char* CameraType);
	SDL_Thread* mthread;
	bool misrunning;
};

