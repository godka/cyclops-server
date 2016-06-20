#include "MythConfig.hh"
#include "MainInclude.hh"
#include "mythWorkBenchServer.hh"
int WorkBenchMain(int argc, char** argv){
	mythWorkBenchServer* server = mythWorkBenchServer::CreateNew(streamserverport);
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