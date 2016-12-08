#include "mythStreamClient.hh"


mythStreamClient::mythStreamClient(MythSocket* people)
	:mythBaseClient(people)
{
	iFrameCount = 4096;
}


int mythStreamClient::DataCallBack(PacketQueue* pkt)
{
	auto data = pkt->Packet;
	auto len = pkt->PacketLength;
	auto timestamp = pkt->TimeStamp;
	if (_basictick == ~0){
		_basictick = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}
	if (timestamp == ~0){
		timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - _basictick;
	}
	char tempbuf[256] = { 0 };

	if (isfirst){
		if (mythSendMessage(firstrequest) < 0){
			return -1;
		}
		isfirst = false;
	}
	auto t = std::chrono::system_clock::now();
	auto tt = std::chrono::system_clock::to_time_t(t);
	struct tm * timeinfo = localtime(&tt);
	sprintf(tempbuf,
		"Content-Type: image/h264\r\nContent_Length: %06d Stamp:%04x%02x%02x %04x%02x%02x %02d %08x\n\n", len,
		1900 + timeinfo->tm_year, 1 + timeinfo->tm_mon, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,
		timestamp % 100, iFrameCount++);
	if (mythSendMessage(tempbuf) < 0){
		return -1;
	}
	if (mythSendMessage(data, len) < 0){
		return -1;
	}
	if (mythSendMessage(" \n\n--myboundary\n") < 0){
		return -1;
	}
	return 0;
}

mythStreamClient::~mythStreamClient()
{
}
