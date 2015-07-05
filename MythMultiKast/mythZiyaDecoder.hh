#pragma once

#include "MythConfig.hh"
#include "mythVirtualDecoder.hh"
#include "PEOPLE.hh"
class mythZiyaDecoder :
	public mythVirtualDecoder
{
public:
	static mythZiyaDecoder* CreateNew(char* ip,int port,int CameraID);
	static mythZiyaDecoder* CreateNew(char* ip,int CameraID);
	void start();
	void stop();
	~mythZiyaDecoder(void);
	static Uint32 TimerCallbackStatic(Uint32 interval, void *param);
protected:
	Uint32 TimerCallback(Uint32 interval);
	int decodethread();
	static int decodethreadstatic(void* data);
	mythZiyaDecoder(char* ip,int port,int CameraID);
	PEOPLE *msocket;
	SDL_Thread* startthread;
private:
	char* m_ip;
	int m_port;
	int m_cameraid;
};

