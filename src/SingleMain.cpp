﻿#include "MythConfig.hh"
#include "mythStreamMapServer.hh"
#include "mythVirtualSqlite.hh"
#include "mythGlobal.hh"
#include "MainInclude.hh"
#include <string.h>
int SingleMain(int args, char** argv)
{
	mythGlobal::GetInstance()->global_filename = argv[0];
	initalsocket(streamserverport);
	return 0;
}