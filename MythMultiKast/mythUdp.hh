#pragma once
#include "MythConfig.hh"
class mythUdp
{
public:
	static mythUdp* mmythudp;
	static mythUdp* CreateNew(int localport, int remoteport){
		return new mythUdp(localport, remoteport);
	}
	static mythUdp* GetInstance(){
		return mmythudp;
	}
	mythUdp(int localport,int remoteport);
	~mythUdp();
	int SendData(const char* buf, int len = -2);
	int RecvData(const char* buf, int len);
private:
	int m_localport;
	int m_remoteport;
	void init();
	UDPsocket m_client;
	UDPpacket* packet;
};

