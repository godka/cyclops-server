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
#ifdef USEPIPELINE
	if (pipeline)
		m_pipeline = mythMediaPipeline::CreateNew(this, streamtype, level,"test");//only for test
	else
		m_pipeline = nullptr;
#endif
}

void mythVirtualDecoder::start(bool usethread){
	if (usethread == true){
		if (!m_thread)
			m_thread = new std::thread(&mythVirtualDecoder::MainLoop, this);
	}
	else{
		mythLog::GetInstance()->printf("[mythmultikast]I'm in mainloop\n");
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
#ifdef USEPIPELINE
	if (m_pipeline)
		delete m_pipeline;
#endif
}

unsigned int mythVirtualDecoder::GetTimeCount(){
	return 0;
}

int mythVirtualDecoder::put(unsigned char* data, unsigned int length,unsigned int timestamp)
{
#ifdef USEPIPELINE
	if (m_pipeline){
		m_pipeline->PutMedia(data, length, timestamp);
		mythListFactory::put(data, length, timestamp);
		return 0;
	}
	else
		return mythListFactory::put(data, length, timestamp);
#else
	return mythListFactory::put(data, length, timestamp);
#endif
}

PacketQueue * mythVirtualDecoder::get(int freePacket /*= 0*/)
{
#ifdef USEPIPELINE
	if (m_pipeline)
		return m_pipeline->get(freePacket);
	else
		return mythListFactory::get(freePacket);
#else
		return mythListFactory::get(freePacket);
#endif
}

void mythVirtualDecoder::stop()
{

}

int mythVirtualDecoder::MainLoop()
{
	return 0;
}
