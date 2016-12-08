#include "mythServerMap.hh"
#include <memory.h>

mythServerMap::mythServerMap()
{
	mythLog::GetInstance()->printf("Initalizing Servermap\n");
	cameraid_pool = new mythStreamServer*[STREAMSERVERMAX];
	memset(cameraid_pool, NULL, STREAMSERVERMAX*sizeof(mythStreamServer*));
}

int mythServerMap::FreeCameraID(mythStreamServer* server)
{
	for (int i = 0; i < STREAMSERVERMAX; i++){
		if (cameraid_pool[i] == NULL){
			cameraid_pool[i] = server;
			return i + 20000;
		}
	}
	return -1;
}

void mythServerMap::AppendClient(mythRequestParser* parser, MythSocket* people){
	auto url = parser->Parse("url");
	mythStreamServer* server = nullptr;
	mapmutex.lock();
	if (tempservermap[url] == NULL){
		server = mythStreamServer::CreateNew(parser);
		auto cameraid = FreeCameraID(server);
		if (cameraid > 0 && server){
			server->SetID(cameraid);
		}
		tempservermap[url] = server;
	}
	else{
		server = tempservermap[url];
	}
	if (server){
		auto client = mythClientFactory::CreateNew(people, parser->Parse("Type").c_str());
		if (client){
			people->data = server;
			people->addtionaldata = client;
			server->AppendClient(client);
			server->start();
		}
	}
	mapmutex.unlock();
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
	if (cameraid >= 20000 && cameraid < 20000 + STREAMSERVERMAX){
		server = cameraid_pool[cameraid - 20000];
	}
	else{
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
	}

	if (server){
		auto client = mythClientFactory::CreateNew(people, cameratype);
		people->data = server;
		people->addtionaldata = client;
		server->AppendClient(client);
		server->start();
	}
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
		//for temp link
		auto cameraid = server->GetID();
		if (cameraid >= 20000 && cameraid < 20000 + STREAMSERVERMAX){
			cameraid_pool[cameraid - 20000] = 0;
		}
		server->DropClient(client);
		delete client;
		client = nullptr;
		people->socket_CloseSocket();
		mapmutex.unlock();
	}

}

mythServerMap::~mythServerMap()
{
	delete [] cameraid_pool;
}
