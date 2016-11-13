#pragma once
#include "MythConfig.hh"
#include "IniFile.h"
#include <string>
using namespace std;
class mythIniFile
{
public:
	static mythIniFile* GetInstance(){
		if (!mmythIniFile)
			return new mythIniFile();
		else
			return mmythIniFile;
	}
	static mythIniFile* mmythIniFile;
	int GetInt(const char* section, const char* name, int defaultret = 0);
	string GetStr(const char* section, const char* name, string defaultret = "");
protected:
	mythIniFile();
	~mythIniFile();
private:
	SDL_mutex* _mutex; 
};

