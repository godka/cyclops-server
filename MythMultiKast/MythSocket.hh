#pragma once
//
//add libcurl to take place of socket_receivedataln2
#include "MythConfig.hh"
#ifdef _WIN32
	#include <WinSock2.h>
	#pragma comment(lib,"ws2_32")
#else
	#include <wchar.h>
	#include <unistd.h>
	#include <assert.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <netinet/in.h>
	#include <time.h>
	#include <unistd.h>
	#include <fcntl.h>
#endif
class MythSocket
{
public:
	MythSocket();
	MythSocket(const char* ip,u_short port);
	~MythSocket(void);
	//int curl_receivedata()
	int socket_InitalSocket();
	int socket_BindAddressPort(const char* ip,u_short port);
	int socket_SendStr(const char* str,int length);
	int socket_SendStr(const char* str);
	int socket_ReceiveData(char* recvBuf, int recvLength);
	int socket_CloseSocket();
	int socket_strcmp(char* buff,char*str,int length);
	int socket_ReceiveDataLn(char* recvBuf,int recvLength,char* lnstr);
	int socket_ReceiveDataLn2(char* recvBuf,int recvLength,char* lnstr);
	//int ReadSocket(char* recvBuf,int length);
	static MythSocket *CreatedNew(const char* ip,u_short port);
protected:
	char* downbuffer;
	int downlength;
#ifdef _WIN32
	SOCKET sockClient;
	SOCKADDR_IN addrSrv;
	SOCKADDR *socketName;
#else
	int sockClient;
    struct sockaddr_in addrSrv;
#endif
	struct timeval mtimeval;
};

