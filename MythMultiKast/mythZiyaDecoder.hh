#pragma once
#include "MythConfig.hh"
#include "mythVirtualDecoder.hh"
#include "MythSocket.hh"
class mythZiyaDecoder :
	public mythVirtualDecoder
{
public:
	static mythZiyaDecoder* CreateNew(char* ip,int port,int CameraID);
	static mythZiyaDecoder* CreateNew(char* ip,int CameraID);
	void start();
	void stop();
	~mythZiyaDecoder(void);
protected:
	int decodethread();
	static int decodethreadstatic(void* data);
	mythZiyaDecoder(char* ip,int port,int CameraID);
	MythSocket *msocket;
	SDL_Thread* startthread;
	char* m_ip;
	int m_port;
	int m_cameraid;

};

