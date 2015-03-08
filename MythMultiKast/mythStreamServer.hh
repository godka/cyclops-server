#pragma once
#include "MythConfig.hh"
#include "mythVirtualDecoder.hh"
#include "mythLive555Decoder.hh"
#include "mythVirtualServer.hh"
#include "mythZiyaDecoder.hh"
#include "mythBaseClient.hh"
#include "mythStreamSQLresult.hh"
#define RTSPLINK "rtsp://%s:%s%s"
#define FINDCAMERA "select a.ip,a.username,a.password,a.httpport,b.FullSize,a.vstypeid,c.port,c.subname from videoserver as a,vstype as b,camera as c where a.vstypeid = b.vstypeid and a.videoserverid = c.videoserverid and c.cameraid = %d"
//using namespace std;
#include <vector>
using namespace std;
class mythStreamServer 
	//: public mythVirtualSqlite
{
public:
	//int addClientNumber();
	//int minClientNumber();
	//int setClientNumber(int num);
	int getClientNumber();
	int AppendClient(mythBaseClient* client);
	int DropClient(mythBaseClient* client);
	//char* getTop( int* length );
	static int mainthreadstatic(void* data);
	int mainthread();
	static mythStreamServer* CreateNew(int cameraid);
	~mythStreamServer(void);
	/*
	void AddClient(PEOPLE* people);
	void DelClient(PEOPLE* people);
	int GetClientNumber();
	void ClearAllClients();
	*/
	int start();
	int stop();
	int m_cameraid;
private:
	vector<mythBaseClient*> baselist;
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
	mythStreamServer(int cameraid);
	bool FindClient(vector <mythBaseClient*>::iterator beg, vector <mythBaseClient*>::iterator end, mythBaseClient* ival);
	SDL_mutex* numbermutex;
	SDL_mutex* streamservermutex;
	SDL_Thread* streamserverthread;
	SDL_mutex* decodemutex;
	int isrunning;
	char* topchar;
	int toplength;
};

