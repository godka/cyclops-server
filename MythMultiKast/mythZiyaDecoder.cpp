#include "mythZiyaDecoder.hh"

mythZiyaDecoder::mythZiyaDecoder(char* ip,int port,int CameraID)
	:mythVirtualDecoder()
{
	flag = 0;
	m_ip = ip;
	m_port = port;
	m_cameraid = CameraID;
	startthread = NULL;
#ifdef USINGCURL
	curl = NULL;
#endif
	msocket = NULL;
}
#ifdef USINGCURL
size_t mythZiyaDecoder::myth_curlcallback_static(void *buffer, size_t size, size_t nmemb, void *stream){
	mythZiyaDecoder* m_decoder = (mythZiyaDecoder*) stream;
	return m_decoder->myth_curlcallback(buffer, size, nmemb);
}
#endif
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
int mythZiyaDecoder::decodethread(){
#ifdef USINGCURL
	return start_with_curl();
#else
#define BUFF_COUNT 1024*1024	
	char* buf = new char[BUFF_COUNT];
	msocket = MythSocket::CreatedNew(m_ip,m_port);
	if (msocket != NULL){
		char tmpsendstr[100];
		SDL_snprintf(tmpsendstr, 100, "GET /CameraID=%d&Type=zyh264 HTTP/1.0\r\n\r\n", m_cameraid);
		msocket->socket_SendStr(tmpsendstr);
		SDL_Delay(100);
		while (flag == 0){
			//printf("ready to receive buff\n");
			int rc = msocket->socket_ReceiveDataLn2(buf, BUFF_COUNT, "Content_Length: ");
			if (rc > 0) {
				put((unsigned char*) buf, rc);
			}else{
				printf("start to reconnect\n");
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
	//*/
#endif
}
mythZiyaDecoder::~mythZiyaDecoder(void)
{
	if (msocket){
		delete msocket;
	}
}
#ifdef USINGCURL
int mythZiyaDecoder::start_with_curl()
{
	char tmpurl[256] = { 0 };
	char tmppost[256] = { 0 };
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init(); 
	if (curl) { 
		SDL_snprintf(tmpurl, 256, "http://%s:%d/", m_ip, m_port);
		SDL_snprintf(tmppost, 256, "CameraID=%d", m_cameraid);

		struct curl_slist *headers = NULL; 
		headers = curl_slist_append(headers, "Content-Type: multipart/x-mixed-replace;boundary=--myboundary");
		//headers = curl_slist_append(headers, "Content - Type: image / h264");
		
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 120);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_URL, tmpurl);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, tmppost);
		//curl_easy_setopt(curl, CURLOPT_URL, "http://svn.mythkast.net:8088/src.tar.gz");
		//curl_easy_setopt(curl, CURLOPT_HEADER, 0);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, myth_curlcallback_static);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, this); 
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		res = curl_easy_perform(curl);

		curl_easy_cleanup(curl);
		//curl_slist_free_all(headers); /* free the header list */

		if (res != CURLE_OK)
		{
			start_with_curl();
			//printf("%d\n",123123213);
		}
	}
	return 0;
}

size_t mythZiyaDecoder::myth_curlcallback(void *buffer, size_t size, size_t nmemb)
{
	char *ct;
	size_t sizes = size * nmemb;
	res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
	if (CURLE_OK == res && ct){
		printf("We received Content-Type: %s\n", ct);
	}
	//curl_easy_getinfo()
	//size_t stDataLen = size * nmemb;
	put((unsigned char*) buffer, sizes);
	return sizes;
}
#endif