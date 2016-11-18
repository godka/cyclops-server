#pragma once
#include "MythConfig.hh"
#include "mythListFactory.hh"
#include "mythMediaPipeline.hh"
#include <thread>
class mythVirtualDecoder :
	public mythListFactory
{
public:
	static mythVirtualDecoder* CreateNew(void);
	void start(bool usethread = true);
	virtual void stop();
	void StopThread();
	virtual int MainLoop();
	virtual ~mythVirtualDecoder(void);
	unsigned int GetTimeCount();
	virtual int put(unsigned char* data, unsigned int length);
	virtual PacketQueue *get(int freePacket = 0);
	static Uint32 TimerCallbackStatic(Uint32 interval, void *param);
protected:
	mythVirtualDecoder(void);
	Uint32 TimerCallback(Uint32 interval);
	int flag;
	unsigned int m_count;
	unsigned int ori_count;
	unsigned int ret_count;
	SDL_TimerID m_timeid;
	std::thread* m_thread;
	mythMediaPipeline* m_pipeline;
};

