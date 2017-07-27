#pragma once
#include "MythConfig.hh"
#include "mythListFactory.hh"
#ifdef USEPIPELINE
#include "mythMediaPipeline.hh"
#endif
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
	virtual int put(unsigned char* data, unsigned int length, unsigned int timestamp = ~0);
	virtual PacketQueue *get(int freePacket = 0);
protected:
	mythVirtualDecoder(void);
	int flag;
	unsigned int m_count;
	unsigned int ori_count;
	unsigned int ret_count;
	std::thread* m_thread;
#ifdef USEPIPELINE
	mythMediaPipeline* m_pipeline;
#endif
};

