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
	int start = mythIniFile::GetInstance()->GetInt("config", "autostart");
	if (start || MYTH_FORCE_AUTOSTART){
		startAll();
	}
	servermap = mythServerMap::CreateNew();
}

int mythStreamMapServer::startAll(void)
{
	int sum = 0;
	char sqlstr[256] = "select c.cameraid from videoserver as a,vstype as b,camera as c where a.vstypeid = b.vstypeid and a.videoserverid = c.videoserverid";
	mythStreamSQLresult* result = mythVirtualSqlite::GetInstance()->doSQLFromStream(sqlstr);
	if (result){
		int ksum = 0;
		//read result from baseserver one by one
		while (result->MoveNext()){
			string cameraidstr = result->prase("CameraID");
			int cameraid = atoi(cameraidstr.c_str());
			servermap->AppendClient(cameraid);
			cout << ksum << "  cameraid:" << cameraid << " started." << endl;
			ksum++;
		}
		delete result;

	}
	return 0;
}

mythStreamMapServer::~mythStreamMapServer(void)
{

}

void mythStreamMapServer::OnClientClose(mythStreamServer * streamserver, int duration)
{
	//SDL_LockMutex(mapmutex);
	//servermap[streamserver->GetID()] = NULL;
	//SDL_UnlockMutex(mapmutex);
	printf("%d is closed\n", streamserver->GetID());
}

mythStreamMapServer* mythStreamMapServer::CreateNew(int port)
{
	return new mythStreamMapServer(port);
}

void mythStreamMapServer::ServerDecodeCallBack(MythSocket* people, char* data, int datalength)
{
	int cameraid = -1;
	char cameratype[20] = { 0 };
	SDL_sscanf(data,"GET /CameraID=%d&Type=%s ",&cameraid,cameratype);
	if (cameraid != -1){
		servermap->AppendClient(cameraid, people, cameratype);
	}
	else{
		people->socket_SendStr("404");
		closePeople(people);
	}
	return;
}

void mythStreamMapServer::showAllClients(){
	//int sum = 0;
	//int clientnum = 0;
	//for (map<int, mythStreamServer*>::iterator Iter = servermap.begin(); Iter != servermap.end(); Iter++){
	//	if (Iter->second){
	//		int tmpnum = Iter->second->getClientNumber();
	//		cout << "server cameraid :" << Iter->second->GetID() << ";client num :" << tmpnum << endl;
	//		sum++;
	//		clientnum += tmpnum;
	//	}
	//}
	//cout << "server num :" << sum << ";client sum :" << clientnum << endl;
}

void mythStreamMapServer::ServerCloseCallBack(MythSocket* people)
{
	servermap->DropClient(people);
	return;
}