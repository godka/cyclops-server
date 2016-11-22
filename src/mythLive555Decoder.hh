#pragma once
#include "MythConfig.hh"
#include "mythVirtualDecoder.hh"
#include "mythRTSP.hh"
#include <string>
using namespace std;

class mythLive555Decoder :
	public mythVirtualDecoder
{
public:
	static mythLive555Decoder* CreateNew(char* rtsplink,char* username,char* password);
	~mythLive555Decoder(void);
	void callbackdata(unsigned char* data,unsigned int length);
	int MainLoop();
	void stop();
protected:
	mythLive555Decoder(char* rtsplink,char* username,char* password);
	string m_rtsplink,m_username,m_password;
	mythRTSP* rtsp;
	RTSPClient* client;
	bool isrunning;
	char flag;
};

