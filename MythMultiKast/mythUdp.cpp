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
	IPaddress ip;
	SDL_Init(NULL);
	SDLNet_Init();
	SDLNet_ResolveHost(&ip, "127.0.0.1", m_remoteport);
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
