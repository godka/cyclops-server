#include "mythIniFile.hh"


mythIniFile*  mythIniFile::mmythIniFile = NULL;

int mythIniFile::GetInt(const char* section, const char* name,int defaultret)
{
	int ret = defaultret;
	_mutex.lock();
	ret = read_profile_int(section, name, 0, MYTH_INFORMATIONINI_FILE);
	_mutex.unlock();
	return ret;
}

std::string mythIniFile::GetStr(const char* section, const char* name,std::string defaultret)
{
	char ret[256] = { 0 };
	_mutex.lock();
	read_profile_string("config", "ip", ret, 256, defaultret.c_str(), MYTH_INFORMATIONINI_FILE);
	_mutex.unlock();
	return ret;
}

mythIniFile::mythIniFile()
{
	mmythIniFile = this;
}


mythIniFile::~mythIniFile()
{
}
