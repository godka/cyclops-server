#pragma once
#include "mythVirtualDecoder.hh"
#include "PEOPLE.hh"
class mythProxyDecoder :
	public mythVirtualDecoder
{
public:
	static mythProxyDecoder* CreateNew(PEOPLE* people){
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
	int refreshSocket(PEOPLE* people);//refresh socket if connect timeout
	void start();
	void stop();
protected:
	int decode_thread();
	mythProxyDecoder(PEOPLE* people);
	PEOPLE* msocket;
	SDL_Thread* mthread;
	SDL_mutex* mmutex;
};

