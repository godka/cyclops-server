#pragma once
#if (defined _WIN32) || (defined WIN32)
#	define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdio.h>
#include "mythIniFile.hh"
#include "mythLog.hh"
#define USELIBEVENT 1
//#define USEPIPELINE 1
//#define USEENCODER 1
//#define USETHREAD 1
#if (defined _WIN32) || (defined WIN32)
#	
#	pragma comment(lib,"ws2_32")
#	ifdef USEPIPELINE
#		pragma comment(lib,"../winlib/avcodec.lib")
#		pragma comment(lib,"../winlib/avdevice.lib")
#		pragma comment(lib,"../winlib/avfilter.lib")
#		pragma comment(lib,"../winlib/avformat.lib")
#		pragma comment(lib,"../winlib/avutil.lib")
#		pragma comment(lib,"../winlib/postproc.lib")
#		pragma comment(lib,"../winlib/swscale.lib")
#		pragma comment(lib,"../winlib/swresample.lib")
#	endif
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
#define flvfirstrequest "HTTP/1.1 200 OK\r\nServer: MythMultiKast\r\nConnection: Keep-Alive\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: video/x-flv\r\n\r\n"
#define STREAMSERVERMAX 100
#ifdef ANDROID
#include <android/log.h>
#define LOG_TAG "org.app.sdl"

#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#else
#define LOGE printf
#endif

#include "List.h"
//magic is a fucking rubbish recently,
typedef struct PacketQueue {
	unsigned char* Packet;
	unsigned int PacketLength;
	unsigned char* yuvPacket[3];
	unsigned int yuvPacketLength[3];
	unsigned int stream_type;
	unsigned int TimeStamp;
	unsigned int PacketCount;
	unsigned int width;
	unsigned int height;
	void* magic;
	bool isIframe;
	struct list_head list;
} PacketQueue;

//char* global_filename = NULL;
