#include "mythRedisList.hh"

mythRedisList::mythRedisList()
	:mythVirtualList()
{
	listread = 0;
	maxlistread = 0;
	listwrite = 0;
	//file = fopen("in.264", "w");
	
	timeval tv = { 0 };
	tv.tv_sec = 1; tv.tv_usec = 0;
	context = redisConnectWithTimeout("127.0.0.1", 6379, tv);
	if (context->err) {
		printf("SET:Redis:Connection error: %s\n", context->errstr);
	}
	else{
		redisCommand(context, "SET %d:listwrite %d", (int) magic, 0);
	}

}


mythRedisList::~mythRedisList()
{
}

PacketQueue * mythRedisList::get(int freePacket)
{
	memset(&tmp, 0, sizeof(PacketQueue));
	redisReply* _reply = NULL;
	unsigned long tmplistwrite = 0;
	if (context){
		tmplistwrite = readListIndex((int) magic);
		if (tmplistwrite - listread > 10 || tmplistwrite < listread){
			listread = tmplistwrite;
		}
		_reply = (redisReply*) redisCommand(context, "GET %d:%d", (int) magic, listread);
		if (_reply)
		{
			if (_reply->len > 0){
				tmp.h264Packet = (unsigned char*) _reply->str;
				tmp.h264PacketLength = _reply->len;
				tmp.magic = _reply;
				listread++;
			}

		}
	}
	return &tmp;
}

int mythRedisList::put(unsigned char* data, unsigned int length)
{
	redisReply* _reply = NULL;
	if (context){
		_reply = (redisReply*) redisCommand(context, "SET %d:%d %b", (int) magic, listwrite, data, length);
		if (listwrite >= AVFRAMECOUNT){
			_reply = (redisReply*) redisCommand(context, "del %d:%d", (int) magic, listwrite - AVFRAMECOUNT);
		}
		listwrite++;
		_reply = (redisReply*) redisCommand(context, "SET %d:listwrite %d", (int) magic, listwrite);
	}
	return 0;
}

int mythRedisList::release(PacketQueue *pack)
{
	if (pack){
		if (pack->magic){
			freeReplyObject(pack->magic);
			//free(pack->magic);
			//pack->magic = NULL;
		}
	}
	return 0;
}

unsigned long mythRedisList::readListIndex(int cameraid)
{
	unsigned long ret = 0;
	if (context){
		redisReply* _reply = (redisReply*) redisCommand(context, "GET %d:listwrite", (int) cameraid);
		if (_reply){
			if (_reply->str)
				ret = atol(_reply->str);
		}
	}
	return ret;
}
