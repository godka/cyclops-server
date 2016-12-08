#include "mythLog.hh"
#include <stdio.h>
#include <stdarg.h>
#include <chrono>
#include <fstream>
mythLog*  mythLog::mmythLog = nullptr;

void mythLog::printf(const char * format, ...)
{
	_mutex.lock();
	static char str[256] = { 0 };
	va_list argptr;

	va_start(argptr, format);
	int len = vsnprintf(str, 256, format, argptr);
	va_end(argptr);
	static char timefile[64] = { 0 };
	auto t = std::chrono::system_clock::now();
	auto tt = std::chrono::system_clock::to_time_t(t);
	struct tm * timeinfo = localtime(&tt);

	sprintf(timefile, "%04d-%02d-%02d.log",
		1900 + timeinfo->tm_year, 1 + timeinfo->tm_mon, timeinfo->tm_mday);


	FILE* file = fopen(timefile, "a+");
	if (len > 0){
		if (_writedate){
			static char timenow[64] = { 0 };
			sprintf(timenow, "%04d-%02d-%02d %02d:%02d:%02d",
				1900 + timeinfo->tm_year, 1 + timeinfo->tm_mon, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

			fprintf(stderr, "[%s]%s", timenow, str);
			fprintf(file, "[%s]%s", timenow, str);
		}
		else{
			fprintf(stderr, "%s", str);
			fprintf(file, "%s", str);
		}
		if (str[len - 1] != '\n'){
			_writedate = false;
		}
		else{
			_writedate = true;
		}
	}
	fclose(file);
	_mutex.unlock();

}

mythLog::mythLog()
{
	mmythLog = this;
	_writedate = true;
}


mythLog::~mythLog()
{
}
