#include "mythIniFile.hh"


mythIniFile*  mythIniFile::mmythIniFile = NULL;

int mythIniFile::GetInt(const char* section, const char* name,int defaultret)
{
	int ret = defaultret;
	SDL_LockMutex(_mutex);
	ret = read_profile_int(section, name, 0, MYTH_INFORMATIONINI_FILE);
	SDL_UnlockMutex(_mutex);
	return ret;
}

std::string mythIniFile::GetStr(const char* section, const char* name,string defaultret)
{
	char ret[256] = { 0 };
	SDL_LockMutex(_mutex);
	read_profile_string("config", "ip", ret, 256, defaultret.c_str(), MYTH_INFORMATIONINI_FILE);
	SDL_UnlockMutex(_mutex);
	return ret;
}

mythIniFile::mythIniFile()
{
	mmythIniFile = this;
	_mutex = SDL_CreateMutex();
}


mythIniFile::~mythIniFile()
{
}
