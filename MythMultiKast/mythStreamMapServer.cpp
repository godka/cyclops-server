#include "mythStreamMapServer.hh"
#include "mythVirtualSqlite.hh"
//#define AUTOSTART

mythStreamMapServer::mythStreamMapServer(int port, bool start)
:mythVirtualServer(port)//, mythVirtualSqlite()
{
	if (start){
		startAll();
	}
	this->timerid = SDL_AddTimer(1000, TimerCallbackStatic, this);
}
int mythStreamMapServer::startAll(void){
	//if (start){
		//this->open("myth.db");
	int sum = 0;
	char sqlstr[256] = "select c.cameraid from videoserver as a,vstype as b,camera as c where a.vstypeid = b.vstypeid and a.videoserverid = c.videoserverid";
	mythStreamSQLresult* result = mythVirtualSqlite::GetInstance()->doSQLFromStream(sqlstr);
	while (result->MoveNext()){
		string cameraidstr = result->prase("CameraID");
		int cameraid = atoi(cameraidstr.c_str());
		if (servermap[cameraid] == NULL){
			mythStreamServer* server = mythStreamServer::CreateNew(cameraid);
			servermap[cameraid] = server;
			server->start();
			//SDL_Delay(100);
			cout << sum++ << "  cameraid:" << cameraidstr << " started." << endl;
		}
	}
	delete result;
	return 0;
	//}
}
mythStreamMapServer::~mythStreamMapServer(void)
{
	SDL_RemoveTimer(timerid);
}

mythStreamMapServer* mythStreamMapServer::CreateNew(int port,bool autostart)
{
	return new mythStreamMapServer(port,autostart);
}

void mythStreamMapServer::ServerDecodeCallBack( PEOPLE* people,char* data,int datalength )
{
	map<int,mythStreamServer*>::iterator Iter;
	int cameraid = -1;
	sscanf(data,"GET /CameraID=%d",&cameraid);
	if(cameraid != -1){
		mythStreamServer* server = NULL;
		Iter = servermap.find(cameraid);
		if(Iter == servermap.end() || Iter->second == NULL){
		//if (servermap[cameraid] != NULL){
			server = mythStreamServer::CreateNew(cameraid);
			servermap[cameraid] = server;
			server->start();
		}else{
			server = Iter->second;
		}
		mythBaseClient* client = NULL;
		if(people->addtionaldata){
			client = (mythBaseClient*)people->addtionaldata;
			//client->changeStreamServer(server);
		}else{
			client = mythBaseClient::CreateNew(people);
			people->data = server;
			people->addtionaldata = client;
			server->AppendClient(client);
		}
		//if(client)
		//	client->start();
	}
	return;
}

void mythStreamMapServer::showAllClients(){
	int sum = 0;
	int clientnum = 0;
	for (map<int, mythStreamServer*>::iterator Iter = servermap.begin(); Iter != servermap.end(); Iter++){
		if (Iter->second){
			int tmpnum = Iter->second->getClientNumber();
			cout << "server cameraid :" << Iter->second->m_cameraid << ";client num :" << tmpnum << endl;
			sum++;
			clientnum+=tmpnum;
		}
	}
	cout << "server num :" << sum << ";client sum :" << clientnum << endl;
}

void mythStreamMapServer::ServerCloseCallBack( PEOPLE* people )
{
	if(people->addtionaldata){
		mythBaseClient* client = (mythBaseClient*)people->addtionaldata;
		mythStreamServer* server = (mythStreamServer*) people->data;
		//people->sock = NULL;
		server->DropClient(client);
		delete client;
		client = NULL;
	}
	return;
}

Uint32 mythStreamMapServer::TimerCallbackStatic(Uint32 interval, void *param)
{
	mythStreamMapServer* mapserver = (mythStreamMapServer*) param;
	return mapserver->TimerCallback(interval);
}

Uint32 mythStreamMapServer::TimerCallback(Uint32 interval)
{
	for (map<int, mythStreamServer*>::iterator Iter = servermap.begin(); Iter != servermap.end(); Iter++){
		if (Iter->second){
			int tmpnum = Iter->second->getClientNumber();
			if (tmpnum == 0){
				servercount[tmpnum]++;
				if (servercount[tmpnum] >= 5){
					Iter->second->stop();
					//printf("delete server = %d\n", Iter->first);
					delete servermap[Iter->first];
					//Iter->first
					servermap[Iter->first] = NULL;
				}
			}
			else{
				servercount[tmpnum] = 0;
			}
		}
	}
	return interval;
}
