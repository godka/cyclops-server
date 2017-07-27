#pragma once
#include "mythVirtualDecoder.hh"
#include "mythRTSP.hh"
#include <string>
using namespace std;

class mythLive555Decoder :
	public mythVirtualDecoder
{
public:
	void OnReconnect(int times);
	static int decodethreadstatic(void* data);
	int decodethread();
	static int ParseRTSPlink(char* src, char* rtsplink, char* username, char* password);
	static mythLive555Decoder* CreateNew(char* rtsplink, bool usetcp = true);
	static mythLive555Decoder* CreateNew(char* rtsplink, char* username, char* password, bool usetcp = true);
	~mythLive555Decoder(void);
	static void callbackdatastatic(void *myth, unsigned char* data, unsigned int length, unsigned int timestamp);
	void callbackdata(unsigned char* data, unsigned int length, unsigned int timestamp);
	int MainLoop();
	void stop();
protected:
	mythLive555Decoder(char* rtsplink, char* username, char* password, bool usetcp);
	string m_rtsplink, m_username, m_password;
	mythRTSP* rtsp;
	RTSPClient* client;
	bool isrunning;
	char flag;
	bool fusetcp;
	int _times;
};

