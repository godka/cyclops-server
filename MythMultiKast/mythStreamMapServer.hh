#pragma once
#include "MythConfig.hh"
#include "mythServerMap.hh"
#include "mythStreamServer.hh"
#include "mythBaseClient.hh"
#include <map>
using namespace std;
class mythStreamMapServer :
	public mythVirtualServer//,public mythVirtualSqlite
{
public:
	static void OnClientCloseStatic(mythStreamServer * streamserver, int duration, void* data){
		mythStreamMapServer* mapserver = (mythStreamMapServer*) data;
		mapserver->OnClientClose(streamserver, duration);
	}
	void OnClientClose(mythStreamServer * streamserver, int duration);
	static mythStreamMapServer* CreateNew(int port);
	void ServerDecodeCallBack(MythSocket* people, char* data, int datalength);
	void ServerCloseCallBack(MythSocket* people);
	void showAllClients();
	int startAll(void);

	~mythStreamMapServer(void);
protected:
	mythStreamMapServer(int port);
private:
	mythServerMap* servermap;
};

