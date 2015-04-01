#include "mythZiyaDecoder.hh"

mythZiyaDecoder::mythZiyaDecoder(char* ip,int port,int CameraID)
	:mythVirtualDecoder()
{
	flag = 0;
	m_ip = ip;
	m_port = port;
	m_cameraid = CameraID;
	startthread = NULL;
}
mythZiyaDecoder* mythZiyaDecoder::CreateNew(char* ip,int CameraID){
	return new mythZiyaDecoder(ip,5834,CameraID);
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
int mythZiyaDecoder::decodethread(){

#define BUFF_COUNT 1024*1024	
	char* buf = new char[BUFF_COUNT];
	msocket = MythSocket::CreatedNew(m_ip,m_port);
	if (msocket != NULL){
		char tmpsendstr[100];
		SDL_snprintf(tmpsendstr, 100, "GET /CameraID=%d&Type=zyh264 HTTP/1.0\r\n\r\n", m_cameraid);
		msocket->socket_SendStr(tmpsendstr);
		SDL_Delay(100);
		while (flag == 0){
			int rc = msocket->socket_ReceiveDataLn2(buf, BUFF_COUNT, "Content_Length: ");
			if (rc > 0) {
				put((unsigned char*) buf, rc);
			}else{
				//msocket->Reconnect(m_ip,m_port);
				SDL_Delay(1000);
				msocket->socket_SendStr(tmpsendstr);
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
	//printf("ziya decoder delete! : %d\n", m_cameraid);
	return 0;
}
mythZiyaDecoder::~mythZiyaDecoder(void)
{
	if (msocket){
		delete msocket;
	}
}
