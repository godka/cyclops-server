#pragma once
#include "mythAvlist.hh"
#include "mythFFmpeg.hh"
#include "mythFFmpegEncoder.hh"
#include <thread>
#define mythH264 0
#define mythH265 1
class mythMediaPipeline :
	public mythAvlist
{
public:
	void decodecallback(char** pdata, int* plength, int width, int height);
	void encodecallback(char* pdata, int plength);
	static mythMediaPipeline* CreateNew(void* ptr,int streamtype, int level){
		return new mythMediaPipeline(ptr,streamtype, level);
	}
	int encodethread();
	~mythMediaPipeline();
	void PutMedia(void* data, int len);
protected:
	mythMediaPipeline(void* ptr,int streamtype, int level);
private:
	mythFFmpeg* m_decoder;
	mythFFmpegEncoder* m_encoder;
	void* _ptr;
	int _streamtype;
	int _level;
	mythAvlist* yuvlist;
	std::thread* _thread;
	bool isrunning;
};

