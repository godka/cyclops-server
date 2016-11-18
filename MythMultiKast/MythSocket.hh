#pragma once
#include "MythConfig.hh"
#include "event2/listener.h"
#include "event2/bufferevent.h"
#include "event2/buffer.h"
#include "event2/event.h"
#include "event2/http.h"
#include "event2/http_struct.h"
#include "event2/keyvalq_struct.h"
#ifdef WIN32
#include <Winsock2.h>
#pragma comment(lib,"ws2_32")
#endif
#include <string.h>
#include <thread>
class MythSocket
{
public:
	int socket_SendStr(const char* str, int length = -2);
	int socket_ReceiveData(char* recvBuf, int recvLength, int timeout = 500);
	int socket_CloseSocket();
	int socket_ReceiveDataLn2(char* recvBuf, int recvLength, char* lnstr);
	static MythSocket* CreateNew(const char* ip, int port){
		return new MythSocket(ip, port);
	}
	static MythSocket* CreateNew(){
		return new MythSocket();
	}
	static MythSocket* CreateNew(bufferevent* bev){
		return new MythSocket(bev);
	}
	void generateSock(TCPsocket msock);
	~MythSocket();
	int active;
	int isPush;
	TCPsocket sock;
	IPaddress peer;
	void* addtionaldata;
	void* data;
private:
	bool isrunning;
	//char* downbuffer;
	int downlength;
	int maxlength;
	SDLNet_SocketSet socketset;
	int socket_strcmp(char* buff, char*str, int length);
	bufferevent* _bev;
protected:
	MythSocket(const char* ip, int port);
	MythSocket();
	MythSocket(bufferevent* bev);
};

