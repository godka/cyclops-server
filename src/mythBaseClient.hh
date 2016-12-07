#pragma once
#include "MythConfig.hh"
extern "C"{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
};
#include "MythSocket.hh"
#include "mythAvlist.hh"
#include <thread>
#include <chrono>
#include <memory.h>
#include <string.h>
#include <stdio.h>

#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <memory.h>
#include <math.h>

typedef unsigned int UINT;
typedef unsigned char BYTE;
typedef unsigned long DWORD;

#define FLV_TAG_HEAD_LEN 11
#define FLV_PRE_TAG_LEN 4

class mythBaseClient
{
public:
	bool isfirst;
	static mythBaseClient* CreateNew(MythSocket* people,const char* CameraType = NULL);
	int DataCallBack(PacketQueue* pkt);
	~mythBaseClient(void);
private:
	MythSocket* mpeople;
	int mythSendMessage(void* data, int length = -1);
	int iFrameCount;
	int m_cameratype;
	int _mode;
	long long _basictick;

	static uint32_t find_start_code_core(uint8_t *buf, uint32_t zeros_in_startcode)
	{
		uint32_t info;
		uint32_t i;

		info = 1;
		if ((info = (buf[zeros_in_startcode] != 1) ? 0 : 1) == 0)
			return 0;

		for (i = 0; i < zeros_in_startcode; i++)
			if (buf[i] != 0)
			{
				info = 0;
				break;
			};

		return info;
	}
	static uint32_t find_start_code(uint8_t *buf)
	{
		if (find_start_code_core(buf, 2) > 0){
			return 3;
		}
		else if (find_start_code_core(buf, 3) > 0){
			return 4;
		}
		else{
			return 0;
		}
	}
	uint8_t * get_nal(uint32_t *len, uint8_t **offset, uint8_t *start, uint32_t total)
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
	int writespspps(uint8_t * sps, uint32_t spslen, uint8_t * pps, uint32_t ppslen, uint32_t timestamp)
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
		if (mythSendMessage(output, output_len) < 0){
			//RTMP Send out
			free(output);
			return -1;
		}
		else{
			free(output);
		}
		return 0;
	}

	int writeavcframe(uint8_t * nal, uint32_t nal_len, uint32_t timestamp, bool IsIframe)
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
		if (mythSendMessage(output, output_len) < 0){
			free(output);
			return -1;
		}
		else{
			free(output);
			return 0;
		}
	}
	char* _sps , *_pps;
	int _spslen; int _ppslen;
	unsigned int width, height, fps;
	long long timestart;
	bool _hassendIframe;
	void CloseClient();
	//void OnFlvFrame(unsigned char* data, int len);
protected:
	mythBaseClient(MythSocket* people, const char* CameraType);
};

