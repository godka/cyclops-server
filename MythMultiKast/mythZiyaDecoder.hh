#pragma once
#include "MythConfig.hh"
#include "mythVirtualDecoder.hh"
#include "MythSocket.hh"
#include "curl/curl.h"
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

	static size_t myth_curlcallback_static(void *buffer, size_t size, size_t nmemb, void *stream);
	size_t myth_curlcallback(void *buffer, size_t size, size_t nmemb);

	int start_with_curl();

	mythZiyaDecoder(char* ip,int port,int CameraID);
	MythSocket *msocket;
	SDL_Thread* startthread;
	char* m_ip;
	int m_port;
	int m_cameraid;
private:
	CURL *curl;
	CURLcode res;
};

