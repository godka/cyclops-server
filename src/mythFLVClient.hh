#pragma once
#include "mythBaseClient.hh"


#define FLV_TAG_HEAD_LEN 11
#define FLV_PRE_TAG_LEN 4
class mythFLVClient :
	public mythBaseClient
{
public:
	static mythFLVClient* CreateNew(MythSocket* people){
		return new mythFLVClient(people);
	}
	int DataCallBack(PacketQueue* pkt);
	~mythFLVClient();
protected:
	mythFLVClient(MythSocket* people);
private:
	int writespspps(uint8_t * sps, uint32_t spslen, uint8_t * pps, uint32_t ppslen, uint32_t timestamp);
	int writeavcframe(uint8_t * nal, uint32_t nal_len, uint32_t timestamp, bool IsIframe);
	char* _sps, *_pps;
	int _spslen; int _ppslen;
	unsigned int width, height, fps;
	long long timestart;
	bool _hassendIframe;
};

