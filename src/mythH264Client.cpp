#include "mythH264Client.hh"

mythH264Client::mythH264Client(MythSocket* people)
	:mythBaseClient(people)
{

}

int mythH264Client::DataCallBack(PacketQueue* pkt)
{
	auto data = pkt->Packet;
	auto len = pkt->PacketLength;

	if (isfirst){
		if (mythSendMessage(firstrequest) < 0){
			return -1;
		}
		isfirst = false;
	}
	if (mythSendMessage(data, len) < 0){
		return -1;
	}
	return 0;
}

mythH264Client::~mythH264Client()
{
}
