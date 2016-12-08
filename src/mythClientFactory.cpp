#include "mythClientFactory.hh"

mythBaseClient* mythClientFactory::CreateNew(MythSocket* people, const char* CameraType)
{
	if (CameraType){
		if (strcmp(CameraType, "zyh264") == 0){
			return mythStreamClient::CreateNew(people);
		}
		else if (strcmp(CameraType, "flv") == 0){
			return mythFLVClient::CreateNew(people);
		}
		else{
			return mythH264Client::CreateNew(people);
		}
	}
	else{
		return nullptr;
	}
}
