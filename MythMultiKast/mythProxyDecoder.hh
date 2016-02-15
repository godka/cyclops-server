#pragma once
#include "mythVirtualDecoder.hh"
#include "MythSocket.hh"
class mythProxyDecoder :
	public mythVirtualDecoder
{
public:
	static mythProxyDecoder* CreateNew(MythSocket* people){
		return new mythProxyDecoder(people);
	}
	~mythProxyDecoder();
	int refreshSocket(MythSocket* people);//refresh socket if connect timeout
	void stop();
	int MainLoop();
protected:
	mythProxyDecoder(MythSocket* people);
	MythSocket* msocket;
	SDL_mutex* mmutex;
};

