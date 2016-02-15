#include "mythRedisList.hh"

mythRedisList::mythRedisList()
	:mythVirtualList()
{
	listread = 0;
	listwrite = 0;
	context = redisConnect("127.0.0.1", 6379);
	if (context->err) {
		printf("SET:Redis:Connection error: %s\n", context->errstr);
	}
}


mythRedisList::~mythRedisList()
{
}

PacketQueue * mythRedisList::get(int freePacket /*= 0*/)
{
	PacketQueue *tmp = NULL;// (PacketQueue*) SDL_malloc(sizeof(PacketQueue));
	if (context){
		if (this->listwrite - this->listread == 1 ||
			this->listwrite - this->listread == 0 ||
			(this->listwrite == 0 && this->listread == AVFRAMECOUNT)){
			tmp = NULL;
		}
		else{
			redisReply* _reply = (redisReply*) redisCommand(context, "GET %d:%d", (int) magic, listread);
			tmp->h264Packet = NULL;
			if (_reply->len> 0){
				tmp = (PacketQueue*) SDL_malloc(sizeof(PacketQueue));
				tmp->h264Packet = (unsigned char*) _reply->str;
				tmp->h264PacketLength = _reply->len;
			}
			if (tmp->h264Packet == NULL){
				tmp = NULL;
			}
			else{
				if (freePacket == 0){
					if (listwrite - listread > 10){
						LOGE("skip frames");
						LOGE(" read = %d,write = %d,minus = %d\n", listread, listwrite, listwrite - listread);
						listread += 9;
					}
					else
						listread++;
				}
			}
		}
		listread %= AVFRAMECOUNT;
	}
	return tmp;
}

int mythRedisList::put(unsigned char* data, unsigned int length)
{

	if (context){
		redisReply* _reply = (redisReply*) redisCommand(context, "SET %d:%d %b", (int) magic, listread,data,length);
		listwrite++;
		listwrite %= AVFRAMECOUNT;
	}
	return 0;
}

int mythRedisList::release(PacketQueue *pack)
{
	if (pack){
		//if (pack->h264Packet){
		//	SDL_free(pack->h264Packet);
		//	pack->h264Packet = 0;
		//}
		SDL_free(pack);
	}
	return 0;
}