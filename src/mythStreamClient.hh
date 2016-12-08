#pragma once
#include "mythBaseClient.hh"
class mythStreamClient :
	public mythBaseClient
{
public:
	static mythStreamClient* CreateNew(MythSocket* people){
		return new mythStreamClient(people);
	}
	virtual int DataCallBack(PacketQueue* pkt);
	~mythStreamClient();
protected:
	mythStreamClient(MythSocket* people);
private:
	int iFrameCount;
};

