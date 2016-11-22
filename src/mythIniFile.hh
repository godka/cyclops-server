#pragma once
#include "MythConfig.hh"
#include "IniFile.h"
#include <string>
#include <mutex>
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
	std::string GetStr(const char* section, const char* name, std::string defaultret = "");
protected:
	mythIniFile();
	~mythIniFile();
private:
	std::mutex _mutex; 
};

