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
	m_count += length;
	put(data,length);
}
int mythLive555Decoder::decodethread(){
	rtsp = mythRTSP::CreateNew();
	if (rtsp){
		client = rtsp->openURL(this->m_rtsplink.c_str(), this->m_username.c_str(), this->m_password.c_str(), mythLive555Decoder::callbackdatastatic, (void*)this);
		if (client){
			rtsp->Start(client);
		}
	}
	return 0;
}
int mythLive555Decoder::decodethreadstatic(void* data){
	mythLive555Decoder* live555decoder = (mythLive555Decoder*)data;
	return live555decoder->decodethread();
}
void mythLive555Decoder::stop(){
	if (rtsp && client)
		rtsp->Stop(client);
	if (startthread)
		SDL_WaitThread(startthread, NULL); 
	startthread = NULL;
	delete rtsp;
}
mythLive555Decoder::~mythLive555Decoder(void)
{
}
