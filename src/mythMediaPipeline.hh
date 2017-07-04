#pragma once
#include "MythConfig.hh"
#ifdef USEPIPELINE
#include "mythAvlist.hh"
#include "mythRec.hh"
#include "mythFFmpeg.hh"
#include "mythFFmpegEncoder.hh"
#include <thread>
#include <string>
#define mythH264 0
#define mythH265 1
class mythMediaPipeline :
	public mythAvlist, public mythRec
{
public:
	//void decodecallback(char** pdata, int* plength, int width, int height);
	void decodecallback(char** pdata, int* plength, int width, int height, char* oridata, int orilen);
	void encodecallback(char* pdata, int plength);
	static mythMediaPipeline* CreateNew(void* ptr,int streamtype, int level,const char* recfilename){
		return new mythMediaPipeline(ptr, streamtype, level, recfilename);
	}
	int encodethread();
	~mythMediaPipeline();
	void PutMedia(void* data, int len);
protected:
	mythMediaPipeline(void* ptr, int streamtype, int level, const char* recfilename = NULL);
private:
	mythFFmpeg* m_decoder;
#ifdef USEENCODER
	mythFFmpegEncoder* m_encoder;
#endif
	void* _ptr;
	int _streamtype;
	int _level;
	mythAvlist* yuvlist;
	std::thread* _thread;
	bool isrunning;
	std::string _recfilename;
};
#endif

