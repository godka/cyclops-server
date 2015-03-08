#include "MythSocket.hh"
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
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



int MythSocket::socket_InitalSocket()
{
#ifdef _WIN32
	WSADATA wsaData;
	WORD wVersionRequested;

	wVersionRequested=MAKEWORD(2,2);		//macro in c
	if(WSAStartup(wVersionRequested,&wsaData)!=0){
		return 1;
	}
#endif
	sockClient=socket(AF_INET,SOCK_STREAM,0);
	mtimeval.tv_sec = 0;
	mtimeval.tv_usec = 100;
	//if(sockClient==INVALID_SOCKET){
	//	return 1;
	//}

	return 0;
}
int MythSocket::socket_BindAddressPort(const char* ip,u_short port)
{

//do not bind  ..	dynamic
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(port);
	addrSrv.sin_addr.s_addr=inet_addr(ip);
	//addrSrv.sin_addr.S_un.S_addr=inet_addr(ip);
//	socketName = (SOCKADDR *)(&addrSrv);
	if(connect(sockClient,(sockaddr *)(&addrSrv),sizeof(addrSrv))==-1){
		return 1;
	}
	return 0;
}

int MythSocket::socket_SendStr(const char* str){
	return socket_SendStr(str, -2);
}
int MythSocket::socket_SendStr(const char* str,int length)
{
	if(length == -2)length = strlen(str);
	if(sockClient != NULL){
		if(send(sockClient, str, length,0) == -1){
			return 1;
		}
	}
	//Sleep(10);
	return 0;
}

int MythSocket::socket_ReceiveData(char* recvBuf,int recvLength)
{
	int length;
	if(sockClient != NULL){
		length = recv(this->sockClient,recvBuf,recvLength,0);
		if(length>0){
			*(recvBuf + length) = '\0';
			return length;
		}
		else{
			return 0;
		}
	}
	else{
		return 0;
	}
}

int MythSocket::socket_strcmp(char* buff,char*str,int length){
	for(int i = 0;i<length;i++)
		if(buff[i] != str[i])return 1;
	return 0;
}
int MythSocket::socket_ReceiveDataLn2(char* recvBuf,int recvLength,char* lnstr){
	int tmplength = strlen(lnstr);
	int i;
	int len;
	int buffptr = 0;
	int rlength = 4096;
	char recv[4097];// = (char*)malloc(rlength);
	int contentlength;
	int length = 60;
	fd_set readset;
	while(1){
#define FAST_SOCKET
#ifdef FAST_SOCKET
		FD_ZERO(&readset);
		FD_SET(this->sockClient,&readset);
		int nselect = select((int)(sockClient)+1,&readset,NULL,NULL,&this->mtimeval);
		switch(nselect){
			case -1:
				return 0;
				break;
			case 0:
				continue;
				break;
		}
		if(FD_ISSET(this->sockClient,&readset)!=0){
#endif
		len = socket_ReceiveData(recv,rlength);
		if(len > 0){
			for(i = 0 ;i < len - tmplength;i++){
				if(socket_strcmp(&recv[i],lnstr,tmplength) == 0){
					sscanf(&recv[i],"Content_Length: %06d",&contentlength);
					if(contentlength > 0){
						int tmpptr = 0;
						int returnvalue = contentlength;
						int tmplen = len - i - length > contentlength?contentlength:len - i - length;
						if(tmplen < 0)tmplen = 0;
						memcpy(recvBuf,&recv[i + length],tmplen);
						tmpptr += tmplen;
						contentlength -= tmplen;
						while(contentlength > 0){
							len = socket_ReceiveData(recvBuf + tmpptr,contentlength);
							tmpptr += len;
							contentlength -= len;
						}
						return returnvalue;
					}
				}
			}
		}else{
			return 0;
		}
#ifdef FAST_SOCKET
		}
#endif
	}
}
int MythSocket::socket_ReceiveDataLn(char* recvBuf,int recvLength,char* lnstr)
{
/*
	int rlength;
	int len;
	int i,j;
	int buffptr;
	int tmplength = strlen(lnstr);
	rlength = 4096;
	buffptr = 0;
	char recv[4097];// = (char*)malloc(rlength);
	char* tmpcmp = (char*)malloc(tmplength + 1);
	*(tmpcmp + tmplength) = 0;
	//读取缓存
	j = 0;
	for(i = 0;i<downlength;i++){
		*(recvBuf + i) = *(downbuffer + i);
		if(i + tmplength <= downlength){
			memcpy(tmpcmp,downbuffer + i,tmplength);
			if(strcmp(tmpcmp,lnstr) == 0){
				memmove(downbuffer,downbuffer + i + tmplength,downlength - i - tmplength);
				downlength =downlength - tmplength - i;
				delete[] tmpcmp;
				return i;
			}
		}
	}
	buffptr+=downlength;
	while(1){
		len = socket_ReceiveData(recv,rlength);
		if(len > 0){
			for(i = 0;i<len;i++){
				*(recvBuf + i + j  + buffptr) = recv[i];
				if(i + tmplength <= len){
					memcpy(tmpcmp,recv + i,tmplength);
					if(strcmp(tmpcmp,lnstr) == 0){
						memcpy(downbuffer,recv + i + tmplength,len - i- tmplength);
						downlength = len - i - tmplength;
						//free(recv);
						delete[] tmpcmp;
						return i + j + buffptr;
					}
				}
			}
			j+=len;
		}else{
			return buffptr;
		}
	}
	*/
	return 0;
}
int MythSocket::socket_CloseSocket()
{
	delete[] downbuffer;
#ifdef _WIN32
	closesocket(sockClient);
	WSACleanup();
#else
	close(sockClient);
#endif
	return 0;
}
MythSocket* MythSocket::CreatedNew(const char* ip,u_short port){
	return new MythSocket(ip,port);
}
MythSocket::MythSocket(const char* ip,u_short port)
{
	socket_InitalSocket();
	socket_BindAddressPort(ip,port);
	//buffptr = 0;
	downbuffer = new char[4097];
	downlength = 0;
}


MythSocket::~MythSocket(void)
{
	//delete[] downbuffer;
}
