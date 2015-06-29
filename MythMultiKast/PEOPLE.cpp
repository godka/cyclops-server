/********************************************************************
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
B
MythKAst(asdic182@sina.com), in 2013 June.
*********************************************************************/
#include "PEOPLE.hh"


PEOPLE::PEOPLE()
{
	maxlength = 512;
}

PEOPLE::PEOPLE(const char* ip, int port)
{
	IPaddress serverIP = {0};
	SDL_Init(NULL);
	SDLNet_Init();
	socketset = SDLNet_AllocSocketSet(2);
	SDLNet_ResolveHost(&serverIP, ip, (Uint16)port);
	if (!(sock = SDLNet_TCP_Open(&serverIP))){
		printf("fuck,not open!\n");
	}
	SDLNet_TCP_AddSocket(socketset, sock);
	downbuffer = new char[4097];
	downlength = 0;
}

int PEOPLE::socket_SendStr(const char* data, int length){
#ifdef MYTH_CONFIG_SENDMESSAGE_SLOW
	int tlen = length;
	if (sock){
		while (tlen > 0){
			int len = SDLNet_TCP_Send(this->sock, data, maxlength);
			tlen -= len;
		}
	}
	return 0;
#else
	if (length == -2){
		length = strlen(data);
	}
	if (sock){
		if (SDLNet_TCP_Send(this->sock, data, length) < length){
			return 1;
		}
		else{
			return 0;
		}
	}
	else
		return 1;
#endif
//	return 0;
}

PEOPLE::~PEOPLE()
{
}

int PEOPLE::socket_ReceiveData(char* recvBuf, int recvLength)
{

	SDLNet_CheckSockets(socketset, 100);
	if (SDLNet_SocketReady(sock)){
		return SDLNet_TCP_Recv(sock, recvBuf, recvLength);
	}
	else{
		return 0;
	}

}

int PEOPLE::socket_ReceiveDataLn2(char* recvBuf, int recvLength, char* lnstr)
{
	int tmplength = strlen(lnstr);
	int i;
	int len;
	int buffptr = 0;
	int rlength = 4096;
	char recv[4097];// = (char*)malloc(rlength);
	int contentlength;
	int length = 60;
	while (1){
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
	}
}

int PEOPLE::socket_strcmp(char* buff, char*str, int length)
{
	for (int i = 0; i < length; i++)
		if (buff[i] != str[i])
			return 1;
	return 0;
}

int PEOPLE::socket_CloseSocket()
{
	delete [] downbuffer;
	SDLNet_TCP_Close(sock);
	SDLNet_TCP_DelSocket(socketset, sock);
	return 0;
}
