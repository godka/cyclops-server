#include "MythConfig.hh"
#include "MainInclude.hh"
#include "mythOpenresyServer.hh"
int OpenRestyMain(int args, char** argv){
	mythOpenresyServer* server = mythOpenresyServer::CreateNew(streamserverport);
	server->StartServer();
#ifdef _DEBUG
	char input[256];
	for (;;){
		printf(">");
		gets(input);
		system(input);
	}
#endif
	return 0;
}