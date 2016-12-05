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
	servermap = mythServerMap::CreateNew();
	base = event_base_new();
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
						auto cameratype = header->Parse("Type");
						if (cameraid > 0){
							servermap->AppendClient(cameraid, people, cameratype.c_str());
						}else{
							people->socket_SendStr("404");
							bufferevent_free(bev);
						}
					}else if (request_header == "PUT"){
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
			MythSocket* people = (MythSocket*) ctx;
			if (events & BEV_EVENT_ERROR)
				perror("Error from bufferevent");
			if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
				//servermap->DropClient(people);
				bufferevent_free(bev);
			}
		},people);
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
