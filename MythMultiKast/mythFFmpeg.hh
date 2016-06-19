#pragma once
extern "C"{
#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavutil/avutil.h"
#include "libavdevice/avdevice.h"
#include "libavutil/opt.h"       // for av_opt_set  
}

class mythFFmpeg
{
public:
	typedef void (responseHandler)(void *myth,char** pdata, int* plength,int width,int height);
	mythFFmpeg(void* phwnd);
	static void RGB2yuv(int width, int height, int stride, const void* src, void** dst);
	~mythFFmpeg(void);
	bool Init();
	void Cleanup();
	void ProcessFrame(unsigned char* framePtr, int frameSize,responseHandler* response);
	static void yuv2RGB(int width,int height,const void** src,int* src_linesize,void** dst);
	static void yuv2RGB(int width,int height,
	const void* ysrc,const void* usrc,const void* vsrc,
	int ysize,int usize,int vsize,void** dst);
	static mythFFmpeg* CreateNew(void* hwnd);//º¯ÊýÈë¿Ú
public:
	void* hwnd;
protected:
	AVCodec* avCodec;
	AVCodecContext* avCodecContext;
	AVFrame *avFrame;
};

