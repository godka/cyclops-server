#include "mythLive555Decoder.hh"
//#include "curl/curl.h"

mythLive555Decoder::mythLive555Decoder(char* rtsplink,char* username,char* password)
	:mythVirtualDecoder()
{
	m_rtsplink = rtsplink;
	m_username = username;
	m_password = password;
	flag = 0;
}
mythLive555Decoder* mythLive555Decoder::CreateNew(char* rtsplink,char* username,char* password){
	return new mythLive555Decoder(rtsplink,username,password);
}
void mythLive555Decoder::start(){
	startthread = SDL_CreateThread(decodethreadstatic,"decode",this);
}
void mythLive555Decoder::callbackdatastatic(void *myth,unsigned char* data,unsigned int length){
	mythLive555Decoder* mythlive555 = (mythLive555Decoder*)myth;
	mythlive555->callbackdata(data,length);
}
void mythLive555Decoder::callbackdata(unsigned char* data,unsigned int length)
{
	put(data,length);
}
int mythLive555Decoder::decodethread()
{

	UsageEnvironment *env = initalAllcommons();
	//RTSPClient * rtspClient =  openURL(*env, "rtsp://192.168.245.71/h264/ch1/main/av_stream","admin","12345");
	rtspClient = openURL(*env,
		this->m_rtsplink,(const char*)this->m_username,(const char*)this->m_password,
		mythLive555Decoder::callbackdatastatic,(void*)this);
	beginDescrible(rtspClient);
	startLoop(env,&flag);
	return 0;
}
int mythLive555Decoder::decodethreadstatic(void* data)
{
	mythLive555Decoder* live555decoder = (mythLive555Decoder*)data;
	return live555decoder->decodethread();
}
void mythLive555Decoder::stop()
{
	flag = 1;
	SDL_WaitThread(startthread, NULL); 
	shutdownStream(this->rtspClient);
	rtspClient->close(NULL);
	//delete rtspClient;
	//rtspClient = NULL;
}
mythLive555Decoder::~mythLive555Decoder(void)
{
	//shutdownStream(rtspClient);
}
