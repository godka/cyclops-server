#pragma once
#include "mythVirtualDecoder.hh"
#include <math.h>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
typedef unsigned int UINT;
typedef unsigned char BYTE;
typedef unsigned long DWORD;
class mythH264Decoder :
	public mythVirtualDecoder
{
public:
	static mythH264Decoder* CreateNew(const char* filename){
		return new mythH264Decoder(filename);
	}
	~mythH264Decoder();
	int MainLoop();
protected:
	mythH264Decoder(const char* filename);
	int h264_decode_sps(BYTE * buf, unsigned int nLen, unsigned &width, unsigned &height, unsigned &fps);


	UINT Ue(BYTE *pBuff, UINT nLen, UINT &nStartBit);
	int Se(BYTE *pBuff, UINT nLen, UINT &nStartBit);
	DWORD u(UINT BitCount, BYTE * buf, UINT &nStartBit);
	void de_emulation_prevention(BYTE* buf, unsigned int* buf_size);
private:
	std::string _filename;
};
