#include "mythProxyDecoder.hh"


int mythProxyDecoder::decode_thread()
{
#define BUFF_COUNT 1024*1024	
	char* buf = new char[BUFF_COUNT];
	while (flag == 0){
		SDL_LockMutex(mmutex);
		if (msocket){
			int rc = msocket->socket_ReceiveDataLn2(buf, BUFF_COUNT, "Content_Length: ");
			if (rc > 0) {
				printf("%d\n", rc);
				m_count += rc;
				put((unsigned char*) buf, rc);
			}
			else{
				msocket = NULL;
			}
		}
		SDL_UnlockMutex(mmutex);
		SDL_PollEvent(NULL);
		SDL_Delay(1);
	}
	if (msocket){
		msocket->socket_CloseSocket();
		delete [] buf;
		this->free();
		delete msocket;
		msocket = NULL;
	}
	return 0;
}

mythProxyDecoder::mythProxyDecoder(PEOPLE* people)
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

int mythProxyDecoder::refreshSocket(PEOPLE* people)
{
	SDL_LockMutex(mmutex);
	if (!msocket)
		msocket = people;
	SDL_UnlockMutex(mmutex);
	return 0;
}

void mythProxyDecoder::start()
{
	this->mthread = SDL_CreateThread(decode_thread_static, "decode_thread_static", this);
}

void mythProxyDecoder::stop()
{
	flag = 1;
	if (mthread)
		SDL_WaitThread(mthread, NULL);
	return;
}
