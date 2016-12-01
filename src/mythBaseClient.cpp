﻿#include "mythBaseClient.hh"
#include <iostream>

mythBaseClient::mythBaseClient(MythSocket* people, const char* CameraType)
{
	_hassendIframe = false;
	_sps = nullptr; _pps = nullptr;
	_spslen = 0; _ppslen = 0;
	width = 0; height = 0; fps = 0;
	mpeople = people;
	isfirst = true;
	iFrameCount = 4096;
	if (CameraType){
		if (strcmp(CameraType, "zyh264") == 0){
			m_cameratype = 0;	//stream
		}
		else if(strcmp(CameraType, "flv") == 0){
			m_cameratype = 2;//HTTP
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
	long long ts = 0;
	uint8_t *buf_offset = (uint8_t*) data;
	uint32_t nal_len;
	uint8_t *nal;
	if (m_cameratype == 2){
		if (isfirst){
			mythSendMessage((void*) flvfirstrequest);
			uint8_t flv_header[13] = { 0x46, 0x4c, 0x56, 0x01, 0x01, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00 };
			mythSendMessage(flv_header, 13);
			timestart = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			//ts = timestart;
			isfirst = false;
		}
		while (1) {
			nal = get_nal(&nal_len, &buf_offset, (uint8_t*)data, len);
			if (nal_len <= 3) break;
			int nalu = nal[0] & 0x1f;
			switch (nalu)
			{
			case 7:
				if (_spslen == 0){
					_sps = new char[nal_len];
					memcpy(_sps, nal, nal_len);
					_spslen = nal_len;
				}
				break;
			case 8:
				if (_ppslen == 0){
					_pps = new char[nal_len];
					memcpy(_pps, nal, nal_len);
					_ppslen = nal_len;
				}
				if (_spslen > 0 && _ppslen > 0){
					writespspps((uint8_t*) _sps, _spslen, (uint8_t*) _pps, _ppslen, ts);
				}
				break;
			case 5:
				ts = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - timestart;
				if (!_hassendIframe){
					if (_spslen > 0 && _ppslen > 0){
						writespspps((uint8_t*) _sps, _spslen, (uint8_t*) _pps, _ppslen, ts);
					}
				}
				writeavcframe(nal, nal_len, ts, true);
				_hassendIframe = true;
				//std::cout << ts << "ms" << std::endl;
				break;
			default:
				if (_hassendIframe){
					ts = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - timestart;
					writeavcframe(nal, nal_len, ts, false);
				}
				//std::cout << ts << "ms" << std::endl;
				break;
			}

		}

	}else{
		char tempbuf[256] = { 0 };

		if (isfirst){
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
	}
	return 0;
}

mythBaseClient* mythBaseClient::CreateNew(MythSocket* people, const char* CameraType)
{
	return new mythBaseClient(people, CameraType);
}

int mythBaseClient::mythSendMessage(void* data, int length)
{
#ifdef _DEBUG
	mythLog::GetInstance()->printf("writing to socket:%d\n", length);
#endif // _DEBUG
	if (length == -1)
		length = strlen((char*) data);
	int tmplength = 0;
	
	if (mpeople){
		tmplength = mpeople->socket_SendStr((char*) data, length);
	}
	return tmplength;
}
