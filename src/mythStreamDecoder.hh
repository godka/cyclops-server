#pragma once
#include "MythConfig.hh"
#include "mythVirtualDecoder.hh"
#include "MythSocket.hh"
#include <thread>
class mythStreamDecoder :
	public mythVirtualDecoder
{
public:
	static mythStreamDecoder* CreateNew(char* ip, int port, int CameraID);
	static mythStreamDecoder* CreateNew(char* ip, int CameraID);
	//void start();
	void stop();
	~mythStreamDecoder(void);
	int MainLoop();
protected:
	//static int decodethreadstatic(void* data);
	mythStreamDecoder(char* ip, int port, int CameraID);
	MythSocket *msocket;
private:
	char* m_ip;
	int m_port;
	int m_cameraid;
	int SendBufferBlock(const char* socket_SendStr);
};

