#pragma once
#include "mythAvlist.hh"
#include "mythFFmpeg.hh"
#include "mythFFmpegEncoder.hh"
#define mythH264 0
#define mythH265 1
class mythMediaPipeline :
	public mythAvlist
{
public:
	static void decodecallback_static(void *ptr, char** pdata, int* plength, int width, int height){
		if (ptr){
			mythMediaPipeline* tmp = (mythMediaPipeline*) ptr;
			tmp->decodecallback(pdata, plength, width, height);
		}
	}
	void decodecallback(char** pdata, int* plength, int width, int height);
	static void encodecallback_static(void *ptr, char* pdata, int plength){
		if (ptr){
			mythMediaPipeline* tmp = (mythMediaPipeline*) ptr;
			tmp->encodecallback(pdata, plength);
		}
	}
	void encodecallback(char* pdata, int plength);
	static mythMediaPipeline* CreateNew(void* ptr,int streamtype, int level){
		return new mythMediaPipeline(ptr,streamtype, level);
	}
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
};

