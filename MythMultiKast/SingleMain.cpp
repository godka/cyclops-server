#include "MythConfig.hh"
#include "mythStreamMapServer.hh"
#include "mythVirtualSqlite.hh"
#include "mythGlobal.hh"
#include "mythRedisClient.hh"
#include "MainInclude.hh"
#include <string.h>
int SingleMain(int args, char** argv)
{
	mythGlobal::GetInstance()->global_filename = argv[0];
	if (args > 2){
		if (strcmp(argv[1], "-client") == 0){
			int cameraid = atoi(argv[2]);
			mythRedisClient* cli = mythRedisClient::CreateNew(cameraid);
			cli->start();
		}
	}
	else{
		initalsocket(streamserverport);
	}
	return 0;
}
