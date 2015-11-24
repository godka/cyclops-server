/********************************************************************
Created by MythKAst
©2013 MythKAst Some rights reserved.


You can build it with vc2010,gcc.
Anybody who gets this source code is able to modify or rebuild it anyway,
but please keep this section when you want to spread a new version.
It's strongly not recommended to change the original copyright. Adding new version
information, however, is Allowed. Thanks.
For the latest version, please be sure to check my website:
Http://code.google.com/mythkast


你可以用vc2010,gcc编译这些代码
任何得到此代码的人都可以修改或者重新编译这段代码，但是请保留这段文字。
请不要修改原始版权，但是可以添加新的版本信息。
最新版本请留意：Http://code.google.com/mythkast
B
MythKAst(asdic182@sina.com), in 2013 June.
*********************************************************************/
#include "mythStreamMapServer.hh"
#include "mythVirtualSqlite.hh"
//#define AUTOSTART
#ifndef WIN32
#include <unistd.h>  
#endif
//#include "mythUdp.hh"
mythStreamMapServer::mythStreamMapServer(int port)
:mythVirtualServer(port)//, mythVirtualSqlite()
{
	char tmpip[256] = { 0 };
	int start = read_profile_int("config", "autostart", 0 , MYTH_INFORMATIONINI_FILE);
	if (start || MYTH_FORCE_AUTOSTART){
		startAll();
	}
	mapmutex = SDL_CreateMutex();
#ifdef MYTH_STREAM_CLOSE
	this->timerid = SDL_AddTimer(1000, TimerCallbackStatic, this);
#endif
}
int mythStreamMapServer::startAll(void){
	//if (start){
		//this->open("myth.db");

#ifdef MYTH_STREAM_CLOSE
	SDL_RemoveTimer(timerid);
#endif
	int sum = 0;
	char sqlstr[256] = "select c.cameraid from videoserver as a,vstype as b,camera as c where a.vstypeid = b.vstypeid and a.videoserverid = c.videoserverid";
	mythStreamSQLresult* result = mythVirtualSqlite::GetInstance()->doSQLFromStream(sqlstr);
	if (result){
		int ksum = 0;
		//read result from baseserver one by one
		while (result->MoveNext()){
			string cameraidstr = result->prase("CameraID");
			int cameraid = atoi(cameraidstr.c_str());
			if (servermap[cameraid] == NULL){
				//if I using fork?
				mythStreamServer* server = mythStreamServer::CreateNew(cameraid);
				servermap[cameraid] = server;
				cout << ksum << "  cameraid:" << cameraid << " started." << endl;
				//SDL_Delay(500);
				server->start();
				ksum++;
			}
		}
		//cout << "sum client:" << ksum << endl;
		//delete result;
		//for (map<int, mythStreamServer*>::iterator Iter = servermap.begin(); Iter != servermap.end(); Iter++){
		//	if (Iter->second){
		//		Iter->second->start();
		//		cout << sum++ << "  cameraid:" << Iter->first << " started." << endl;
		//		SDL_Delay(1000);
		//	}
		//}
	}
	return 0;
	//}
}
mythStreamMapServer::~mythStreamMapServer(void)
{
#ifdef MYTH_STREAM_CLOSE
	SDL_RemoveTimer(timerid);
#endif
}

mythStreamMapServer* mythStreamMapServer::CreateNew(int port)
{
	return new mythStreamMapServer(port);
}

void mythStreamMapServer::ServerDecodeCallBack( PEOPLE* people,char* data,int datalength )
{
	map<int,mythStreamServer*>::iterator Iter;
	int cameraid = -1;
	SDL_sscanf(data,"GET /CameraID=%d",&cameraid);
	if(cameraid != -1){
		mythStreamServer* server = NULL;
		//find cameraid from map
		Iter = servermap.find(cameraid);
		if(Iter == servermap.end() || Iter->second == NULL){
		//if (servermap[cameraid] != NULL){
			SDL_LockMutex(mapmutex);
			server = mythStreamServer::CreateNew(cameraid);			//add a new server into map list,not found ,so create 
			servermap[cameraid] = server;
			SDL_UnlockMutex(mapmutex);
			server->start();
		}
		else{
			SDL_LockMutex(mapmutex);
			server = Iter->second;									//find an existing server from map list,then add client into server list
			SDL_UnlockMutex(mapmutex);
		}
		mythBaseClient* client = NULL;
		if(!people->addtionaldata){
			SDL_LockMutex(mapmutex);
			int usingthread = read_profile_int("config", "usethread", 0, MYTH_INFORMATIONINI_FILE);
			if (usingthread == 1)
				client = mythBaseClient::CreateNew(people,true);
			else
				client = mythBaseClient::CreateNew(people, false);
			people->data = server;
			people->addtionaldata = client;
			server->AppendClient(client);
			SDL_UnlockMutex(mapmutex);
		}
	}
	else{
		SDL_sscanf(data, "PUT /CameraID=%d", &cameraid);
		if (cameraid != -1){
			mythStreamServer* server = NULL;
			//find cameraid from map
			Iter = servermap.find(cameraid);
			if (Iter == servermap.end() || Iter->second == NULL){
				//if (servermap[cameraid] != NULL){
				SDL_LockMutex(mapmutex);
				server = mythStreamServer::CreateNew(cameraid,people);			//add a new server into map list,not found ,so create 
				servermap[cameraid] = server;
				SDL_UnlockMutex(mapmutex);
				server->start();
			}
			else{
				SDL_LockMutex(mapmutex);
				server = Iter->second;									//find an existing server from map list,then add client into server list
				if (server){
					((mythProxyDecoder*) server->GetDecoder())->refreshSocket(people);	//probiem
				}
				SDL_UnlockMutex(mapmutex);
			}
			/*mythBaseClient* client = NULL;
			if (!people->addtionaldata){
			SDL_LockMutex(mapmutex);
			int usingthread = read_profile_int("config", "usethread", 0, MYTH_INFORMATIONINI_FILE);
			if (usingthread == 1)
			client = mythBaseClient::CreateNew(people, true);
			else
			client = mythBaseClient::CreateNew(people, false);
			people->data = server;
			people->addtionaldata = client;
			server->AppendClient(client);
			SDL_UnlockMutex(mapmutex);
			}*/
		}
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
			clientnum += tmpnum;
		}
	}
	cout << "server num :" << sum << ";client sum :" << clientnum << endl;
}

void mythStreamMapServer::ServerCloseCallBack( PEOPLE* people )
{
	if (people->addtionaldata){
		SDL_LockMutex(mapmutex);
		mythBaseClient* client = (mythBaseClient*)people->addtionaldata;
		mythStreamServer* server = (mythStreamServer*) people->data;
		//people->sock = NULL;
		server->DropClient(client);
		delete client;
		client = NULL;
		SDL_UnlockMutex(mapmutex);
	}
	return;
}
//this will make a fatal error if using stream close,unknow bug
#ifdef MYTH_STREAM_CLOSE
Uint32 mythStreamMapServer::TimerCallbackStatic(Uint32 interval, void *param)
{
	mythStreamMapServer* mapserver = (mythStreamMapServer*) param;
	return mapserver->TimerCallback(interval);
}

Uint32 mythStreamMapServer::TimerCallback(Uint32 interval)
{
	char tmp[256] = { 0 };
	for (map<int, mythStreamServer*>::iterator Iter = servermap.begin(); Iter != servermap.end(); Iter++){
		if (Iter->second){
			int tmpnum = Iter->second->getClientNumber();
			int speed = Iter->second->GetDecoder()->GetTimeCount();
			sprintf(tmp, "status:%d:%d:%d\n", (int) Iter->first, tmpnum,speed);
			mythUdp::GetInstance()->SendData(tmp);
			if (tmpnum == 0){
			//lock
				servercount[tmpnum]++;
				if (servercount[tmpnum] >= 5){
					SDL_LockMutex(mapmutex);
					Iter->second->stop();
					//printf("delete server = %d\n", Iter->first);
					delete Iter->second;
					servermap[Iter->first] = NULL;
					SDL_UnlockMutex(mapmutex);
					//Iter->first
					sprintf(tmp, "clear:%d\n", (int) Iter->first);
					mythUdp::GetInstance()->SendData(tmp);
				}
			}
			else{
				servercount[tmpnum] = 0;
			}
		}
	}
	return interval;
}
#endif