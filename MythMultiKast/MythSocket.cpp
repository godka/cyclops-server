#include "MythSocket.hh"
/********************************************************************
tcpsocket(dll import)
A dll -- based on socket
Created by MythKAst
©2013 MythKAst Some rights reserved.


You can build it with vc2010,gcc.
Anybody who gets this source code is able to modify or rebuild it anyway,
but please keep this section when you want to spread a new version.
It's strongly not recommended to change the original copyright. Adding new version
information, however, is Allowed. Thanks.
For the latest version, please be sure to check my website:
Http://code.google.com/mythkast


你可以用vc2010,gcc编译这些代码
任何得到此代码的人都可以修改或者重新编译这段代码，但是请保留这段文字。
请不要修改原始版权，但是可以添加新的版本信息。
最新版本请留意：Http://code.google.com/mythkast

MythKAst(asdic182@sina.com), in 2013 June.
*********************************************************************/


#define USINGCURL
int MythSocket::wait_on_socket(curl_socket_t sockfd, int for_recv, long timeout_ms)
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
int MythSocket::socket_InitalSocket()
{
	curl = curl_easy_init();
	return 0;
}
int MythSocket::socket_BindAddressPort(const char* ip, u_short port)
{
	//wait_on_socket()
	if (curl){ 
		curl_easy_setopt(curl, CURLOPT_URL, ip);
		curl_easy_setopt(curl, CURLOPT_PORT, port);
		/* Do not do the transfer - only connect to host */
		curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 1L);
		res = curl_easy_perform(curl);
		if (CURLE_OK != res){
			printf("Error: %s\n", strerror(res));
			return 1;
		}
		/* Extract the socket from the curl handle - we'll need it for waiting.
		* Note that this API takes a pointer to a 'long' while we use
		* curl_socket_t for sockets otherwise.
		*/
		res = curl_easy_getinfo(curl, CURLINFO_LASTSOCKET, &sockextr);
		if (CURLE_OK != res){
			printf("Error: %s\n", curl_easy_strerror(res));
			return 1;
		}
		sockfd = sockextr;
	}
	return 0;
}
int MythSocket::socket_SendStr(const char* str, int length)
{
	if (curl){
		size_t iolen;
		if (length == -2){
			length = strlen(str);
		}
		res = curl_easy_send(curl, str, length, &iolen);

		if (CURLE_OK != res){
			printf("Error: %s\n", curl_easy_strerror(res));
			return 1;
		}
		return 0;
	}
	else{
		return 1;
	}
}

int MythSocket::socket_ReceiveData(char* recvBuf, int recvLength)
{
	size_t iolen;
	if (curl){
		wait_on_socket(sockfd, 1, 1000L);
		res = curl_easy_recv(curl, recvBuf, recvLength, &iolen);
		//if (res != 0){
		//	printf("error!\n");
		//}
		switch (res){
			case CURLE_OK:
			//if (iolen > 0)
				break;
			case CURLE_RECV_ERROR:
				printf("Client Miss Connect\n");
				printf("socket state error #0 (%d)\n", res);
				break;
			case CURLE_AGAIN:				
				break;
			case CURLE_UNSUPPORTED_PROTOCOL:
				//重连
				break;
			case CURLE_OPERATION_TIMEDOUT:
				//超时
				printf("connect timeout\n");
				break;
			default:
				break;
		}
		return iolen;
	}
	else{
		return 0;
	}
}

int MythSocket::socket_strcmp(char* buff, char*str, int length){
	for (int i = 0; i<length; i++)
		if (buff[i] != str[i])return 1;
	return 0;
}
int MythSocket::socket_ReceiveDataLn2(char* recvBuf, int recvLength, char* lnstr){
	int tmplength = strlen(lnstr);
	int i;
	int len;
	int buffptr = 0;
	int rlength = 4096;
	char recv[4097];// = (char*)malloc(rlength);
	int contentlength;
	int length = 60;
	while (1){
		SDL_PollEvent(NULL);
		SDL_Delay(1);
		len = socket_ReceiveData(recv, rlength);
		if (len > 0){
			for (i = 0; i < len - tmplength; i++){
				if (socket_strcmp(&recv[i], lnstr, tmplength) == 0){
					sscanf(&recv[i], "Content_Length: %06d", &contentlength);
					if (contentlength > 0){
						int tmpptr = 0;
						int returnvalue = contentlength;
						int tmplen = len - i - length > contentlength ? contentlength : len - i - length;
						if (tmplen < 0)tmplen = 0;
						SDL_memcpy(recvBuf, &recv[i + length], tmplen);
						tmpptr += tmplen;
						contentlength -= tmplen;
						while (contentlength > 0){
							len = socket_ReceiveData(recvBuf + tmpptr, contentlength);
							tmpptr += len;
							contentlength -= len;
						}
						return returnvalue;
					}
				}
			}
		}
		else{
			return 0;
		}
	}
}

int MythSocket::socket_CloseSocket()
{
	delete [] downbuffer;
	curl_easy_cleanup(curl);
	return 0;
}
MythSocket* MythSocket::CreatedNew(const char* ip, u_short port){
	return new MythSocket(ip, port);
}
MythSocket::MythSocket(const char* ip, u_short port)
{
	socket_InitalSocket();
	socket_BindAddressPort(ip, port);
	//buffptr = 0;
	downbuffer = new char[4097];
	downlength = 0;
}


MythSocket::~MythSocket(void)
{
//	delete [] downbuffer;
}
