#include "mythWorkBenchServer.hh"


mythWorkBenchServer::mythWorkBenchServer(int port)
	:mythVirtualServer(port)
{

}

void mythWorkBenchServer::ServerDecodeCallBack(MythSocket* people, char* data, int datalength)
{
	closePeople(people);
}

void mythWorkBenchServer::ServerCloseCallBack(MythSocket* people)
{

}

mythWorkBenchServer::~mythWorkBenchServer()
{
}
