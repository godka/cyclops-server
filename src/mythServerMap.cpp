#include "mythServerMap.hh"


mythServerMap::mythServerMap()
{
}

void mythServerMap::AppendClient(int cameraid, MythSocket* people){
		people->isPush = 1;
		mythStreamServer* server = NULL;
		mapmutex.lock();
		if (servermap[cameraid] == NULL){
			server = mythStreamServer::CreateNew(cameraid, people);			//add a new server into map list,not found ,so create 
			servermap[cameraid] = server;
		}
		else{
			server = servermap[cameraid];									//find an existing server from map list,then add client into server list
			if (server){
				((mythProxyDecoder*) server->GetDecoder())->refreshSocket(people);	//problem
			}
		}
		mapmutex.unlock();
		if (server)
			server->start();
}
void mythServerMap::AppendClient(int cameraid,MythSocket* people,const char* cameratype)
{
	mythStreamServer* server = nullptr;
	//find cameraid from map
	if (servermap[cameraid] == NULL){
		mapmutex.lock();
		server = mythStreamServer::CreateNew(cameraid);			//add a new server into map list,not found ,so create 
		//server->SetOnCloseHandler(OnClientCloseStatic, this);
		servermap[cameraid] = server;
		mapmutex.unlock();
	}
	else{
		mapmutex.lock();
		server = servermap[cameraid];									//find an existing server from map list,then add client into server list
		mapmutex.unlock();
	}
	mythBaseClient* client = nullptr;
	mapmutex.lock();
	client = mythBaseClient::CreateNew(people, cameratype);
	people->data = server;
	people->addtionaldata = client;
	server->AppendClient(client);
	mapmutex.unlock();
	if (server)
		server->start();
}

void mythServerMap::AppendClient(int cameraid)
{
	if (servermap[cameraid] == nullptr){
		//if I using fork?
		mythStreamServer* server = mythStreamServer::CreateNew(cameraid);
		servermap[cameraid] = server;
		server->start();
	}
}

void mythServerMap::DropClient(MythSocket* people)
{
	if (people->addtionaldata){
		mapmutex.lock();
		mythBaseClient* client = (mythBaseClient*) people->addtionaldata;
		mythStreamServer* server = (mythStreamServer*) people->data;
		//people->sock = NULL;
		server->DropClient(client);
		delete client;
		client = nullptr;
		people->socket_CloseSocket();
		mapmutex.unlock();
	}

}

mythServerMap::~mythServerMap()
{
}