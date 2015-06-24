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
	//shutdownStream(this->rtspClient);
	//delete rtspClient;
	//rtspClient = NULL;
}
mythLive555Decoder::~mythLive555Decoder(void)
{
	//shutdownStream(rtspClient);
}
