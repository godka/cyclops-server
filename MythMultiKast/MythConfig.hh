#pragma once
#if (defined _WIN32) || (defined WIN32)
#	define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdio.h>
#ifndef ANDROID
#	include "SDL2/SDL.h"
#	include "SDL2/SDL_net.h"
#else
#	include "SDL.h"
#	include "SDL_net.h"
#endif
#include "mythIniFile.hh"

#if (defined _WIN32) || (defined WIN32)
#	
#	pragma comment(lib,"ws2_32")
//#	ifdef _DEBUG
//#		pragma comment(lib,"live555-multikast-debug.lib")
//#	else
//		#pragma comment(lib,"live555-multikast-release.lib")
//#	endif
#	pragma comment(lib,"SDL2_x86.lib")
#	pragma comment(lib,"SDL2main_x86.lib")
#	pragma comment(lib,"SDL2_net_x86.lib")

#	pragma comment(lib,"avcodec.lib")
#	pragma comment(lib,"avdevice.lib")
#	pragma comment(lib,"avfilter.lib")
#	pragma comment(lib,"avformat.lib")
#	pragma comment(lib,"avutil.lib")
#	pragma comment(lib,"postproc.lib")
#	pragma comment(lib,"swscale.lib")
#	pragma comment(lib,"swresample.lib")
#endif
#if (defined _WIN64) || (defined WIN64)
#	
#	pragma comment(lib,"ws2_32")
//#	ifdef _DEBUG
//#		pragma comment(lib,"live555-multikast-debug.lib")
//#	else
//		#pragma comment(lib,"live555-multikast-release.lib")
//#	endif
#	pragma comment(lib,"SDL2_x64.lib")
#	pragma comment(lib,"SDL2main_x64.lib")
#	pragma comment(lib,"SDL2_net_x64.lib")
#endif
#define mythcmp(A) strcmp(input,A) == 0
#define streamserverport 5834

//#define MYTH_CONFIG_SENDMESSAGE_SLOW
#define MYTH_CONFIG_SENDMESSAGE_FAST
#define MYTH_INFORMATIONINI_FILE "mythconfig.ini"
#define MYTH_FORCE_AUTOSTART 0			//force start all the camera?

#define MYTH_SERV_RECV_BUFFLEN 4096		//len to recv the buffer when recving data from mythvirtualserver
//#define MYTH_STREAM_CLOSE
//#define MYTH_USING_REDIS 1	//force use redis
#define AVBUFFERSIZE 5 //M
#define AVFRAMECOUNT 25
#define RTSPLINK "rtsp://%s:%s%s"
#define FINDCAMERA "select a.ip,a.username,a.password,a.httpport,b.FullSize,a.vstypeid,c.port,c.subname from videoserver as a,vstype as b,camera as c where a.vstypeid = b.vstypeid and a.videoserverid = c.videoserverid and c.cameraid = %d"
#define firstrequest "HTTP/1.1 200 OK\r\nServer: WWeb/2.0\r\nConnection: Close\r\nContent-Type: multipart/x-mixed-replace;boundary=--myboundary\r\n\r\n \n\n--myboundary\n"

#define STREAMSERVERMAX 100
#ifdef ANDROID
#include <android/log.h>
#define LOG_TAG "org.app.sdl"

#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#else
#define LOGE printf
#endif

//magic is a fucking rubbish recently,
typedef struct PacketQueue {
	unsigned char* h264Packet;
	unsigned int h264PacketLength;
	unsigned char* yuvPacket[3];
	unsigned int yuvPacketLength[3];
	unsigned int stream_type;
	unsigned int tickcount;
	void* magic;
	bool isIframe;
} PacketQueue;

//char* global_filename = NULL;
