#include "mythRedisList.hh"

mythRedisList::mythRedisList()
	:mythVirtualList()
{
	listread = 0;
	maxlistread = 0;
	listwrite = 0;
	file = fopen("in.264", "w");
	context = redisConnect("127.0.0.1", 6379);
	if (context->err) {
		printf("SET:Redis:Connection error: %s\n", context->errstr);
	}

	redisCommand(context, "SET %d:listwrite %d", (int) magic, 0);
}


mythRedisList::~mythRedisList()
{
}

PacketQueue * mythRedisList::get(int freePacket /*= 0*/)
{
	memset(&tmp, 0, sizeof(PacketQueue));
	redisReply* _reply = NULL;
	unsigned long tmplistwrite = 0;
	if (context){
		//if (listread % AVFRAMECOUNT == 0){
		tmplistwrite = readListIndex((int) magic);
		if (tmplistwrite - listread > 10 || tmplistwrite < listread){
			listread = tmplistwrite;
		}
		//}
		//if (listread != maxlistread || listread == 0){
		//	maxlistread = listread;
		_reply = (redisReply*) redisCommand(context, "GET %d:%d", (int) magic, listread);
		if (_reply)
		{
			if (_reply->len > 0){
				printf("reply:%d\n", _reply->len);
				tmp.h264Packet = (unsigned char*) _reply->str;
				tmp.h264PacketLength = _reply->len;
				listread++;
			}
		}
	}
	return &tmp;
}

int mythRedisList::put(unsigned char* data, unsigned int length)
{

	fwrite(data, length, 1, file);
	if (context){
		redisCommand(context, "SET %d:%d %b", (int) magic, listwrite, data, length);
		if (listwrite >= AVFRAMECOUNT){
			 redisCommand(context, "del %d:%d", (int) magic, listwrite - AVFRAMECOUNT);
		}
		listwrite++;
		redisCommand(context, "SET %d:listwrite %d", (int) magic, listwrite);
		printf("write:%d\n", length);
	}
	return 0;
}

int mythRedisList::release(PacketQueue *pack)
{
	return 0;
}

unsigned long mythRedisList::readListIndex(int cameraid)
{
	unsigned long ret = 0;
	if (context){
		redisReply* _reply = (redisReply*) redisCommand(context, "GET %d:listwrite", (int) cameraid);
		if (_reply){
			ret = atol(_reply->str);
		}
	}
	return ret;
}
