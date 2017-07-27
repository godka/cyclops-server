#include "mythClientFactory.hh"

mythBaseClient* mythClientFactory::CreateNew(MythSocket* people, const char* CameraType,const char* protocol)
{
	mythBaseClient* client = nullptr;
	if (CameraType){
		if (strcmp(CameraType, "zyh264") == 0){
			client = mythStreamClient::CreateNew(people);
		}
		else if (strcmp(CameraType, "flv") == 0){
			client = mythFLVClient::CreateNew(people);
		}
		else{
			client = mythH264Client::CreateNew(people);
		}
	}
	else{
		client = mythH264Client::CreateNew(people);
	}
	if (client){
		client->SetProtocol(protocol);
	}
	return client;
}
