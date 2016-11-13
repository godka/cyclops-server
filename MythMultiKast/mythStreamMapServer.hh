#pragma once
#include "MythConfig.hh"
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
	map<int,mythStreamServer*> servermap;
	map<int, int> servercount;
	static mythStreamMapServer* CreateNew(int port);
	void ServerDecodeCallBack(MythSocket* people, char* data, int datalength);
	void ServerCloseCallBack(MythSocket* people);
	void showAllClients();
	int startAll(void);

#ifdef MYTH_STREAM_CLOSE
	static Uint32 TimerCallbackStatic(Uint32 interval, void *param);
	Uint32 TimerCallback(Uint32 interval);
#endif
	~mythStreamMapServer(void);
protected:

#ifdef MYTH_STREAM_CLOSE
	SDL_TimerID timerid;
#endif
	mythStreamMapServer(int port);
private:
	SDL_mutex* mapmutex;
};

