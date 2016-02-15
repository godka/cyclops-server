#pragma once
#include "MythConfig.hh"
#include "mythListFactory.hh"

class mythVirtualDecoder :
	public mythListFactory
{
public:
	static mythVirtualDecoder* CreateNew(void);
	virtual void start();
	virtual void stop();
	virtual ~mythVirtualDecoder(void);
	unsigned int GetTimeCount();
	static Uint32 TimerCallbackStatic(Uint32 interval, void *param);
protected:
	mythVirtualDecoder(void);
	Uint32 TimerCallback(Uint32 interval);
	int flag;
	unsigned int m_count;
	unsigned int ori_count;
	unsigned int ret_count;
	SDL_TimerID m_timeid;
};

