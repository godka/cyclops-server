#include "mythProxyDecoder.hh"


int mythProxyDecoder::MainLoop()
{
#define BUFF_COUNT 1024*1024	
	char* buf = new char[BUFF_COUNT];
	while (flag == 0){
		SDL_LockMutex(mmutex);
		if (msocket){
			int rc = msocket->socket_ReceiveDataLn2(buf, BUFF_COUNT, "Content_Length: ");
			if (rc > 0) {
				//printf("%d\n", rc);
				m_count += rc;
				put((unsigned char*) buf, rc);
			}
			else{
		//		msocket->socket_CloseSocket();
				msocket = NULL;
			}
		}
		else{
			SDL_Delay(100);
		}
		SDL_UnlockMutex(mmutex);
		SDL_Delay(1);
		SDL_PollEvent(NULL);
	}
	if (msocket){
		//msocket->socket_CloseSocket();
		delete [] buf;
		//delete msocket;
		msocket = NULL;
	}
	return 0;
}

mythProxyDecoder::mythProxyDecoder(MythSocket* people)
{
	msocket = people;
	flag = 0;
	mmutex = SDL_CreateMutex();
}

mythProxyDecoder::~mythProxyDecoder()
{
	if (mmutex){
		SDL_DestroyMutex(mmutex);
	}
}

int mythProxyDecoder::refreshSocket(MythSocket* people)
{
	SDL_LockMutex(mmutex);
	if (!msocket)
		msocket = people;
	SDL_UnlockMutex(mmutex);
	return 0;
}
void mythProxyDecoder::stop()
{
	flag = 1;
	StopThread();
	return;
}
