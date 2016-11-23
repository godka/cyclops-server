#pragma once
#include "MythConfig.hh"
#include "mythVirtualDecoder.hh"
#include "mythVirtualDecoder.hh"
#include "mythLive555Decoder.hh"
#include "mythRedisDecoder.hh"
#include "mythProxyDecoder.hh"
#include "mythStreamDecoder.hh"
#include "mythVirtualSqlite.hh"
#include <string>
class mythRedisClient
{
public:
	static mythRedisClient* CreateNew(int cameraid){
		return new mythRedisClient(cameraid);
	}
	~mythRedisClient();
	void start();
	void stop();
private:
	int m_cameraid;
	mythVirtualDecoder* decoder;

	std::string username;
	std::string password;
	std::string httpport;
	std::string FullSize;
	std::string vstypeid;
	std::string ip;
	std::string realcameraid;

protected:
	mythRedisClient(int cameraid);
};

