#include "mythStreamMapServer.hh"
#include "mythRequestParser.hh"
mythServerMap* servermap = nullptr;

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
	auto t = event_get_supported_methods();
	int i = 0;
	while (t[i]){
		printf("%s\n", t[i++]);
	}
	//event_config_require_features(cfg, EV_FEATURE_ET);  //使用边沿触发类型
	servermap = mythServerMap::CreateNew();
	base = event_base_new_with_config(cfg);
	event_config_free(cfg);
	//显示当前使用的异步类型
	mythLog::GetInstance()->printf("Current Using Method: %s\n", event_base_get_method(base)); // epoll

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
								people->socket_SendStr("404");
								bufferevent_free(bev);
							}
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
				//servermap->DropClient(people);
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
