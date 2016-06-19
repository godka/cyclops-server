#include "mythMediaPipeline.hh"

FILE* file;
int lena = 0;
int lenb = 0;
mythMediaPipeline::mythMediaPipeline(void* ptr,int streamtype, int level)
	:mythAvlist()
{
	_ptr = ptr;
	_streamtype = streamtype;
	_level = level;
	m_decoder = mythFFmpeg::CreateNew(this);
	m_encoder = NULL;
	file = fopen("test.txt", "w");
}

void mythMediaPipeline::decodecallback(char** pdata, int* plength, int width, int height)
{
	if (width > 0 && height > 0 && m_encoder == NULL){
		m_encoder = mythFFmpegEncoder::CreateNew(this, _streamtype, _level, width, height);
	}
	m_encoder->ProcessFrame((unsigned char**) pdata, plength, encodecallback_static);
}

void mythMediaPipeline::encodecallback(char* pdata, int plength)
{
	lenb += plength;
	fprintf(file, "%d,%d\n", lena,lenb);
	put((unsigned char*)pdata, plength);
}

mythMediaPipeline::~mythMediaPipeline()
{
}

void mythMediaPipeline::PutMedia(void* data, int len)
{
	if (m_decoder){
		lena += len;
		//fprintf(file , "%d,", len);
		m_decoder->ProcessFrame((unsigned char*) data, len, decodecallback_static);
	}
}
