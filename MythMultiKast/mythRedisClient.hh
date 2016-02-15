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
using namespace std;
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

	string username;
	string password;
	string httpport;
	string FullSize;
	string vstypeid;
	string ip;
	string realcameraid;

protected:
	mythRedisClient(int cameraid);
};

