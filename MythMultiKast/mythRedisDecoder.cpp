#include "mythRedisDecoder.hh"
char* global_filename = NULL;

mythRedisDecoder::mythRedisDecoder(int cameraid)
{
	m_cameraid = cameraid;
	SetMagic((void*)m_cameraid);
}


mythRedisDecoder::~mythRedisDecoder()
{
}

void mythRedisDecoder::start()
{
	char tmp[512] = { 0 };
	SDL_snprintf(tmp, 512, "%s -client %d", global_filename, m_cameraid);
	puts(tmp);
	system(tmp);
}

void mythRedisDecoder::stop()
{

}
