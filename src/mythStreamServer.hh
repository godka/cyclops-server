#pragma once
#include "MythConfig.hh"
#include "mythVirtualDecoder.hh"
#include "mythLive555Decoder.hh"
#include "mythProxyDecoder.hh"
#include "mythStreamDecoder.hh"
#include "mythBaseClient.hh"
#include "mythStreamSQLresult.hh"
#include <thread>
class mythStreamServer 
{
public:
	typedef void (OnCloseHandler)(mythStreamServer * streamserver, int duration, void* data);
	void SetOnCloseHandler(OnCloseHandler* handler,void* data);
	int getClientNumber();
	int AppendClient(mythBaseClient* client);
	int DropClient(mythBaseClient* client);
	//char* getTop( int* length );
	int mainthread();
	static mythStreamServer* CreateNew(int cameraid,void* args = NULL);
	//mythStreamServer* CreateNew(int cameraid, void* args);
	~mythStreamServer(void);
	mythVirtualDecoder* GetDecoder(){
		return decoder;
	}
	int GetID(){
		return m_cameraid;
	}
	int start(bool canthread = true);
	int stop();
private:
	long long mythTickCount();
	bool CheckTime(long long &foo, int timeout);
	int m_cameraid;
	mythBaseClient* _baselist[STREAMSERVERMAX];
	int ClientNumber;
	void connect();
	void SetStart(bool foo);
	bool GetStart();
protected:
	mythVirtualDecoder* decoder;
	string username;
	string password;
	string httpport;
	string FullSize;
	string vstypeid;
	string ip;
	string realcameraid;
	int PeopleAdd;
	mythStreamServer(int cameraid, void* args = NULL);
	bool FindClient(mythBaseClient* ival);
	std::thread* streamserverthread;
	int isrunning;

	OnCloseHandler* _handler;
	void* _handlerdata;
	void* additionalargs;
	bool _hasstart;
};

