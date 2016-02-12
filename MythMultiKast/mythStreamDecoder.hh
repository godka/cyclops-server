#pragma once

#include "MythConfig.hh"
#include "mythVirtualDecoder.hh"
#include "PEOPLE.hh"
class mythStreamDecoder :
	public mythVirtualDecoder
{
public:
	static mythStreamDecoder* CreateNew(char* ip, int port, int CameraID);
	static mythStreamDecoder* CreateNew(char* ip, int CameraID);
	void start();
	void stop();
	~mythStreamDecoder(void);
	static Uint32 TimerCallbackStatic(Uint32 interval, void *param);
protected:
	Uint32 TimerCallback(Uint32 interval);
	int decodethread();
	static int decodethreadstatic(void* data);
	mythStreamDecoder(char* ip, int port, int CameraID);
	PEOPLE *msocket;
	SDL_Thread* startthread;
private:
	char* m_ip;
	int m_port;
	int m_cameraid;
	int SendBufferBlock(const char* socket_SendStr);
};

