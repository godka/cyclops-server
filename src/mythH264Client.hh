#pragma once
#include "mythBaseClient.hh"
class mythH264Client :
	public mythBaseClient
{
public:
	static mythH264Client* CreateNew(MythSocket* people){
		return new mythH264Client(people);
	}
	virtual int DataCallBack(PacketQueue* pkt);
	~mythH264Client();
protected:
	mythH264Client(MythSocket* people);
private:
	int iFrameCount;
};

