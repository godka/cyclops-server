#include "mythRedisDecoder.hh"
#include "mythGlobal.hh"
//char* global_filename = NULL;

mythRedisDecoder::mythRedisDecoder(int cameraid)
{
	m_cameraid = cameraid;
	SetMagic((void*)m_cameraid);
}


mythRedisDecoder::~mythRedisDecoder()
{
}

int mythRedisDecoder::MainLoop()
{
	char tmp[512] = { 0 };
#ifdef WIN32
	SDL_snprintf(tmp, 512, "start %s -client %d", mythGlobal::GetInstance()->global_filename, m_cameraid);
#else
	SDL_snprintf(tmp, 512, "%s -client %d", mythGlobal::GetInstance()->global_filename, m_cameraid);
#endif
	puts(tmp);
	return system(tmp);
}

void mythRedisDecoder::stop()
{

}
