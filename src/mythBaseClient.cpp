#include "mythBaseClient.hh"

mythBaseClient::mythBaseClient(MythSocket* people, const char* CameraType)
{
	mpeople = people;
	isfirst = true;
	iFrameCount = 4096;
	if (CameraType){
		if (strcmp(CameraType, "zyh264") == 0){
			m_cameratype = 0;	//stream
		}
		else{ 
			m_cameratype = 1;//HTTP
		}
	}
}

mythBaseClient::~mythBaseClient(void)
{
}

int mythBaseClient::DataCallBack(void* data, int len)
{
	char tempbuf[256] = { 0 };

	if (isfirst == true){
		mythSendMessage((void*) firstrequest);
		isfirst = false;
	}
	auto t = std::chrono::system_clock::now();
	auto tt = std::chrono::system_clock::to_time_t(t);
	struct tm * timeinfo = localtime(&tt);
	sprintf(tempbuf,
		"Content-Type: image/h264\r\nContent_Length: %06d Stamp:%04x%02x%02x %04x%02x%02x %02d %08x\n\n", len,
		1900 + timeinfo->tm_year, 1 + timeinfo->tm_mon, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,
		std::chrono::duration_cast<std::chrono::milliseconds>(t.time_since_epoch()).count() % 100, iFrameCount++);
	if (m_cameratype == 0)
		mythSendMessage(tempbuf);
	mythSendMessage(data, len);
	if (m_cameratype == 0)
		mythSendMessage((void*)" \n\n--myboundary\n");
	return 0;
}

mythBaseClient* mythBaseClient::CreateNew(MythSocket* people, const char* CameraType)
{
	return new mythBaseClient(people, CameraType);
}

int mythBaseClient::mythSendMessage(void* data, int length)
{
	if (length == -1)
		length = strlen((char*) data);
	int tmplength = 0;
	
	if (mpeople){
		tmplength = mpeople->socket_SendStr((char*) data, length);
	}
	return tmplength;
}
