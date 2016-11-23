#include "mythLive555Decoder.hh"

mythLive555Decoder::mythLive555Decoder(char* rtsplink,char* username,char* password)
	:mythVirtualDecoder()
{
	if (rtsplink)
		m_rtsplink = rtsplink;
	if (username)
		m_username = username;
	if (password)
		m_password = password;
	flag = 0;
	isrunning = false;
}
mythLive555Decoder* mythLive555Decoder::CreateNew(char* rtsplink,char* username,char* password){
	return new mythLive555Decoder(rtsplink,username,password);
}

void mythLive555Decoder::callbackdata(unsigned char* data,unsigned int length)
{
	m_count += length;
	put(data,length);
}
int mythLive555Decoder::MainLoop(){
	isrunning = true;
	rtsp = mythRTSP::CreateNew();
	while (isrunning){
		if (rtsp){
			client = rtsp->openURL(this->m_rtsplink.c_str(), this->m_username.c_str(), this->m_password.c_str(), [](void *myth, unsigned char* data, unsigned int length){
				mythLive555Decoder* mythlive555 = (mythLive555Decoder*) myth;
				mythlive555->callbackdata(data, length);
			}, (void*)this);
			if (client){
				rtsp->Start(client);
			}
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
#ifdef _DEBUG
		mythLog::GetInstance()->printf("Ready to reconnect\n");
#endif
	}
	return 0;
}
mythLive555Decoder::~mythLive555Decoder(void)
{
}

void mythLive555Decoder::stop()
{
	isrunning = false;
	if (rtsp && client)
		rtsp->Stop(client);
	StopThread();
	delete rtsp;
}
