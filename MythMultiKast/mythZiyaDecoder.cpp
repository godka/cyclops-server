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
#include "mythZiyaDecoder.hh"

mythZiyaDecoder::mythZiyaDecoder(char* ip,int port,int CameraID)
	:mythVirtualDecoder(){
	flag = 0;
	m_ip = ip;
	m_port = port;
	m_cameraid = CameraID;
	startthread = NULL;
	msocket = NULL;
}
mythZiyaDecoder* mythZiyaDecoder::CreateNew(char* ip,int CameraID){
	return new mythZiyaDecoder(ip, streamserverport, CameraID);
}
mythZiyaDecoder* mythZiyaDecoder::CreateNew(char* ip,int port,int CameraID){
	return new mythZiyaDecoder(ip,port,CameraID);
}
void mythZiyaDecoder::start(){
	startthread = SDL_CreateThread(decodethreadstatic,"decode",this);
}
void mythZiyaDecoder::stop(){
	//this->startthread = SDL_CreateThread(decodethreadstatic,"decode",this);
	flag = 1;
	if (startthread)
		SDL_WaitThread(startthread,NULL);
	return;
}
int mythZiyaDecoder::decodethreadstatic(void* data){
	mythZiyaDecoder* m_decoder = (mythZiyaDecoder*)data;
	return m_decoder->decodethread();
}
int mythZiyaDecoder::SendBufferBlock(const char* tmpsendstr){
	while (flag == 0){
		if (msocket){
			if (msocket->socket_SendStr(tmpsendstr) == 0){
				return 0;
				break;
			}
			else{
				printf("start to reconnect\n");
				SDL_Delay(1000);
				msocket->socket_CloseSocket();
				delete msocket;
				msocket = PEOPLE::CreateNew(m_ip, m_port);
			}
		}
	}
	return 1;
}
int mythZiyaDecoder::decodethread(){
#define BUFF_COUNT 1024*1024	
	char* buf = new char[BUFF_COUNT];
	msocket = PEOPLE::CreateNew(m_ip,m_port);
	if (msocket != NULL){
		char tmpsendstr[100];
		SDL_snprintf(tmpsendstr, 100, "GET /CameraID=%d&Type=zyh264 HTTP/1.0\r\n\r\n", m_cameraid);
		SendBufferBlock(tmpsendstr);
		while (flag == 0){
			//printf("ready to receive buff\n");
			int rc = msocket->socket_ReceiveDataLn2(buf, BUFF_COUNT, "Content_Length: ");
			if (rc > 0) {
				m_count += rc;
				put((unsigned char*) buf, rc);
			}else{
				printf("start to reconnect\n");
				//msocket->Reconnect(m_ip,m_port);
				SDL_Delay(1000);
				msocket->socket_CloseSocket();
				delete msocket;
				msocket = PEOPLE::CreateNew(m_ip, m_port);
				SendBufferBlock(tmpsendstr);
				//msocket->socket_SendStr(tmpsendstr);
				printf("reconnecting\n");
			}
			SDL_PollEvent(NULL);
			SDL_Delay(1);
		}
		msocket->socket_CloseSocket();
	}
	delete [] buf;
	this->free();
	delete msocket;
	msocket = NULL;
	return 0;
}
mythZiyaDecoder::~mythZiyaDecoder(void){
	if (msocket){
		delete msocket;
	}
}

