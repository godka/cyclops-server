#include "mythMediaPipeline.hh"

mythMediaPipeline::mythMediaPipeline(void* ptr,int streamtype, int level)
	:mythAvlist()
{
	_ptr = ptr;
	_streamtype = streamtype;
	_level = level;
	m_decoder = mythFFmpeg::CreateNew(this);
	m_encoder = NULL;
	yuvlist = mythAvlist::CreateNew(50); 
	isrunning = true;
	_thread = SDL_CreateThread(encodethreadstatic, "encodethread", this);
	
}
int mythMediaPipeline::encodethreadstatic(void* data){
	if (data){
		mythMediaPipeline* tmp = (mythMediaPipeline*) data;
		return tmp->encodethread();
	}
	return 0;
}

int mythMediaPipeline::encodethread()
{
	while (isrunning){
		PacketQueue* pkt = yuvlist->get();
		if (pkt){
			m_encoder->ProcessFrame((unsigned char**) pkt->yuvPacket, (int*)pkt->yuvPacketLength, encodecallback_static);
		}
		SDL_Delay(1);
	}
	return 0;
}

void mythMediaPipeline::decodecallback(char** pdata, int* plength, int width, int height)
{
	if (width > 0 && height > 0 && m_encoder == NULL){
		m_encoder = mythFFmpegEncoder::CreateNew(this, _streamtype, _level, width, height);
	}
	yuvlist->put((unsigned char**) pdata, (unsigned int*) plength,width,height);
	//m_encoder->ProcessFrame((unsigned char**) pdata, plength, encodecallback_static);
}

void mythMediaPipeline::encodecallback(char* pdata, int plength)
{
	put((unsigned char*)pdata, plength);
	printf("out:%d\n", plength);
}

mythMediaPipeline::~mythMediaPipeline()
{
	isrunning = false;
	if (_thread)
		SDL_WaitThread(_thread, NULL);
	_thread = NULL;
}

void mythMediaPipeline::PutMedia(void* data, int len)
{
	if (m_decoder){
		m_decoder->ProcessFrame((unsigned char*) data, len, decodecallback_static); 
		printf("in:%d\n", len);
	}
}
