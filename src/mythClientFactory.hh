#pragma once
#include "mythBaseClient.hh"
#include "mythH264Client.hh"
#include "mythStreamClient.hh"
#include "mythFLVClient.hh"
#include "MythSocket.hh"
class mythClientFactory
{
public:
	static mythBaseClient* CreateNew(MythSocket* people, const char* cameratype, const char* protocol = "http");
private:
	mythBaseClient* _client;
};

