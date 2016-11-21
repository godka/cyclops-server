#include "MythConfig.hh"
#ifdef USEPIPELINE
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
	_thread = new std::thread(&mythMediaPipeline::encodethread, this);
	
}

int mythMediaPipeline::encodethread()
{
	while (isrunning){
		PacketQueue* pkt = yuvlist->get();
		if (pkt){
			m_encoder->ProcessFrame((unsigned char**) pkt->yuvPacket, (int*) pkt->yuvPacketLength, [](void* ptr, char* pdata, int plength){
				if (ptr){
					mythMediaPipeline* tmp = (mythMediaPipeline*) ptr;
					tmp->encodecallback(pdata, plength);
				}
			});
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
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
	//printf("out:%d\n", plength);
}

mythMediaPipeline::~mythMediaPipeline()
{
	isrunning = false;
	if (_thread)
		_thread->join();
	_thread = nullptr;
}

void mythMediaPipeline::PutMedia(void* data, int len)
{
	if (m_decoder){
		m_decoder->ProcessFrame((unsigned char*) data, len, [](void *ptr, char** pdata, int* plength, int width, int height){
			if (ptr){
				mythMediaPipeline* tmp = (mythMediaPipeline*) ptr;
				tmp->decodecallback(pdata, plength, width, height);
			}
		});
		//printf("in:%d\n", len);
	}
}
#endif
