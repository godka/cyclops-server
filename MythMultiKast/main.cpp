#include <iostream>
#include "MythConfig.hh"
#include "mythStreamMapServer.hh"
#include "mythVirtualSqlite.hh"
#define MAX_ADDRESSES 10
int main(int args,char** argv)
{
	SDL_Init(SDL_INIT_TIMER);
#ifdef AUTOSTART
	mythStreamMapServer* streammapserver = mythStreamMapServer::CreateNew(streamserverport,true);
#else
	mythStreamMapServer* streammapserver = mythStreamMapServer::CreateNew(streamserverport,false);
#endif
	if (args > 1){
		mythVirtualSqlite::GetInstance()->SetSQLIP(argv[1]);
		SDL_Log("sqlip is now changed to %s\n", argv[1]);
	}
	streammapserver->StartServer();
#ifdef _WIN32
	char input[256];
	for(;;){
		SDL_Log(">");
		gets(input);
		if (mythcmp("exit")){
			break;
		}
		else if (mythcmp("show")){
			streammapserver->showAllClients();
		}
		else if (mythcmp("stop")){
			streammapserver->StopServer();
			cout << "stop servers OK" << endl;
		}
		else if (mythcmp("startall")){
			streammapserver->startAll();
		}
		else if (mythcmp("setSQL")){
			//mythVirtualSqlite::GetInstance()->SetSQLIP()
			//streammapserver->startAll();
		}
		else{
			system(input);
		}
	}
#endif
	streammapserver->StopServer();
	delete streammapserver;
	SDL_Quit();
	return 0;
}
