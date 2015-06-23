#pragma once
#include "MythConfig.hh"
#include <string.h>
class PEOPLE
{
public:
	int socket_SendStr(const char* str, int length = -2);
	int socket_ReceiveData(char* recvBuf, int recvLength);
	int socket_CloseSocket();
	int socket_ReceiveDataLn2(char* recvBuf, int recvLength, char* lnstr);
	static PEOPLE* CreateNew(const char* ip, int port){
		return new PEOPLE(ip, port);
	}
	static PEOPLE* CreateNew(){
		return new PEOPLE();
	}
	PEOPLE(const char* ip, int port);
	PEOPLE();
	~PEOPLE();
	int active;
	TCPsocket sock;
	IPaddress peer;
	void* addtionaldata;
	void* data;
private:
	char* downbuffer;
	int downlength;
	int maxlength;
	SDLNet_SocketSet socketset;
	int socket_strcmp(char* buff, char*str, int length);
};

