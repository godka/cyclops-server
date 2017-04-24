#pragma once
#include <map>
#include <mutex>
#include "MythSocket.hh"
#include "mythClientFactory.hh"
#include "mythBaseClient.hh"
#include "mythStreamServer.hh"
#include "mythRequestParser.hh"
#include "cJSON.h"
class mythServerMap
{
public:
	static mythServerMap* CreateNew(){
		return new mythServerMap();
	}
	void AppendClient(int cameraid, MythSocket* socket, const char* cameratype);
	void AppendClient(int cameraid);
	void AppendClient(int cameraid, MythSocket* people);
	void AppendClient(mythRequestParser* parser, MythSocket* people);
	void AppendClient(cJSON* parser);
	void DropClient(MythSocket* socket);
	~mythServerMap();
protected:
	mythServerMap();
private:
	int FreeCameraID(mythStreamServer* server);
	mythStreamServer** cameraid_pool;
	std::map<int, mythStreamServer*> servermap;
	std::map<std::string, mythStreamServer*> tempservermap;
	std::mutex mapmutex;
};

