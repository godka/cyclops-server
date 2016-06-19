#pragma once
#include "MythConfig.hh"
#include "mythVirtualServer.hh"
#include "mythVirtualDecoder.hh"
#include <map>
using namespace std;
class mythOpenresyServer :
	public mythVirtualServer
{
public:
	static mythOpenresyServer* CreateNew(int port);
	~mythOpenresyServer();
	void ServerDecodeCallBack(MythSocket* people, char* data, int datalength);
	void ServerCloseCallBack(MythSocket* people);
protected:
	mythOpenresyServer(int port);
private:
	map<int, mythVirtualDecoder*> servermap;
	SDL_mutex* mapmutex;
	mythVirtualDecoder* connectServer(int cameraid);
};

