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
	static int decode_thread_static(void* data){
		if (data){
			mythProxyDecoder* fn = (mythProxyDecoder*) data;
			fn->decode_thread();
		}
		return 0;
	}
	int refreshSocket(MythSocket* people);//refresh socket if connect timeout
	void start();
	void stop();
protected:
	int decode_thread();
	mythProxyDecoder(MythSocket* people);
	MythSocket* msocket;
	SDL_Thread* mthread;
	SDL_mutex* mmutex;
};

