#pragma once
#include "MythConfig.hh"
#include <mutex>
class mythRec
{
public:
	mythRec();
	~mythRec();
	void WriteFrame(char* data, int len);
	void SingleStep(PacketQueue* pkt);
protected:
	bool isRecording;
	long long timenow;
private:
	inline uint32_t find_start_code(uint8_t *buf){
		auto nal = *(unsigned int*) &buf[0];
		if ((nal & 0x00ffffff) == 0x00010000){
			return 3;
		}
		else if (nal == 0x01000000){
			return 4;
		}
		else{
			return 0;
		}
	}
	uint8_t * get_nal(uint32_t *len, uint8_t **offset, uint8_t *start, uint32_t total);
	int writespspps(uint8_t * sps, uint32_t spslen, uint8_t * pps, uint32_t ppslen, uint32_t timestamp);
	int writeavcframe(uint8_t * nal, uint32_t nal_len, uint32_t timestamp, bool IsIframe);
	char* _sps, *_pps;
	int _spslen; int _ppslen;
	FILE* file;
	void StartRecord();
	void StopRecord();
	bool IsMotion(PacketQueue* pkt);
	void DeepCopy(PacketQueue* pkt);
	int RECORD_DELAY_TIME;
	int RECORD_THRESHOLD;
	PacketQueue* record_last_pkt;
	long long _basictick;
	bool isfirst;
	bool _hassendIframe;
	std::mutex _mutex;
};

