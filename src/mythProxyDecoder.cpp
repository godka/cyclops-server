#include "mythProxyDecoder.hh"


int mythProxyDecoder::MainLoop()
{
#define BUFF_COUNT 1024*1024	
	char* buf = new char[BUFF_COUNT];
	while (flag == 0){
		mmutex.lock();
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
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		mmutex.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	/*
	if (msocket){
		//msocket->socket_CloseSocket();
		delete [] buf;
		//delete msocket;
		msocket = NULL;
	}
	*/
	return 0;
}

mythProxyDecoder::mythProxyDecoder(MythSocket* people)
{
	msocket = people;
	flag = 0;
}

mythProxyDecoder::~mythProxyDecoder()
{
}

int mythProxyDecoder::refreshSocket(MythSocket* people)
{
	mmutex.lock();
	if (!msocket)
		msocket = people;
	mmutex.unlock();
	return 0;
}
void mythProxyDecoder::stop()
{
	flag = 1;
	StopThread();
	return;
}
