#pragma once
#include "mythBaseClient.hh"
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
	inline uint32_t find_start_code_core(uint8_t *buf, uint32_t zeros_in_startcode);
	inline uint32_t find_start_code(uint8_t *buf);
	uint8_t * get_nal(uint32_t *len, uint8_t **offset, uint8_t *start, uint32_t total);
	int writespspps(uint8_t * sps, uint32_t spslen, uint8_t * pps, uint32_t ppslen, uint32_t timestamp);
	int writeavcframe(uint8_t * nal, uint32_t nal_len, uint32_t timestamp, bool IsIframe);
	char* _sps, *_pps;
	int _spslen; int _ppslen;
	unsigned int width, height, fps;
	long long timestart;
	bool _hassendIframe;
};

