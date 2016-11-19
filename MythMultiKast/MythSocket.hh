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
	static MythSocket* CreateNew(int sockfd){
		return new MythSocket(sockfd);
	}
	~MythSocket();
	int active;
	int isPush;
	void* addtionaldata;
	void* data;
private:
	int _sockfd;
	bool isrunning;
	//char* downbuffer;
	int downlength;
	int maxlength;
protected:
	MythSocket(const char* ip, int port);
	MythSocket();
	MythSocket(int sockfd);
	int wait_on_socket(int sockfd, int for_recv, long timeout_ms);
};

