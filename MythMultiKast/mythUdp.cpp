#include "mythUdp.hh"
#include <string.h>

mythUdp*  mythUdp::mmythudp;
mythUdp::mythUdp(int localport, int remoteport)
{
	m_remoteport = remoteport;
	m_localport = localport;
	init();
	this->mmythudp = this;
}


mythUdp::~mythUdp()
{
}

void mythUdp::init()
{
	char tmpip[256] = { 0 };
	IPaddress ip;
	SDL_Init(NULL);
	SDLNet_Init();
	read_profile_string("config", "debugip", tmpip, 20, "127.0.0.1", MYTH_INFORMATIONINI_FILE);
	SDLNet_ResolveHost(&ip, tmpip, m_remoteport);
	m_client = SDLNet_UDP_Open(m_localport + 1);
	int ret = SDLNet_UDP_Bind(m_client, 1, &ip);
	packet = SDLNet_AllocPacket(1024 * 10);
}

int mythUdp::SendData(const char* buf, int len)
{
	if (len == -2){
		len = strlen(buf);
	}
	int tlen = len;
	if (len > packet->maxlen){
		tlen = packet->maxlen;
	}
	SDL_memcpy(packet->data, buf, tlen);
	packet->len = len;
	int ret = SDLNet_UDP_Send(m_client, 1, packet);
	return ret;
}

int mythUdp::RecvData(const char* buf, int len)
{
	UDPpacket pkt = { 0 };
	if (SDLNet_UDP_Recv(m_client, &pkt) > 0){
		buf = (const char*)pkt.data;
		return pkt.len;
	}
	else{
		return 0;
	}
}
