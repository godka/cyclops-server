#pragma once
#include "MythConfig.hh"
#include "mythVirtualDecoder.hh"
#include "mythLive555Decoder.hh"
#include "mythRedisDecoder.hh"
#include "mythProxyDecoder.hh"
#include "mythVirtualServer.hh"
#include "mythStreamDecoder.hh"
#include "mythBaseClient.hh"
#include "mythStreamSQLresult.hh"
class mythStreamServer 
{
public:
	int getClientNumber();
	int AppendClient(mythBaseClient* client);
	int DropClient(mythBaseClient* client);
	//char* getTop( int* length );
	static int mainthreadstatic(void* data);
	int mainthread();
	static mythStreamServer* CreateNew(int cameraid,void* args = NULL);
	//mythStreamServer* CreateNew(int cameraid, void* args);
	~mythStreamServer(void);
	mythVirtualDecoder* GetDecoder(){
		return decoder;
	}
	/*
	void AddClient(PEOPLE* people);
	void DelClient(PEOPLE* people);
	int GetClientNumber();
	void ClearAllClients();
	*/
	int start(bool canthread = true);
	int stop();
	int m_cameraid;
private:
	mythBaseClient* _baselist[STREAMSERVERMAX];
	int ClientNumber;
	void connect();
protected:
	mythVirtualDecoder* decoder;
	string username;
	string password;
	string httpport;
	string FullSize;
	string vstypeid;
	string ip;
	string realcameraid;
	//vector<mythBaseClient*> mpeople;
	//PeopleMap mpeople;
	int PeopleAdd;
	mythStreamServer(int cameraid, void* args = NULL);
	//bool FindClient(vector <mythBaseClient*>::iterator beg, vector <mythBaseClient*>::iterator end, mythBaseClient* ival);
	bool FindClient(mythBaseClient* ival);
	SDL_Thread* streamserverthread;
	int isrunning;
	char* topchar;
	int toplength;
	void* additionalargs;
};

