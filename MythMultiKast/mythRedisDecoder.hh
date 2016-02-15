#pragma once
#include "mythVirtualDecoder.hh"
class mythRedisDecoder :
	public mythVirtualDecoder
{
public:
	static mythRedisDecoder* CreateNew(int cameraid){
		return new mythRedisDecoder(cameraid);
	}
	void start();
	void stop();
	~mythRedisDecoder();
protected:
	mythRedisDecoder(int cameraid);
	int m_cameraid;
};

