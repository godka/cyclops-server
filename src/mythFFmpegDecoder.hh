#pragma once
#include "MythConfig.hh"
#ifdef USEPIPELINE
#include "mythVirtualDecoder.hh"
#include <string>
class mythFFmpegDecoder :
	public mythVirtualDecoder
{
public:
	static mythFFmpegDecoder* CreateNew(const char* filename){
		return new mythFFmpegDecoder(filename);
	}
	int MainLoop();
	~mythFFmpegDecoder();
protected:
	mythFFmpegDecoder(const char* filename);
private:
	std::string _filename;
};
#endif
