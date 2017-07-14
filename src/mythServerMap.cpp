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

void mythServerMap::AppendClient(cJSON* parser){
	mapmutex.lock();
	int iCount = cJSON_GetArraySize(parser);
	for (int i = 0; i < iCount; ++i) {
		cJSON* pItem = cJSON_GetArrayItem(parser, i);
		if (NULL == pItem)
			continue;
		cJSON* cjson_url = cJSON_GetObjectItem(pItem, "url");
		if (cjson_url){
			auto url = cjson_url->valuestring;
			mythStreamServer* server = nullptr;
			if (tempservermap[url] == NULL){
				server = mythStreamServer::CreateNew(pItem);
				auto cameraid = FreeCameraID(server);
				if (cameraid > 0 && server){
					server->SetID(cameraid);
				}
				tempservermap[url] = server;
			}
			else{
				server = tempservermap[url];
			}
		}
	}
	cJSON_Delete(parser);
	mapmutex.unlock();
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
		auto client = mythClientFactory::CreateNew(people, parser->Parse("Type").c_str(), parser->Parse("protocol").c_str());
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

void mythServerMap::DropClient(int sockfd)
{
	if (sockfd > 0){
		mapmutex.lock();
		for (auto &t : servermap){
			t.second->DropClient(sockfd);
		}
		for (auto &t : tempservermap){
			t.second->DropClient(sockfd);
		}
		mapmutex.unlock();
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
		auto servernum = server->getClientNumber();
		if (servernum == 0){
			mythLog::GetInstance()->printf("Delete server Start,cameraid=%d,url=%s\n", server->GetID(), server->ToString().c_str());
			server->stop();
			if (cameraid < 20000 && cameraid > 0){
				servermap[cameraid] = NULL;
			}
			else{
				auto _url = server->ToString();
				tempservermap[_url] = NULL;
			}
			delete server;
			mythLog::GetInstance()->printf("Delete server Success,cameraid=%d,url=%s\n", server->GetID(), server->ToString().c_str());
		}
		mapmutex.unlock();
	}
	/*why not close socket*/
	/*libevent says:I wanna close it in bufferevent_free,so close it now will cause stackflow*/
	//people->socket_CloseSocket();
	delete people;

}

mythServerMap::~mythServerMap()
{
	delete [] cameraid_pool;
}
