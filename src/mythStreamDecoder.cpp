#include "mythStreamDecoder.hh"

 mythStreamDecoder::mythStreamDecoder(char* ip, int port, int CameraID)
 :mythVirtualDecoder(){
	flag = 0;
	m_ip = ip;
	m_port = port;
	m_cameraid = CameraID;
	msocket = NULL;
}
mythStreamDecoder* mythStreamDecoder::CreateNew(char* ip, int CameraID){
	return new mythStreamDecoder(ip, streamserverport, CameraID);
}
mythStreamDecoder* mythStreamDecoder::CreateNew(char* ip, int port, int CameraID){
	return new mythStreamDecoder(ip, port, CameraID);
}
void mythStreamDecoder::stop(){
	//this->startthread = SDL_CreateThread(decodethreadstatic,"decode",this);
	flag = 1;
	StopThread();
	return;
}
int mythStreamDecoder::SendBufferBlock(const char* tmpsendstr){
	while (flag == 0){
		if (msocket){
			if (msocket->socket_SendStr(tmpsendstr) == 0){
				return 0;
				break;
			}
			else{
				mythLog::GetInstance()->printf("start to reconnect\n");
				std::this_thread::sleep_for(std::chrono::seconds(1));
				msocket->socket_CloseSocket();
				delete msocket;
				msocket = MythSocket::CreateNew(m_ip.c_str(), m_port);
			}
		}
	}
	return 1;
}
mythStreamDecoder* mythStreamDecoder::CreateNew(const char* url)
{
	int ip1, ip2, ip3, ip4, port, cameraid;
	char ip[20] = { 0 };
	if (sscanf(url, "stream://%d.%d.%d.%d:%d/%d", &ip1, &ip2, &ip3, &ip4, &port, &cameraid) == 6){
		sprintf(ip, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
		return new mythStreamDecoder(ip, port, cameraid);
	}
	else if (sscanf(url, "stream://%d.%d.%d.%d/%d", &ip1, &ip2, &ip3, &ip4, &cameraid) == 5){
		sprintf(ip, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
		port = 5834;
		return new mythStreamDecoder(ip, port, cameraid);
	}
	return NULL;
}
int mythStreamDecoder::MainLoop(){
#define BUFF_COUNT 1024*1024	
	char* buf = new char[BUFF_COUNT];
	msocket = MythSocket::CreateNew(m_ip.c_str(), m_port);
	if (msocket != NULL){
		char tmpsendstr[100];
		sprintf(tmpsendstr, "GET /CameraID=%d&Type=zyh264 HTTP/1.0\r\n\r\n", m_cameraid);
		SendBufferBlock(tmpsendstr);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		while (flag == 0){
			int rc = msocket->socket_ReceiveDataLn2(buf, BUFF_COUNT, "Content_Length: ");
			if (rc > 0) {
				m_count += rc;
				put((unsigned char*) buf, rc);
			}
			else if (rc == 0){
				mythLog::GetInstance()->printf("start to reconnect:ip=%s,port=%d\n",m_ip.c_str(),m_port);
				std::this_thread::sleep_for(std::chrono::seconds(1));
				msocket->socket_CloseSocket();
				delete msocket;
				msocket = MythSocket::CreateNew(m_ip.c_str(), m_port);
				SendBufferBlock(tmpsendstr);
				mythLog::GetInstance()->printf("reconnecting:ip=%s,port=%d\n", m_ip.c_str(), m_port);
			}
			else{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}
		msocket->socket_CloseSocket();
	}
	delete [] buf;
	delete msocket;
	msocket = NULL;
	return 0;
}
mythStreamDecoder::~mythStreamDecoder(void){
	if (msocket){
		delete msocket;
	}
}

