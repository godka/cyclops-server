#pragma once
#include "MythConfig.hh"
#include "mythVirtualDecoder.hh"
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
#endif
#include <stdlib.h>
#include <stdio.h>
class mythStreamDecoder :
	public mythVirtualDecoder
{
public:
	void bufferevent_read_callback(struct bufferevent *bufevt);
	static mythStreamDecoder* CreateNew(char* ip, int port, int CameraID);
	static mythStreamDecoder* CreateNew(char* ip, int CameraID);
	//void start();
	void stop();
	~mythStreamDecoder(void);
	static Uint32 TimerCallbackStatic(Uint32 interval, void *param);
	int MainLoop();
protected:
	Uint32 TimerCallback(Uint32 interval);
	int init_win_socket();
	//static int decodethreadstatic(void* data);
	mythStreamDecoder(char* ip, int port, int CameraID);
private:
	char* buf;
	char* m_ip;
	int m_port;
	int m_cameraid;
	struct event_base * evbase;

	unsigned int bufindex = 0;
	unsigned int buflen = 0;
	bool isSkipOA = false;

	struct sockaddr * make_sock_addr(const char *ip, int port, int *addrlen);
};

