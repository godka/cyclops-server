/********************************************************************
Created by MythKAst
©2013 MythKAst Some rights reserved.


You can build it with vc2010,gcc.
Anybody who gets this source code is able to modify or rebuild it anyway,
but please keep this section when you want to spread a new version.
It's strongly not recommended to change the original copyright. Adding new version
information, however, is Allowed. Thanks.
For the latest version, please be sure to check my website:
Http://code.google.com/mythkast


你可以用vc2010,gcc编译这些代码
任何得到此代码的人都可以修改或者重新编译这段代码，但是请保留这段文字。
请不要修改原始版权，但是可以添加新的版本信息。
最新版本请留意：Http://code.google.com/mythkast
B
MythKAst(asdic182@sina.com), in 2013 June.
*********************************************************************/
#include "mythStreamServer.hh"
#include <string>
#include "mythVirtualSqlite.hh"
//#include <omp.h>
mythStreamServer* mythStreamServer::CreateNew(int cameraid,void* args){
	return new mythStreamServer(cameraid,args);
}

mythStreamServer::mythStreamServer(int cameraid, void* args)
	//:mythVirtualSqlite()
{
	streamserverthread = NULL;
	decoder = NULL;
	ClientNumber = 0;
	m_cameraid = cameraid;
	PeopleAdd = 0;
	numbermutex = SDL_CreateMutex();
	streamservermutex = SDL_CreateMutex();
	decodemutex = SDL_CreateMutex();
	baselist.reserve(100);
	additionalargs = args;
	//open("myth.db");
}


mythStreamServer::~mythStreamServer(void)
{
	//delete decoder;
	SDL_DestroyMutex(numbermutex);
	SDL_DestroyMutex(streamservermutex);
	//delete decoder;
}

void mythStreamServer::connect()
{
	char sqltmp[4096] = {0};
	//add proxy server if cameraid > 10000
	if (m_cameraid >= 10000){
		this->decoder = mythProxyDecoder::CreateNew((MythSocket*) additionalargs);
		if (decoder)
			decoder->start();
	}
	else{
		mythStreamSQLresult* result = NULL;
		int list_type = read_profile_int("config", "list_type", 0, MYTH_INFORMATIONINI_FILE);
		switch (list_type)
		{
		case 0:
			SDL_snprintf(sqltmp, 4096, FINDCAMERA, m_cameraid);
			result = mythVirtualSqlite::GetInstance()->doSQLFromStream(sqltmp);
			if (result){
				while (result->MoveNext()){
					if (!decoder){
						username = result->TryPrase("username");
						password = result->TryPrase("password");
						httpport = result->TryPrase("SubName");
						FullSize = result->TryPrase("FullSize");
						vstypeid = result->TryPrase("vstypeid");
						ip = result->TryPrase("ip");
						realcameraid = result->TryPrase("Port");
						switch (atoi(vstypeid.c_str()))
						{
						case 88:
							//ziyadecoder
							this->decoder = mythStreamDecoder::CreateNew((char*) ip.c_str(), atoi(realcameraid.c_str()));
							break;
#ifdef CAMERADECODER
						case 15:
							this->decoder = mythCameraDecoder::CreateNew();
							break;
#endif
						default:
							string strRecordUrl = "rtsp://" + ip + ":" + httpport + FullSize;
							int iFind = strRecordUrl.find("$camera");
							if (iFind >= 0)
								strRecordUrl.replace(iFind, iFind + strlen("$camera"), realcameraid);
							//strcpy(url, strRecordUrl.c_str());
							this->decoder = mythLive555Decoder::CreateNew((char*) strRecordUrl.c_str(), (char*) username.c_str(), (char*) password.c_str());
							break;
						}
						//SDL_LockMutex(decodemutex);
						if (decoder){
							decoder->SetMagic((void*) m_cameraid);	//set magic
							decoder->start();
						}
						//SDL_UnlockMutex(decodemutex);
					}
				}
				delete result;
			}
			//if (result)
			break;
		case 1:
			decoder = mythRedisDecoder::CreateNew(m_cameraid);
			if (decoder)
				decoder->start();
			break;
		default:
			break;
		}
	}
}

int mythStreamServer::mainthreadstatic( void* data )
{
	if(data){
		mythStreamServer* server = (mythStreamServer*)data;
		server->mainthread();
	}
	return 0;
}
bool mythStreamServer::FindClient(vector <mythBaseClient*>::iterator beg,
	vector <mythBaseClient*>::iterator end, mythBaseClient* ival)
{
	while (beg != end)
	{
		if (*beg == ival)
			break;
		else
			++beg;
	}

	if (beg != end)
		return true;
	else
		return false;
}
int mythStreamServer::AppendClient(mythBaseClient* client){
	SDL_LockMutex(streamservermutex);
	if (!FindClient(baselist.begin(), baselist.end(), client)){
		baselist.push_back(client);
	}
	SDL_UnlockMutex(streamservermutex);
	return 0;
}
int mythStreamServer::mainthread()
{
	PacketQueue* tmp = NULL;
	connect();
	while(isrunning == 0){
		SDL_PollEvent(NULL);
		if (decoder){
			if (baselist.size() <= 0){
				SDL_Delay(100);
			}
			else{
				tmp = decoder->get();
				if (tmp){
					//add omp version
					if (tmp->h264PacketLength > 0){
						for (unsigned int i = 0; i < baselist.size(); i++){
							mythBaseClient* tmpclient = baselist.at(i);
							if (tmpclient){
								tmpclient->DataCallBack(tmp->h264Packet, tmp->h264PacketLength);
							}
						}
					}
					decoder->release(tmp);
					SDL_Delay(1);
					//baselist.push_back(this);
				}
				//else{
				SDL_PollEvent(NULL);
				SDL_Delay(1);
				//}
			}
		}
	}
	if (decoder)
		decoder->stop();
	delete decoder;
	return 0;
}

int mythStreamServer::start(bool canthread)
{
	if (!canthread){
		mainthreadstatic(this);
	}
	else{
		isrunning = 0;
		if (!streamserverthread)
			streamserverthread = SDL_CreateThread(mainthreadstatic, "static", this);
	}
	return 0;
}

int mythStreamServer::stop()
{
	isrunning = 1;
	if (streamserverthread)
		SDL_WaitThread(this->streamserverthread,NULL);
	streamserverthread = NULL;
	return 0;
}

int mythStreamServer::getClientNumber()
{
	return baselist.size();
}

int mythStreamServer::DropClient(mythBaseClient* client)
{
	SDL_LockMutex(streamservermutex);
	for (vector<mythBaseClient*>::iterator iter = baselist.begin(); iter != baselist.end();iter++)
	{
		if (*iter == client){
			iter = baselist.erase(iter);
			break;
		}
	}
	SDL_UnlockMutex(streamservermutex);
	return 0;
}