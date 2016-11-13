/********************************************************************
Created by MythKAst
©2013 MythKAst Some rights reserved.


You can build it with vc2010,gcc.
Anybody who gets this source code is able to modify or rebuild it anyway,
but please keep this section when you want to spread a new version.
It's strongly not recommended to change the original copyright. Adding new version
information, however, is Allowed. Thanks.
For the latest version, please be sure to check my website:
Http://code.google.com/mythkast


你可以用vc2010,gcc编译这些代码
任何得到此代码的人都可以修改或者重新编译这段代码，但是请保留这段文字。
请不要修改原始版权，但是可以添加新的版本信息。
最新版本请留意：Http://code.google.com/mythkast
B
MythKAst(asdic182@sina.com), in 2013 June.
*********************************************************************/
#include "mythBaseClient.hh"

mythBaseClient::mythBaseClient(MythSocket* people, int usethread, const char* CameraType)
{
	//musethread = usethread;
	mpeople = people;
	mainthreadhandle = NULL;
	isrunning = 0;
	isfirst = true;
	iFrameCount = 4096;
	mymutex = SDL_CreateMutex();
	misrunning = true;
	sendBuffer = (char*) SDL_malloc(3 * 1024 * 1024);
	sendbufferptr = 0;
	if (CameraType){
		if (SDL_strcmp(CameraType, "zyh264") == 0){
			m_cameratype = 0;	//stream
		}
		else{ 
			m_cameratype = 1;//HTTP
		}
	}
	if (usethread == 1){
		mthread = SDL_CreateThread(SendThreadStatic, "sendthread", this);
		_mode = 1;
	}
	else{
		mthread = NULL;
		_mode = 0;
	}
}
mythBaseClient::~mythBaseClient(void)
{
	SDL_DestroyMutex(mymutex);
	//if (mode == 1){
	misrunning = false;
	if (mthread)
		SDL_WaitThread(mthread, 0);
	mthread = NULL;
	if (sendBuffer)
		SDL_free(sendBuffer);
	//}
}

void mythBaseClient::ChangeMode(int mode)
{
	if (mode == _mode)
		return;
	if (mode == 0){
		if (mthread){
			misrunning = false; 
			if (mthread)
				SDL_WaitThread(mthread, 0);
			mthread = NULL;
		}
	}
	else if (mode == 1){
		mthread = SDL_CreateThread(SendThreadStatic, "sendthread", this);
	}
	_mode = mode;
}

int mythBaseClient::DataCallBack(void* data, int len)
{
	char tempbuf[256] = { 0 };

	if (isfirst == true){
		AddBuffer((void*) firstrequest);
		isfirst = false;
	}
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	SDL_snprintf(tempbuf, 256,
		"Content-Type: image/h264\r\nContent_Length: %06d Stamp:%04x%02x%02x %04x%02x%02x %02d %08x\n\n", len,
		1900 + timeinfo->tm_year, 1 + timeinfo->tm_mon, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,
		0, iFrameCount);
	if (m_cameratype == 0)
		AddBuffer(tempbuf);
	iFrameCount++;
	AddBuffer(data, len);
	if (m_cameratype == 0)
		AddBuffer((void*)" \n\n--myboundary\n");
	FlushBuffer();
	return 0;
}

mythBaseClient* mythBaseClient::CreateNew(MythSocket* people, int usethread, const char* CameraType)
{
	return new mythBaseClient(people, usethread, CameraType);
}

int mythBaseClient::generate(char* data, int length)
{

	return 0;
}
int mythBaseClient::AddBuffer(void* data, int length){
	if (length == -1)
		length = strlen((char*) data);
	SDL_memcpy(sendBuffer + sendbufferptr, data, length);
	sendbufferptr += length;
	return 0;
}
int mythBaseClient::FlushBuffer(){
	mythSendMessage(sendBuffer, sendbufferptr);
	sendbufferptr = 0;
	return 0;
}
int mythBaseClient::mythSendMessage( void* data,int length )
{
	if (length == -1)
		length = strlen((char*) data);
	if (_mode == 1){
		put((unsigned char*) data, (unsigned int) length);				//use avlist
		return 0;
	}
	else{
		int tmplength = 0;
		if (mpeople){
			tmplength = mpeople->socket_SendStr((char*) data, length);
		}
		return tmplength;
	}
}

int mythBaseClient::SendThread()
{
	while (misrunning){
		SDL_PollEvent(NULL);
		PacketQueue* pkt = get();
		if (pkt){
			if (mpeople)
				mpeople->socket_SendStr((char*) pkt->h264Packet, pkt->h264PacketLength);
		}
		SDL_Delay(1);
	}
	return 0;
}