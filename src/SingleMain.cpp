#include "MythConfig.hh"
#include "mythStreamMapServer.hh"
#include "mythVirtualSqlite.hh"
#include "mythGlobal.hh"
#include "MainInclude.hh"
#include <string.h>
int SingleMain(int args, char** argv)
{
	mythGlobal::GetInstance()->global_filename = argv[0];
	int basicport[] = { 80, streamserverport, 0 };
	for (int i = 0; i < sizeof(basicport) / sizeof(int); i++){
		if (initalsocket(basicport[i]) > 0)
			continue;
	}
	return 0;
}
