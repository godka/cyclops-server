/********************************************************************
Created by MythKAst
©2013 MythKAst Some rights reserved.


You can build it with vc2010,gcc.
Anybody who gets this source code is able to modify or rebuild it anyway,
but please keep this section when you want to spread a new version.
It's strongly not recommended to change the original copyright. Adding new version
information, however, is Allowed. Thanks.
For the latest version, please be sure to check my website:
Http://code.google.com/mythkast


你可以用vc2010,gcc编译这些代码
任何得到此代码的人都可以修改或者重新编译这段代码，但是请保留这段文字。
请不要修改原始版权，但是可以添加新的版本信息。
最新版本请留意：Http://code.google.com/mythkast
B
MythKAst(asdic182@sina.com), in 2013 June.
*********************************************************************/
#include "mythVirtualDecoder.hh"


mythVirtualDecoder::mythVirtualDecoder(void)
	:mythListFactory(){
	flag = 0;
	m_count = 0;
	ori_count = 0;
	ret_count = 0;
	m_timeid = SDL_AddTimer(1000, TimerCallbackStatic, this);
	m_thread = NULL;

	int streamtype = read_profile_int("config", "streamtype", 0, MYTH_INFORMATIONINI_FILE);
	int level = read_profile_int("config", "streamlevel", 0, MYTH_INFORMATIONINI_FILE);
	int pipeline = read_profile_int("config", "pipeline", 0, MYTH_INFORMATIONINI_FILE);
	if (pipeline)
		m_pipeline = mythMediaPipeline::CreateNew(this, streamtype, level);//only for test
	else
		m_pipeline = NULL;
}

void mythVirtualDecoder::start(bool usethread){
	if (usethread == true){
		m_thread = SDL_CreateThread(MainLoopstatic, "mainloop", this);
	}
	else{
		printf("[mythmultikast]I'm in mainloop\n");
		MainLoopstatic(this);
	}
}
void mythVirtualDecoder::StopThread(){
	if (m_thread)
		SDL_WaitThread(m_thread, NULL);
	m_thread = NULL;
	//StopCallback();
}

mythVirtualDecoder* mythVirtualDecoder::CreateNew(void){
	return new mythVirtualDecoder();
}
mythVirtualDecoder::~mythVirtualDecoder(void){
	if (m_pipeline)
		delete m_pipeline;
}

Uint32 mythVirtualDecoder::TimerCallbackStatic(Uint32 interval, void *param){
	mythVirtualDecoder* decoder = (mythVirtualDecoder*) param;
	return decoder->TimerCallback(interval);
}

Uint32 mythVirtualDecoder::TimerCallback(Uint32 interval){
	int tmp = m_count - ori_count;
	ret_count = tmp / interval;
	ori_count = m_count;
	//printf("%d k/s\n", ret_count);
	return interval;
}

unsigned int mythVirtualDecoder::GetTimeCount(){
	return 0;
}

int mythVirtualDecoder::put(unsigned char* data, unsigned int length)
{
	if (m_pipeline){
		m_pipeline->PutMedia(data, length);
	}
	else{
		return mythListFactory::put(data, length);
	}
	return 0;
}

PacketQueue * mythVirtualDecoder::get(int freePacket /*= 0*/)
{
	if (m_pipeline)
		return m_pipeline->get(freePacket);
	else
		return mythListFactory::get(freePacket);
}

int mythVirtualDecoder::MainLoopstatic(void* data)
{
	mythVirtualDecoder* decoder = (mythVirtualDecoder*) data;
	if (decoder){
		return decoder->MainLoop();
	}
	return 0;
}

void mythVirtualDecoder::stop()
{

}

int mythVirtualDecoder::MainLoop()
{
	return 0;
}
