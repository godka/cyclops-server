#include "PEOPLE.hh"


PEOPLE::PEOPLE()
{
	maxlength = 512;
}

int PEOPLE::peopleSendMessage(char* data, int length){
#ifdef MYTH_CONFIG_SENDMESSAGE_SLOW
	int tlen = length;
	if (sock){
		while (tlen > 0){
			int len = SDLNet_TCP_Send(this->sock, data, maxlength);
			tlen -= len;
		}
	}
	return 0;
#else
	if (sock)
		return SDLNet_TCP_Send(this->sock, data, length);
	else
		return 0;
#endif
//	return 0;
}

PEOPLE::~PEOPLE()
{
}
