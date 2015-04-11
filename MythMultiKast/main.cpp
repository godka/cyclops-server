#include <iostream>
#include "MythConfig.hh"
#include "mythStreamMapServer.hh"
#include "mythVirtualSqlite.hh"

int main(int args,char** argv)
{
	SDL_Init(SDL_INIT_TIMER);
#ifdef AUTOSTART
	mythStreamMapServer* streammapserver = mythStreamMapServer::CreateNew(streamserverport,true);
#else
	mythStreamMapServer* streammapserver = mythStreamMapServer::CreateNew(streamserverport,false);
#endif
	streammapserver->StartServer();

	if (args > 1){
		mythVirtualSqlite::GetInstance()->SetSQLIP(argv[1]);
	}
#ifdef MYTH_RUN_IN_SERVER
	char input[256];
	for(;;){
		printf(">");
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
