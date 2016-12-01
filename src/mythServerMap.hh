#pragma once
#include <map>
#include <mutex>
#include "MythSocket.hh"
#include "mythBaseClient.hh"
#include "mythStreamServer.hh"
class mythServerMap
{
public:
	static mythServerMap* CreateNew(){
		return new mythServerMap();
	}
	void AppendClient(int cameraid, MythSocket* socket, const char* cameratype);
	void AppendClient(int cameraid);
	void AppendClient(int cameraid, MythSocket* people);
	void DropClient(MythSocket* socket);
	~mythServerMap();
protected:
	mythServerMap();
private:
	std::map<int, mythStreamServer*> servermap;
	std::mutex mapmutex;
};

