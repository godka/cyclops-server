#include "mythStreamMapServer.hh"
#include "mythRequestParser.hh"
#include <fstream>
mythServerMap* servermap = nullptr;

cJSON* loadConfigFile(char const* path) {
	FILE *f;
	if ((f = fopen(path, "rb")) == NULL)
		return NULL;

	cJSON* json = NULL;
	fseek(f, 0, SEEK_END);
	long len = ftell(f);
	fseek(f, 0, SEEK_SET);
	char* data = (char*) malloc(len + 1);
	if (fread(data, 1, len, f) == (size_t) len) {
		data[len] = '\0';
		json = cJSON_Parse(data);
	}
	free(data);
	fclose(f);
	return json;
}
#ifdef USELIBEVENT
int initalsocket(int port)
{
	struct event_base *base;
	struct evconnlistener *listener;
	struct sockaddr_in sin;
#ifdef _WIN32
	WSADATA wsaData;
	WORD wVersionRequested;

	wVersionRequested = MAKEWORD(2, 2);		//macro in c
	if (WSAStartup(wVersionRequested, &wsaData) != 0){
		return 1;
	}
#endif
	auto cfg = event_config_new();
	event_config_avoid_method(cfg, "select");   //避免使用低效率select
	//event_config_require_features(cfg, EV_FEATURE_ET);  //使用边沿触发类型
	servermap = mythServerMap::CreateNew();
	cJSON* in = loadConfigFile("camera.json");
	if (in)
		servermap->AppendClient(in);
	base = event_base_new_with_config(cfg);
	event_config_free(cfg);
	//显示当前使用的异步类型
	mythLog::GetInstance()->printf("Libevent Current Using Method: %s\n", event_base_get_method(base)); // epoll

	//可选设置优先级数目，然后通过event_priority_set设置事件的优先级
	//0为最高，n_priority-1为最低，此后创建的事件默认优先级为中间优先级
	event_base_priority_init(base, 3);
	if (!base) {
		puts("Couldn't open event base");
		return 1;
	}
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(0);
	sin.sin_port = htons(port);

	listener = evconnlistener_new_bind(base, [](struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *address, int socklen, void *ctx){
		struct event_base *base = evconnlistener_get_base(listener);
		struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
		int sockfd = bufferevent_getfd(bev);
		MythSocket* people = MythSocket::CreateNew(sockfd);
		sockaddr_in sin;
		memcpy(&sin, address, sizeof(sin));
		// 取得ip和端口号
		auto ip = inet_ntoa(sin.sin_addr);
		people->ip = ip;
		//free(ip);
		mythLog::GetInstance()->printf("socket:Incomming data,ip=%s,sockfd=%d\n", ip, sockfd);
		bufferevent_setcb(bev, [](struct bufferevent *bev, void *ctx){
			MythSocket* people = (MythSocket*) ctx;
			if (people->isPush)
				return;
			size_t inputlen = 0;
			char inputstr[512] = { 0 };
			int len = bufferevent_read(bev, inputstr, 512);
			if (len > 2){
				mythRequestParser* header = mythRequestParser::CreateNew(inputstr);
				if (header->Success){
					std::string request_header = header->GetHeader();
					if (request_header == "GET"){
						//GET Method
						auto req = header->GetReq();
						if (req == ""){
							auto cameraid = header->ParseInt("CameraID");
							if (cameraid > 0){
								auto cameratype = header->Parse("Type");
								servermap->AppendClient(cameraid, people, cameratype.c_str());
							}
							else{
								auto url = header->Parse("url");
								if (url != ""){
									servermap->AppendClient(header, people);
								}
								else{
									mythLog::GetInstance()->printf("New request,GET:%s\n", req.c_str());
									SendStaticFile(people, req);
									bufferevent_free(bev);
								}
							}
						}else{
							mythLog::GetInstance()->printf("New request,GET:%s\n", req.c_str());
							SendStaticFile(people, req);
							bufferevent_free(bev);
						}
					}
					else if (request_header == "PUT"){
						//Put Method
						auto cameraid = header->ParseInt("CameraID");
						if (cameraid > 0){
							servermap->AppendClient(cameraid, people);
						}
						else{
							people->socket_SendStr("404");
							bufferevent_free(bev);
						}
					}
					else{
						people->socket_SendStr("404");
						bufferevent_free(bev);
					}
				}
				delete header;
			}
		}, NULL, [](struct bufferevent *bev, short events, void *ctx){
			mythLog::GetInstance()->printf("event on %d\n", events);
			
			MythSocket* people = (MythSocket*) ctx;
			if (events & BEV_EVENT_ERROR)
				perror("Error from bufferevent");
			if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
				bufferevent_free(bev);
				servermap->DropClient(people);
				//delete people;
			}
		}, people);
		bufferevent_enable(bev, EV_READ | EV_WRITE);
	},
		NULL, LEV_OPT_THREADSAFE | LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1, (struct sockaddr*)&sin, sizeof(sin));
	if (!listener) {
		perror("Couldn't create listener");
		return 1;
	}
	evconnlistener_set_error_cb(listener, [](struct evconnlistener *listener, void *ctx){
		struct event_base *base = evconnlistener_get_base(listener);
		int err = EVUTIL_SOCKET_ERROR();
		mythLog::GetInstance()->printf("Got an error %d (%s) on the listener. "
			"Shutting down.\n", err, evutil_socket_error_to_string(err));

		event_base_loopexit(base, NULL);
	});
	mythLog::GetInstance()->printf("Server IP: %d.%d.%d.%d ---  %d\n", 0, 0, 0, 0, port);
	mythLog::GetInstance()->printf("MythMultiKast in libevent: stable version.\n");
	event_base_dispatch(base);
#ifdef WIN32
	WSACleanup();
#endif
	return 0;
}
#else
#ifdef WIN32
int init_win_socket()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return -1;
	}
	return 0;
}
#endif

int initalsocket(int port)
{
#ifdef WIN32
	init_win_socket();
#endif
	servermap = mythServerMap::CreateNew();
	cJSON* in = loadConfigFile("camera.json");
	if (in)
		servermap->AppendClient(in);
	struct sockaddr_in cliaddr;
	int listenfd1 = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);
	if (::bind(listenfd1, (const sockaddr *) &servaddr, sizeof(servaddr)) == SOCKET_ERROR){
		printf("bind error\n");
		return 1;
	}
	if (listen(listenfd1, 5) == SOCKET_ERROR){
		printf("listen error\n");
		return 1;
	}
	mythLog::GetInstance()->printf("Start server on %d,use simple select server\n",port); 
	int maxfdp1 = listenfd1 + 1;
	for (;;){
		socklen_t cliaddrlen = sizeof(cliaddr);
		int connfd = ::accept(listenfd1, (sockaddr *) &cliaddr, &cliaddrlen);
		if (connfd == INVALID_SOCKET)
			break;
		int req = connfd;
		char* buf = NULL;
		char inputstr[4096] = { 0 };
		int len = recv(connfd, (char *) inputstr, 4096, 0);
		if (len > 0){
			MythSocket* people = MythSocket::CreateNew(connfd);
			mythLog::GetInstance()->printf("socket:Incomming data,sockfd=%d\n", people->ip, connfd);
			mythRequestParser* header = mythRequestParser::CreateNew(inputstr);
			if (header->Success){
				auto request_header = header->GetHeader();
				if (request_header == "GET"){
					//GET Method
					auto req = header->GetReq();
					if (req == ""){
						auto cameraid = header->ParseInt("CameraID");
						if (cameraid > 0){
							auto cameratype = header->Parse("Type");
							servermap->AppendClient(cameraid, people, cameratype.c_str());
						}
						else{
							auto url = header->Parse("url");
							if (url != ""){
								servermap->AppendClient(header, people);
							}
							else{
								mythLog::GetInstance()->printf("New request,GET:%s\n", req.c_str());
								SendStaticFile(people, req);
								people->socket_CloseSocket();
							}
						}
					}
					else{
						mythLog::GetInstance()->printf("New request,GET:%s\n", req.c_str());
						SendStaticFile(people, req);
						people->socket_CloseSocket();
					}
				}
				else if (request_header == "PUT"){
					//Put Method
					auto cameraid = header->ParseInt("CameraID");
					if (cameraid > 0){
						servermap->AppendClient(cameraid, people);
					}
					else{
						people->socket_SendStr("404");
						people->socket_CloseSocket();
					}
				}
				else{
					people->socket_SendStr("404");
					people->socket_CloseSocket();
				}
			}
			delete header;
		}
		else {
			servermap->DropClient(connfd);
			closesocket(connfd);
		}
	}
	closesocket(listenfd1);
#ifdef WIN32
	WSACleanup();
#endif
	return 0;
}
#endif
void SendStaticFile(MythSocket* _people, std::string _filename){
	if (_filename == ""){
		_filename = "index.html";
	}
	std::string _realfilename = "./html/" + _filename;
	std::fstream _fstreamfile;
	_fstreamfile.open(_realfilename.c_str(), std::ios::binary | std::ios::in);
	if (!_fstreamfile){
		//404
		//_people->socket_SendStr("404");
		std::string _bodystr = "<html><h1>Not Found</h1></html>";
		char buff[256] = { 0 };
		sprintf(buff, "HTTP/1.1 404 \r\nServer: mythmultikast API server\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n%s", _bodystr.length(), _bodystr.c_str());
		_people->socket_SendStr(buff);
	}
	else{
		_fstreamfile.seekg(0, std::ios::end);
		int _filelen = _fstreamfile.tellg();
		_fstreamfile.seekg(0, std::ios::beg);
		char response_str[256] = { 0 };
		char _filebuf[1024] = { 0 };
		sprintf(response_str, 
			"HTTP/1.1 200 OK\r\nServer: mythmultikast API server\r\nConnection: Close\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n", 
			get_mime_type(_filename.c_str()).c_str(), 
			(int) _filelen);
		_people->socket_SendStr(response_str);
		for (;;){
			_fstreamfile.read(_filebuf, 1024);
			auto _len = _fstreamfile.gcount();
			if (_len > 0)
				_people->socket_SendStr(_filebuf, _fstreamfile.gcount());
			else
				break;
		}
		_fstreamfile.close();
	}
}
std::string get_mime_type(const char *name)
{
	char *dot = (char*)strrchr(name, '.');

	/* Text */
	if (strcmp(dot, ".txt") == 0)
	{
		return "text/plain";
	}
	else if (strcmp(dot, ".css") == 0){
		return "text/css";
	}
	else if (strcmp(dot, ".js") == 0)
	{
		return "text/javascript";
	}
	else if (strcmp(dot, ".xml") == 0 || strcmp(dot, ".xsl") == 0){
		return "text/xml";
	}
	else if (strcmp(dot, ".xhtm") == 0 || strcmp(dot, ".xhtml") == 0 || strcmp(dot, ".xht") == 0){
		return "application/xhtml+xml";
	}
	else if (strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0 || strcmp(dot, ".shtml") == 0 || strcmp(dot, ".hts") == 0){
		return "text/html";
	}
	else if (strcmp(dot, ".gif") == 0){
		return "image/gif";
	}
	else if (strcmp(dot, ".png") == 0){
		return "image/png";
	}
	else if (strcmp(dot, ".bmp") == 0){
		return "application/x-MS-bmp";
	}
	else if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0 || strcmp(dot, ".jpe") == 0 || strcmp(dot, ".jpz") == 0){
		return "image/jpeg";
	}
	else if (strcmp(dot, ".wav") == 0){
		return "audio/wav";
	}
	else if (strcmp(dot, ".wma") == 0){
		return "audio/x-ms-wma";
	}
	else if (strcmp(dot, ".wmv") == 0){
		return "audio/x-ms-wmv";
	}
	else if (strcmp(dot, ".au") == 0 || strcmp(dot, ".snd") == 0){
		return "audio/basic";
	}
	else if (strcmp(dot, ".midi") == 0 || strcmp(dot, ".mid") == 0){
		return "audio/midi";
	}
	else if (strcmp(dot, ".mp3") == 0 || strcmp(dot, ".mp2") == 0){
		return "audio/x-mpeg";
	}
	else if (strcmp(dot, ".rm") == 0 || strcmp(dot, ".rmvb") == 0 || strcmp(dot, ".rmm") == 0){
		return "audio/x-pn-realaudio";
	}
	else if (strcmp(dot, ".avi") == 0){
		return "video/x-msvideo";
	}
	else if (strcmp(dot, ".3gp") == 0){
		return "video/3gpp";
	}
	else if (strcmp(dot, ".mov") == 0){
		return "video/quicktime";
	}
	else if (strcmp(dot, ".wmx") == 0){
		return "video/x-ms-wmx";
	}
	else if (strcmp(dot, ".asf") == 0 || strcmp(dot, ".asx") == 0){
		return "video/x-ms-asf";
	}
	else if (strcmp(dot, ".mp4") == 0 || strcmp(dot, ".mpg4") == 0){
		return "video/mp4";
	}
	else if (strcmp(dot, ".mpe") == 0 || strcmp(dot, ".mpeg") == 0 || strcmp(dot, ".mpg") == 0 || strcmp(dot, ".mpga") == 0){
		return "video/mpeg";
	}
	else if (strcmp(dot, ".pdf") == 0){
		return "application/pdf";
	}
	else if (strcmp(dot, ".rtf") == 0){
		return "application/rtf";
	}
	else if (strcmp(dot, ".doc") == 0 || strcmp(dot, ".dot") == 0){
		return "application/msword";
	}
	else if (strcmp(dot, ".xls") == 0 || strcmp(dot, ".xla") == 0){
		return "application/msexcel";
	}
	else if (strcmp(dot, ".hlp") == 0 || strcmp(dot, ".chm") == 0){
		return "application/mshelp";
	}
	else if (strcmp(dot, ".swf") == 0 || strcmp(dot, ".swfl") == 0 || strcmp(dot, ".cab") == 0){
		return "application/x-shockwave-flash";
	}
	else if (strcmp(dot, ".ppt") == 0 || strcmp(dot, ".ppz") == 0 || strcmp(dot, ".pps") == 0 || strcmp(dot, ".pot") == 0){
		return "application/mspowerpoint";
	}
	else if (strcmp(dot, ".zip") == 0){
		return "application/zip";
	}
	else if (strcmp(dot, ".rar") == 0){
		return "application/x-rar-compressed";
	}
	else if (strcmp(dot, ".gz") == 0){
		return "application/x-gzip";
	}
	else if (strcmp(dot, ".jar") == 0){
		return "application/java-archive";
	}
	else if (strcmp(dot, ".tgz") == 0 || strcmp(dot, ".tar") == 0){
		return "application/x-tar";
	}
	else {
		return "application/octet-stream";
	}
}