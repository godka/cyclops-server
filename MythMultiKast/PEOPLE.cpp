#include "PEOPLE.hh"


PEOPLE::PEOPLE()
{
}

int PEOPLE::peopleSendMessage(char* data, int length){
	if (sock)
		return SDLNet_TCP_Send(this->sock, data, length);
	else
		return NULL;
//	return 0;
}

PEOPLE::~PEOPLE()
{
}
