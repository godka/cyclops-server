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
#ifdef USEENCODER
	m_encoder = NULL;
#endif
	yuvlist = mythAvlist::CreateNew(50); 
	isrunning = true;
	_thread = new std::thread(&mythMediaPipeline::encodethread, this);
	
}

int mythMediaPipeline::encodethread()
{
	static unsigned int count = 0;
	while (isrunning){
		PacketQueue* pkt = yuvlist->get();
		if (pkt){
			count++;
			if (count % 12 == 0){
				SingleStep(pkt);
				count == 0;
			}
			/*
			m_encoder->ProcessFrame((unsigned char**) pkt->yuvPacket, (int*) pkt->yuvPacketLength, [](void* ptr, char* pdata, int plength){
				if (ptr){
					mythMediaPipeline* tmp = (mythMediaPipeline*) ptr;
					tmp->encodecallback(pdata, plength);
				}
			});
			*/
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	return 0;
}

void mythMediaPipeline::decodecallback(char** pdata, int* plength, int width, int height,char* oridata,int orilen)
{
	//if (width > 0 && height > 0 && m_encoder == NULL){
	//	m_encoder = mythFFmpegEncoder::CreateNew(this, _streamtype, _level, width, height);
	//}
	yuvlist->put((unsigned char**) pdata, (unsigned int*) plength, width, height, ~0, oridata, orilen);
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
		m_decoder->ProcessFrame((unsigned char*) data, len, [](void *ptr, char** pdata, int* plength, int width, int height,char* oridata,int orilen){
			if (ptr){
				mythMediaPipeline* tmp = (mythMediaPipeline*) ptr;
				tmp->decodecallback(pdata, plength, width, height,oridata, orilen);
			}
		});
		//printf("in:%d\n", len);
	}
	WriteFrame((char*)data, len);
	put((unsigned char*) data, len);
}
#endif
