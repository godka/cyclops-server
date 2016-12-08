#include "MythSocket.hh"
#include <memory.h>

MythSocket::MythSocket(int sockfd){
	_sockfd = sockfd;
	downlength = 0;
	isPush = 0;
	_isconnected = true;
}
MythSocket::MythSocket(const char* ip, int port)
{
#ifdef _WIN32
	WSADATA wsaData;
	WORD wVersionRequested;

	wVersionRequested = MAKEWORD(2, 2);		//macro in c
	if (WSAStartup(wVersionRequested, &wsaData) != 0){
		mythLog::GetInstance()->printf("Error on Initalize Socket\n");
	}
#endif
	_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	//do not bind  ..	dynamic
	//SOCKADDR_IN addrSrv;
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(port);
	addrSrv.sin_addr.s_addr = inet_addr(ip);
	//if (!wait_on_socket(_sockfd, 1, 1000L)){
	//	printf("Select Failed,%d\n", _sockfd);
	//}
	unsigned long ul = 1;
#ifdef WIN32
	ioctlsocket(_sockfd, FIONBIO, (unsigned long *) &ul);//设置成非阻塞模式。
#else
	ioctl(_sockfd, FIONBIO, &ul);
#endif
	if (connect(_sockfd, (sockaddr *) (&addrSrv), sizeof(addrSrv)) == -1){
		_isconnected = true;
		int ret = wait_on_socket(_sockfd, 0, 1000L);
		if (ret == 0){
			//mythLog::GetInstance()->printf("Connect Failed!,%d\n", _sockfd);
			_isconnected = false;
		}
	}
	ul = 0;
#ifdef WIN32
	ioctlsocket(_sockfd, FIONBIO, (unsigned long *) &ul);
#else
	ioctl(_sockfd, FIONBIO, &ul);
#endif
	downlength = 0;
	isPush = 0;
}
int MythSocket::wait_on_socket(int sockfd, int for_recv, long timeout_ms)
{
	struct timeval tv;
	fd_set infd, outfd, errfd;
	int res;

	tv.tv_sec = timeout_ms / 1000;
	tv.tv_usec = (timeout_ms % 1000) * 1000;

	FD_ZERO(&infd);
	FD_ZERO(&outfd);
	FD_ZERO(&errfd);

	FD_SET(sockfd, &errfd); /* always check for error */

	if (for_recv)
	{
		FD_SET(sockfd, &infd);
	}
	else
	{
		FD_SET(sockfd, &outfd);
	}

	/* select() returns the number of signalled sockets or -1 */
	res = select(sockfd + 1, &infd, &outfd, &errfd, &tv);
	return res;
}

#define SOCKET_LEN 512
int MythSocket::socket_SendStr(const char* data, int length){
	if (!_isconnected){
		mythLog::GetInstance()->printf("Socketid:%d connect Failed,Error: not connected.\n", _sockfd);
		return -1;
	}
	if (length == -2){
		length = strlen(data);
	}
	if (!wait_on_socket(_sockfd, 0, 1000L)) {
		mythLog::GetInstance()->printf("Socketid:%d connect Failed,Error: timeout.\n", _sockfd);
		return -1;
	}
	const char* sdata = data;
	int left = length;
	int pleft = SOCKET_LEN;
	//using in SDL_net I don't know how to do
	int sent = 0;
	int len = length;
	do{
		if (left < SOCKET_LEN){
			pleft = left;
		}
		len = send(_sockfd, sdata, pleft, 0);
		if (len > 0){
			sent += len;
			left -= len;
			sdata += len;
		}
	} while ((left) > 0 && (len > 0));
	
	return sent;
}

MythSocket::~MythSocket()
{
}

int MythSocket::socket_ReceiveData(char* recvBuf, int recvLength, int timeout)
{
	if (!_isconnected)
		return -1;
	if (!wait_on_socket(_sockfd, 1, 1000L)){
		return -1;
	}
	return recv(_sockfd, recvBuf, recvLength, 0);
}

int MythSocket::socket_ReceiveDataLn2(char* recvBuf, int recvLength, char* lnstr)
{
	int tmplength = strlen(lnstr);
	int i;
	int len;
	int buffptr = 0;
	int rlength = 4096;
	char recv[4097];
	int contentlength;
	int length = 60;
	while (1){
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		len = socket_ReceiveData(recv, rlength, 100);
		if (len > 0){
			for (i = 0; i < len - tmplength; i++){
				if (memcmp(&recv[i], lnstr, tmplength) == 0){
					sscanf(&recv[i], "Content_Length: %06d", &contentlength);
					if (contentlength > 0){
						int tmpptr = 0;
						int returnvalue = contentlength;
						int tmplen = len - i - length > contentlength ? contentlength : len - i - length;
						if (tmplen < 0)tmplen = 0;
						memcpy(recvBuf, &recv[i + length], tmplen);
						tmpptr += tmplen;
						contentlength -= tmplen;
						while (contentlength > 0){
							len = socket_ReceiveData(recvBuf + tmpptr, contentlength,100);
							if (len > 0){
								tmpptr += len;
								contentlength -= len;
							}
							else if (len == 0){
								return 0;
							}
							else{
								return -1;
							}
						}
						return returnvalue;
					}
				}
			}
		}
		else if(len == 0){
			return 0;
		}
		else{
			return -1;
		}
	}
}

int MythSocket::socket_CloseSocket()
{
	this->isPush = 0;
#ifdef _WIN32
	closesocket(_sockfd);
	WSACleanup();
#else
	close(_sockfd);
#endif
	return 0;
}
