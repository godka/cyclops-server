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
	m_thread = nullptr;

	int streamtype = mythIniFile::GetInstance()->GetInt("config", "streamtype"); 
	int level = mythIniFile::GetInstance()->GetInt("config", "streamlevel"); 
	int pipeline = mythIniFile::GetInstance()->GetInt("config", "pipeline");
	if (pipeline)
		m_pipeline = mythMediaPipeline::CreateNew(this, streamtype, level);//only for test
	else
		m_pipeline = NULL;
}

void mythVirtualDecoder::start(bool usethread){
	if (usethread == true){
		if (!m_thread)
			m_thread = new std::thread(&mythVirtualDecoder::MainLoop, this);
	}
	else{
		printf("[mythmultikast]I'm in mainloop\n");
		MainLoop();
	}
}
void mythVirtualDecoder::StopThread(){
	if (m_thread)
		m_thread->join();
	m_thread = nullptr;
	//StopCallback();
}

mythVirtualDecoder* mythVirtualDecoder::CreateNew(void){
	return new mythVirtualDecoder();
}
mythVirtualDecoder::~mythVirtualDecoder(void){
	if (m_pipeline)
		delete m_pipeline;
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

void mythVirtualDecoder::stop()
{

}

int mythVirtualDecoder::MainLoop()
{
	return 0;
}
