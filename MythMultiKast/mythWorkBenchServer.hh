#pragma once
#include "mythVirtualServer.hh"
class mythWorkBenchServer :
	public mythVirtualServer
{
public:
	static mythWorkBenchServer* CreateNew(int port){
		return new mythWorkBenchServer(port);
	}
	void ServerDecodeCallBack(MythSocket* people, char* data, int datalength);
	void ServerCloseCallBack(MythSocket* people);
	~mythWorkBenchServer();
protected:
	mythWorkBenchServer(int port);
};

