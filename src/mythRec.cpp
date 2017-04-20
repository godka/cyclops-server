#include "mythRec.hh"
#include <chrono>

typedef unsigned int UINT;
typedef unsigned char BYTE;
typedef unsigned long DWORD;

#define FLV_TAG_HEAD_LEN 11
#define FLV_PRE_TAG_LEN 4
mythRec::mythRec()
{
	record_last_pkt = NULL;
	isRecording = false;
	file = NULL;
	isfirst = true;
	RECORD_DELAY_TIME = mythIniFile::GetInstance()->GetInt("record", "delay_time", 30);
}


mythRec::~mythRec()
{
	StopRecord();
	if (_sps){
		delete [] _sps;
	}
	if (_pps){
		delete [] _pps;
	}
}

uint8_t * mythRec::get_nal(uint32_t *len, uint8_t **offset, uint8_t *start, uint32_t total)
{
	uint32_t info;
	uint8_t *q;
	uint8_t *p = *offset;
	*len = 0;

	while (1) {
		info = find_start_code(p);
		if (info > 0)
			break;
		p++;
		if ((p - start) >= total)
			return NULL;
	}
	q = p + info;
	p = q;
	while (1) {
		info = find_start_code(p);
		if (info > 0)
			break;
		p++;
		if ((p - start) >= total){
			//maybe cause error
			break;
		}
	}

	*len = (p - q);
	*offset = p;
	return q;
}

int mythRec::writespspps(uint8_t * sps, uint32_t spslen, uint8_t * pps, uint32_t ppslen, uint32_t timestamp)
{
	uint32_t body_len = spslen + ppslen + 16;
	uint32_t output_len = body_len + FLV_TAG_HEAD_LEN + FLV_PRE_TAG_LEN;
	uint8_t * output = (uint8_t*) malloc(output_len);
	uint32_t offset = 0;
	// flv tag header
	output[offset++] = 0x09; //tagtype video
	output[offset++] = (uint8_t) (body_len >> 16); //data len
	output[offset++] = (uint8_t) (body_len >> 8); //data len
	output[offset++] = (uint8_t) (body_len); //data len
	output[offset++] = (uint8_t) (timestamp >> 16); //time stamp
	output[offset++] = (uint8_t) (timestamp >> 8); //time stamp
	output[offset++] = (uint8_t) (timestamp); //time stamp
	output[offset++] = (uint8_t) (timestamp >> 24); //time stamp
	output[offset++] = 0x00; //stream id 0
	output[offset++] = 0x00; //stream id 0
	output[offset++] = 0x00; //stream id 0

	//flv VideoTagHeader
	output[offset++] = 0x17; //key frame, AVC
	output[offset++] = 0x00; //avc sequence header
	output[offset++] = 0x00; //composit time ??????????
	output[offset++] = 0x00; // composit time
	output[offset++] = 0x00; //composit time

	//flv VideoTagBody --AVCDecoderCOnfigurationRecord
	output[offset++] = 0x01; //configurationversion
	output[offset++] = sps[1]; //avcprofileindication
	output[offset++] = sps[2]; //profilecompatibilty
	output[offset++] = sps[3]; //avclevelindication
	output[offset++] = 0xff; //reserved + lengthsizeminusone
	output[offset++] = 0xe1; //numofsequenceset
	output[offset++] = (uint8_t) (spslen >> 8); //sequence parameter set length high 8 bits
	output[offset++] = (uint8_t) (spslen); //sequence parameter set  length low 8 bits
	memcpy(output + offset, sps, spslen); //H264 sequence parameter set
	offset += spslen;
	output[offset++] = 0x01; //numofpictureset
	output[offset++] = (uint8_t) (ppslen >> 8); //picture parameter set length high 8 bits
	output[offset++] = (uint8_t) (ppslen); //picture parameter set length low 8 bits
	memcpy(output + offset, pps, ppslen); //H264 picture parameter set

	//no need set pre_tag_size ,RTMP NO NEED
	// flv test 
	offset += ppslen;
	uint32_t fff = body_len + FLV_TAG_HEAD_LEN;
	output[offset++] = (uint8_t) (fff >> 24); //data len
	output[offset++] = (uint8_t) (fff >> 16); //data len
	output[offset++] = (uint8_t) (fff >> 8); //data len
	output[offset++] = (uint8_t) (fff); //data len
	_mutex.lock();
	if (file)
		fwrite(output, 1, output_len, file);
	_mutex.unlock();
	free(output);
	return 0;
}

int mythRec::writeavcframe(uint8_t * nal, uint32_t nal_len, uint32_t timestamp, bool IsIframe)
{
	uint32_t body_len = nal_len + 5 + 4; //flv VideoTagHeader +  NALU length
	uint32_t output_len = body_len + FLV_TAG_HEAD_LEN + FLV_PRE_TAG_LEN;
	uint8_t * output = (uint8_t*) malloc(output_len);
	uint32_t offset = 0;
	// flv tag header
	output[offset++] = 0x09; //tagtype video
	output[offset++] = (uint8_t) (body_len >> 16); //data len
	output[offset++] = (uint8_t) (body_len >> 8); //data len
	output[offset++] = (uint8_t) (body_len); //data len
	output[offset++] = (uint8_t) (timestamp >> 16); //time stamp
	output[offset++] = (uint8_t) (timestamp >> 8); //time stamp
	output[offset++] = (uint8_t) (timestamp); //time stamp
	output[offset++] = (uint8_t) (timestamp >> 24); //time stamp
	output[offset++] = 0x00; //stream id 0
	output[offset++] = 0x00; //stream id 0
	output[offset++] = 0x00; //stream id 0

	//flv VideoTagHeader
	if (IsIframe)
		output[offset++] = 0x17; //key frame, AVC
	else
		output[offset++] = 0x27; //key frame, AVC

	output[offset++] = 0x01; //avc NALU unit
	output[offset++] = 0x00; //composit time ??????????
	output[offset++] = 0x00; // composit time
	output[offset++] = 0x00; //composit time

	output[offset++] = (uint8_t) (nal_len >> 24); //nal length 
	output[offset++] = (uint8_t) (nal_len >> 16); //nal length 
	output[offset++] = (uint8_t) (nal_len >> 8); //nal length 
	output[offset++] = (uint8_t) (nal_len); //nal length 
	memcpy(output + offset, nal, nal_len);

	//no need set pre_tag_size ,RTMP NO NEED
	offset += nal_len;
	uint32_t fff = body_len + FLV_TAG_HEAD_LEN;
	output[offset++] = (uint8_t) (fff >> 24); //data len
	output[offset++] = (uint8_t) (fff >> 16); //data len
	output[offset++] = (uint8_t) (fff >> 8); //data len
	output[offset++] = (uint8_t) (fff); //data len
	//fwrite(output, output_len, 1, flv_file);
	_mutex.lock();
	if (file)
		fwrite(output, 1, output_len, file);
	_mutex.unlock();
	free(output);
	return 0;
}

void mythRec::StartRecord()
{
	StopRecord();
	isfirst = true;
	auto t = std::chrono::system_clock::now();
	auto tt = std::chrono::system_clock::to_time_t(t);
	struct tm * timeinfo = localtime(&tt);
	auto duration_in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t.time_since_epoch());
	this->timenow = duration_in_ms.count();
	auto ms_part = duration_in_ms - std::chrono::duration_cast<std::chrono::seconds>(duration_in_ms);

	static char timefilename [64] = { 0 };
	sprintf(timefilename, "%04d-%02d-%02d-%02d-%02d-%02d-%03d.flv",
		1900 + timeinfo->tm_year, 1 + timeinfo->tm_mon, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, ms_part.count());
	mythLog::GetInstance()->printf("[mythrec]start record:%s\n", timefilename);

	_mutex.lock();
	file = fopen(timefilename, "wb");
	_mutex.unlock();

	isRecording = true;
	_basictick = ~0;
	_hassendIframe = false;
	_sps = nullptr; _pps = nullptr;
	_spslen = 0; _ppslen = 0;
}

void mythRec::WriteFrame(char* data,int len)
{
	if (!file)
		return;
	if (len == 0)
		return;
	if (!data){
		return;
	}
	auto timestamp = ~0;
	if (_basictick == ~0){
		_basictick = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}
	if (timestamp == ~0){
		timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - _basictick;
	}
	uint8_t *buf_offset = (uint8_t*) data;
	uint32_t nal_len;
	uint8_t *nal;
	if (isfirst){
		uint8_t flv_header[13] = { 0x46, 0x4c, 0x56, 0x01, 0x01, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00 };
		fwrite(flv_header, 1, 13, file);
		isfirst = false;
	}
	while (1) {
		nal = get_nal(&nal_len, &buf_offset, (uint8_t*) data, len);
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
			break;
		case 5:
			if (!_hassendIframe){
				if (_spslen > 0 && _ppslen > 0){
					if (writespspps((uint8_t*) _sps, _spslen, (uint8_t*) _pps, _ppslen, timestamp) < 0)
						return;
				}
			}
			if (writeavcframe(nal, nal_len, timestamp, true) < 0)
				return;
			_hassendIframe = true;
			break;
		default:
			if (_hassendIframe){
				if (writeavcframe(nal, nal_len, timestamp, false) < 0)
					return;
			}
			break;
		}
	}
	return;
}

void mythRec::StopRecord()
{
	if (file){
		_mutex.lock();
		fclose(file);
		file = NULL;
		_mutex.unlock();
		mythLog::GetInstance()->printf("[mythrec]stop record\n");
	}
	timenow = 0;
	isRecording = false;
	isfirst = false;
	_basictick = ~0;
	_hassendIframe = false;
	_sps = nullptr; _pps = nullptr;
	_spslen = 0; _ppslen = 0;
}

bool mythRec::IsMotion(PacketQueue* pkt)
{
	double ans = 1;
	if (record_last_pkt){
		//scan for each point
		double record_iter = 0, current_iter = 0;
		int iter_count = 0;
		int ystride = record_last_pkt->yuvPacketLength[0];
		int height = record_last_pkt->height;
		int width = record_last_pkt->width;
		for (int i = 0; i < height; i++){
			for (int j = 0; j < width; j++){
				int record_pixel = record_last_pkt->yuvPacket[0][i * ystride + j];
				int current_pixel = pkt->yuvPacket[0][i * ystride + j];
				record_iter += (double) record_pixel;
				current_iter += (double) current_pixel;
				iter_count++;
			}
		}
		double record_average = record_iter / (double) iter_count;
		double current_average = current_iter / (double) iter_count;
		double ins_a = 0;
		double ins_b = 0, ins_c = 0;
		for (int i = 0; i < height; i++){
			for (int j = 0; j < width; j++){
				int record_pixel = record_last_pkt->yuvPacket[0][i * ystride + j];
				int current_pixel = pkt->yuvPacket[0][i * ystride + j];
				double tmpa = ((double) record_pixel - record_average) * ((double) current_pixel - current_average);
				ins_a += tmpa;
				double tmpb = ((double) record_pixel - record_average) * ((double) record_pixel - record_average);
				ins_b += tmpb;
				double tmpc = ((double) current_pixel - current_average) * ((double) current_pixel - current_average);
				ins_c += tmpc;
			}
		}
		double ins_d = sqrt(ins_c * ins_b);
		ans = ins_a / ins_d;
	}
	DeepCopy(pkt);
	ans = abs(ans);
	if (ans < 0.99)
		return true;
	else
		return false;
}

void mythRec::DeepCopy(PacketQueue* pkt)
{
	if (!record_last_pkt){
		record_last_pkt = new PacketQueue();
		record_last_pkt->yuvPacket[0] = new unsigned char[pkt->yuvPacketLength[0] * pkt->height];
	}		
	memcpy(record_last_pkt->yuvPacket[0], pkt->yuvPacket[0], pkt->yuvPacketLength[0] * pkt->height);
	record_last_pkt->yuvPacketLength[0] = pkt->yuvPacketLength[0];
	record_last_pkt->width = pkt->width;
	record_last_pkt->height = pkt->height;
}
void mythRec::SingleStep(PacketQueue* pkt)
{
	if (isRecording){
		auto t = std::chrono::system_clock::now();
		auto tt = std::chrono::system_clock::to_time_t(t);
		auto duration_in_ms = std::chrono::duration_cast
			<std::chrono::milliseconds>(t.time_since_epoch());
		auto currenttime = duration_in_ms.count();
		auto delaytime = currenttime - timenow;
		if (delaytime > RECORD_DELAY_TIME * 1000){
			if (!IsMotion(pkt)){
				StopRecord();
			}
		}
	}
	else{
		if (IsMotion(pkt)){
			StartRecord();
		}
	}
}
