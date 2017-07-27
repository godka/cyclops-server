#pragma once
#include "MythConfig.hh"
#include "mythVirtualDecoder.hh"
#include "mythLive555Decoder.hh"
#include "mythProxyDecoder.hh"
#include "mythStreamDecoder.hh"
#include "mythFFmpegDecoder.hh"
#include "mythBaseClient.hh"
#include "mythH264Decoder.hh"
#include "mythStreamSQLresult.hh"
#include "mythRequestParser.hh"
#include <string>
#include <thread>
#include "cJSON.h"
class mythStreamServer 
{
public:
	int getClientNumber();
	int AppendClient(mythBaseClient* client);
	int DropClient(mythBaseClient* client);
	int DropClient(int SockID);
	static mythStreamServer* CreateNew(int cameraid, void* args = NULL);
	static mythStreamServer* CreateNew(mythRequestParser* parser, int cameraid = -1);
	static mythStreamServer* CreateNew(cJSON* parser, int cameraid = -1);
	//mythStreamServer* CreateNew(int cameraid, void* args);
	~mythStreamServer(void);
	mythVirtualDecoder* GetDecoder(){
		return decoder;
	}
	std::string& ToString(){
		return _parseUrl;
	}
	void SetID(int cameraid){
		m_cameraid = cameraid;
		mythLog::GetInstance()->printf("[SetID][%s]Setting CameraID:%d\n", ToString().c_str(), m_cameraid);
	}
	int GetID(){
		return m_cameraid;
	}
	int start(bool canthread = true);
	int stop();
private:
	int mainthread();
	long long mythTickCount();
	bool CheckTime(long long &foo, int timeout);
	int m_cameraid;
	mythBaseClient** _baselist;
	void connect();
	void connectViaUrl(mythRequestParser* parser);
	void connectViaUrl(cJSON* parser);
	char* parseUrlHeader(const char* url);
	void SetStart(bool foo);
	bool GetStart();
protected:
	mythVirtualDecoder* decoder;
	std::string username;
	std::string password;
	std::string httpport;
	std::string FullSize;
	std::string vstypeid;
	std::string ip;
	std::string realcameraid;
	mythStreamServer(int cameraid, void* args = NULL);
	mythStreamServer(mythRequestParser* parser, int cameraid = -1);
	mythStreamServer(cJSON* parser, int cameraid = -1);
	bool FindClient(mythBaseClient* ival);
	std::thread* streamserverthread;
	int isrunning;
	void* additionalargs;
	bool _hasstart;
	std::string _parseUrl;
};