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
	UINT Ue(BYTE *pBuff, UINT nLen, UINT &nStartBit) {
		UINT nZeroNum = 0;
		while (nStartBit < nLen * 8) {
			if (pBuff[nStartBit / 8] & (0x80 >> (nStartBit % 8)))
				break;
			nZeroNum++;
			nStartBit++;
		}
		nStartBit++;

		DWORD dwRet = 0;
		for (UINT i = 0; i < nZeroNum; i++) {
			dwRet <<= 1;
			if (pBuff[nStartBit / 8] & (0x80 >> (nStartBit % 8))) {
				dwRet += 1;
			}
			nStartBit++;
		}
		return (1 << nZeroNum) - 1 + dwRet;
	}

	int Se(BYTE *pBuff, UINT nLen, UINT &nStartBit) {
		int UeVal = Ue(pBuff, nLen, nStartBit);
		double k = UeVal;
		int nValue = (int) floor(k / 2);
		if (UeVal % 2 == 0)
			nValue = -nValue;
		return nValue;
	}

	DWORD u(UINT BitCount, BYTE * buf, UINT &nStartBit) {
		DWORD dwRet = 0;
		for (UINT i = 0; i < BitCount; i++) {
			dwRet <<= 1;
			if (buf[nStartBit / 8] & (0x80 >> (nStartBit % 8)))
				dwRet += 1;
			nStartBit++;
		}
		return dwRet;
	}

	void de_emulation_prevention(BYTE* buf, unsigned int* buf_size) {
		size_t i = 0, j = 0;
		BYTE* tmp_ptr = NULL;
		unsigned int tmp_buf_size = 0;
		int val = 0;
		tmp_ptr = buf;
		tmp_buf_size = *buf_size;
		for (i = 0; i < (tmp_buf_size - 2); i++) {
			val = (tmp_ptr[i] ^ 0x00) + (tmp_ptr[i + 1] ^ 0x00)
				+ (tmp_ptr[i + 2] ^ 0x03);
			if (val == 0) {
				for (j = i + 2; j < tmp_buf_size - 1; j++)
					tmp_ptr[j] = tmp_ptr[j + 1];
				(*buf_size)--;
			}
		}
	}

	int h264_decode_sps(BYTE * buf, unsigned int nLen, unsigned &width, unsigned &height, unsigned &fps) {
		UINT StartBit = 0;
		unsigned chroma_format_idc = 0;
		unsigned frame_crop_left_offset;
		unsigned frame_crop_right_offset;
		unsigned frame_crop_top_offset;
		unsigned frame_crop_bottom_offset;
		fps = 0;
		de_emulation_prevention(buf, &nLen);

		//forbidden_zero_bit =
		u(1, buf, StartBit);
		//nal_ref_idc =
		u(2, buf, StartBit);
		int nal_unit_type = u(5, buf, StartBit);
		if (nal_unit_type == 7) {
			int profile_idc = u(8, buf, StartBit);
			//constraint_set0_flag =
			u(1, buf, StartBit); //(buf[1] & 0x80)>>7;
			//constraint_set1_flag =
			u(1, buf, StartBit); //(buf[1] & 0x40)>>6;
			//constraint_set2_flag =
			u(1, buf, StartBit); //(buf[1] & 0x20)>>5;
			//constraint_set3_flag =
			u(1, buf, StartBit); //(buf[1] & 0x10)>>4;
			//reserved_zero_4bits =
			u(4, buf, StartBit);
			//level_idc =
			u(8, buf, StartBit);

			//seq_parameter_set_id =
			Ue(buf, nLen, StartBit);

			if (profile_idc == 100 || profile_idc == 110 || profile_idc == 122
				|| profile_idc == 144) {
				chroma_format_idc = Ue(buf, nLen, StartBit);
				if (chroma_format_idc == 3)
					//residual_colour_transform_flag =
					u(1, buf, StartBit);
				//bit_depth_luma_minus8 =
				Ue(buf, nLen, StartBit);
				//bit_depth_chroma_minus8 =
				Ue(buf, nLen, StartBit);
				//qpprime_y_zero_transform_bypass_flag =
				u(1, buf, StartBit);

				int seq_scaling_matrix_present_flag = u(1, buf, StartBit);
				if (seq_scaling_matrix_present_flag) {
					for (int i = 0; i < 8; i++) {
						//seq_scaling_list_present_flag[i] =
						u(1, buf, StartBit);
					}
				}
			}
			//log2_max_frame_num_minus4 =
			Ue(buf, nLen, StartBit);
			int pic_order_cnt_type = Ue(buf, nLen, StartBit);
			if (pic_order_cnt_type == 0)
				//log2_max_pic_order_cnt_lsb_minus4 =
				Ue(buf, nLen, StartBit);
			else if (pic_order_cnt_type == 1) {
				//delta_pic_order_always_zero_flag =
				u(1, buf, StartBit);
				//offset_for_non_ref_pic =
				Se(buf, nLen, StartBit);
				//offset_for_top_to_bottom_field =
				Se(buf, nLen, StartBit);
				int num_ref_frames_in_pic_order_cnt_cycle = Ue(buf, nLen, StartBit);

				int *offset_for_ref_frame = new int[num_ref_frames_in_pic_order_cnt_cycle];
				for (int i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++)
					offset_for_ref_frame[i] = Se(buf, nLen, StartBit);
				delete [] offset_for_ref_frame;
			}

			//num_ref_frames =
			Ue(buf, nLen, StartBit);
			//gaps_in_frame_num_value_allowed_flag =
			u(1, buf, StartBit);
			int pic_width_in_mbs_minus1 = Ue(buf, nLen, StartBit);
			int pic_height_in_map_units_minus1 = Ue(buf, nLen, StartBit);

			int frame_mbs_only_flag = u(1, buf, StartBit);
			if (!frame_mbs_only_flag)
				//mb_adaptive_frame_field_flag =
				u(1, buf, StartBit);

			//direct_8x8_inference_flag =
			u(1, buf, StartBit);
			int frame_cropping_flag = u(1, buf, StartBit);
			if (frame_cropping_flag) {
				frame_crop_left_offset = Ue(buf, nLen, StartBit);
				frame_crop_right_offset = Ue(buf, nLen, StartBit);
				frame_crop_top_offset = Ue(buf, nLen, StartBit);
				frame_crop_bottom_offset = Ue(buf, nLen, StartBit);
			}

			width = (pic_width_in_mbs_minus1 + 1) * 16;
			height = (2 - frame_mbs_only_flag) * (pic_height_in_map_units_minus1 + 1) * 16;

			if (frame_cropping_flag) {
				unsigned int crop_unit_x;
				unsigned int crop_unit_y;
				if (0 == chroma_format_idc) {
					// monochrome
					crop_unit_x = 1;
					crop_unit_y = 2 - frame_mbs_only_flag;
				}
				else if (1 == chroma_format_idc) {
					// 4:2:0
					crop_unit_x = 2;
					crop_unit_y = 2 * (2 - frame_mbs_only_flag);
				}
				else if (2 == chroma_format_idc) {
					// 4:2:2
					crop_unit_x = 2;
					crop_unit_y = 2 - frame_mbs_only_flag;
				}
				else {
					// 3 == chroma_format_idc   // 4:4:4
					crop_unit_x = 1;
					crop_unit_y = 2 - frame_mbs_only_flag;
				}

				width -= crop_unit_x * (frame_crop_left_offset + frame_crop_right_offset);
				height -= crop_unit_y * (frame_crop_top_offset + frame_crop_bottom_offset);
			}

			int vui_parameter_present_flag = u(1, buf, StartBit);
			if (vui_parameter_present_flag) {
				int aspect_ratio_info_present_flag = u(1, buf, StartBit);
				if (aspect_ratio_info_present_flag) {
					int aspect_ratio_idc = u(8, buf, StartBit);
					if (aspect_ratio_idc == 255) {
						//sar_width =
						u(16, buf, StartBit);
						//sar_height =
						u(16, buf, StartBit);
					}
				}

				int overscan_info_present_flag = u(1, buf, StartBit);
				if (overscan_info_present_flag)
					//overscan_appropriate_flagu =
					u(1, buf, StartBit);

				int video_signal_type_present_flag = u(1, buf, StartBit);
				if (video_signal_type_present_flag) {
					//video_format =
					u(3, buf, StartBit);
					//video_full_range_flag =
					u(1, buf, StartBit);
					int colour_description_present_flag = u(1, buf, StartBit);
					if (colour_description_present_flag) {
						//colour_primaries =
						u(8, buf, StartBit);
						//transfer_characteristics =
						u(8, buf, StartBit);
						//matrix_coefficients =
						u(8, buf, StartBit);
					}
				}

				int chroma_loc_info_present_flag = u(1, buf, StartBit);
				if (chroma_loc_info_present_flag) {
					//chroma_sample_loc_type_top_field =
					Ue(buf, nLen, StartBit);
					//chroma_sample_loc_type_bottom_field =
					Ue(buf, nLen, StartBit);
				}

				int timing_info_present_flag = u(1, buf, StartBit);
				if (timing_info_present_flag) {
					int num_units_in_tick = u(32, buf, StartBit);
					int time_scale = u(32, buf, StartBit);
					fps = time_scale / (2 * num_units_in_tick);
				}
			}
			return 1;
		}
		return 0;
	}

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

