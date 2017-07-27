#pragma once
#include <mutex>
class mythLog
{
public:
	static mythLog* GetInstance(){
		if (!mmythLog)
			return new mythLog();
		else
			return mmythLog;
	}
	static mythLog* mmythLog;
	void printf(const char * _Format, ...);
	mythLog();
	~mythLog();
private:
	std::mutex _mutex;
	bool _writedate;
};

