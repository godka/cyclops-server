#include "mythStreamDecoder.hh"
int mythStreamDecoder::init_win_socket()
{
#ifdef WIN32
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return -1;
	}
#endif
	return 0;
}
mythStreamDecoder::mythStreamDecoder(char* ip, int port, int CameraID)
	:mythVirtualDecoder(){
#ifdef WIN32
	init_win_socket();
#endif
	flag = 0;
	m_ip = ip;
	m_port = port;
	m_cameraid = CameraID;
	buflen = 0;
	bufindex = 0;
	isSkipOA = false;
}

void mythStreamDecoder::bufferevent_read_callback(struct bufferevent *bufevt)
{
	static int contentlength = 0;
	size_t len = 0;
	if (contentlength == 0){
		char* request_line = evbuffer_readln(bufferevent_get_input(bufevt), &len, EVBUFFER_EOL_CRLF);
		if (len > 0){
			sscanf(request_line, "Content_Length: %06d", &contentlength);
			if (contentlength > 0){
				isSkipOA = false;
				bufindex = 0;
				buflen = contentlength;
				//printf("Content Length = %d\n", contentlength);
				return;
			}
		}
		free(request_line);
	}
	else{
		if (!isSkipOA){
			char tmp;
			len = bufferevent_read(bufevt, &tmp, 1);
			isSkipOA = true;
		}
		else{
			len = bufferevent_read(bufevt, buf + bufindex, buflen);
			bufindex += len;
			buflen -= len;
			if (buflen == 0){
				put((unsigned char*) buf, bufindex);
				//OnGettingFrame(buf, bufindex);
				contentlength = 0;
			}
		}
	}
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
	event_base_loopexit(evbase, NULL);
	StopThread();
	return;
}
// 根据ip和端口生成sockaddr结构并返回其地址，同时输出addrlen(实现略)
struct sockaddr * mythStreamDecoder::make_sock_addr(const char *ip, int port, int *addrlen){
	sockaddr_in * addrSrv = (sockaddr_in *) malloc(sizeof(sockaddr_in));
	addrSrv->sin_family = AF_INET;
	addrSrv->sin_port = htons(port);
	addrSrv->sin_addr.s_addr = inet_addr(ip);
	*addrlen = sizeof(sockaddr);
	return (sockaddr*) addrSrv;
}
int mythStreamDecoder::MainLoop(){
#define BUFF_COUNT 1024*1024	
	buf = new char[BUFF_COUNT];
	int addrlen = 0;
	struct sockaddr * paddr = make_sock_addr(m_ip, m_port, &addrlen);

	int fd = socket(AF_INET, SOCK_STREAM, 0);

	// 创建event base并绑定bufferevent
	evbase = event_base_new();
	struct bufferevent * bufevt = bufferevent_socket_new(evbase, fd, 0);

	bufferevent_setcb(bufevt, [](struct bufferevent *bufevt, void *arg){
		mythStreamDecoder* decoder = (mythStreamDecoder*) arg;
		decoder->bufferevent_read_callback(bufevt);
	}, NULL, NULL, this);
	bufferevent_enable(bufevt, EV_READ);

	// 连接服务器, 并启动事件循环
	bufferevent_socket_connect(bufevt, paddr, addrlen);
	char connectstr[256] = { 0 };
	sprintf(connectstr, "GET /CameraID=%d&Type=zyh264 HTTP/1.0\r\n\r\n", m_cameraid);
	bufferevent_write(bufevt, connectstr, strlen(connectstr));
	event_base_dispatch(evbase);

	if (bufevt)  bufferevent_free(bufevt);
	if (evbase)  event_base_free(evbase);
	if (fd >= 0) {
#ifdef WIN32
		closesocket(fd);
#else
		close(fd);
#endif
	}
	delete [] buf;
	return 0;
}
mythStreamDecoder::~mythStreamDecoder(void){
#ifdef WIN32
	WSACleanup();
#endif
}

