#pragma once
#include "MythConfig.hh"
#include "mythServerMap.hh"

#ifdef USELIBEVENT
#include "event2/listener.h"
#include "event2/bufferevent.h"
#include "event2/bufferevent_struct.h"
#include "event2/buffer.h"
#include "event2/event.h"
#include "event2/http.h"
#include "event2/http_struct.h"
#include "event2/keyvalq_struct.h"
#ifdef WIN32
#include <Winsock2.h>
#pragma comment(lib,"ws2_32")
#endif
#else
#ifdef WIN32
#include <Winsock2.h>
#pragma comment(lib,"ws2_32")
#else
#define INVALID_SOCKET  (~0)
#define SOCKET_ERROR    (-1)
#include <wchar.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#define closesocket close
#endif
#endif
#include <string>
int initalsocket(int port);
void Send404Request(MythSocket* _people);
bool SendStaticFile(MythSocket* _people, std::string _filename);
std::string get_mime_type(const char *name);
