#include "mythStreamServer.hh"
#include <string>
#include "mythVirtualSqlite.hh"
//#include <omp.h>
mythStreamServer* mythStreamServer::CreateNew(int cameraid){
	return new mythStreamServer(cameraid);
}

mythStreamServer::mythStreamServer(int cameraid)
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
	SDL_snprintf(sqltmp,4096,FINDCAMERA,m_cameraid);
	mythStreamSQLresult* result = mythVirtualSqlite::GetInstance()->doSQLFromStream(sqltmp);
	if (result){
		while (result->MoveNext()){
			if (!decoder){
				username = result->prase("username");
				password = result->prase("password");
				httpport = result->prase("SubName");
				FullSize = result->prase("FullSize");
				vstypeid = result->prase("vstypeid");
				ip = result->prase("ip");
				realcameraid = result->prase("Port");
				switch (atoi(vstypeid.c_str()))
				{
				case 88:
					//ziyadecoder
					this->decoder = mythZiyaDecoder::CreateNew((char*)ip.c_str(), atoi(realcameraid.c_str()));
					break;
#ifdef CAMERADECODER
				case 15:
					this->decoder = mythCameraDecoder::CreateNew();
					break;
#endif
				default:
					//live555decoder
					char url[256] = { 0 };
					sprintf(url, RTSPLINK, ip.c_str(), httpport.c_str(), FullSize.c_str());
					string strRecordUrl = url;
					int iFind = strRecordUrl.find("$camera");
					if (iFind >= 0)
						strRecordUrl.replace(iFind, iFind + strlen("$camera"), realcameraid);
					strcpy(url, strRecordUrl.c_str());
					this->decoder = mythLive555Decoder::CreateNew(url, (char*) username.c_str(), (char*) password.c_str());
					break;
				}
				//SDL_LockMutex(decodemutex);
				if (decoder)
					decoder->start();
				//SDL_UnlockMutex(decodemutex);
			}
		}
	}
	if (result)
		delete result;
	
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
	if (!FindClient(baselist.begin(), baselist.end(), client)){
		baselist.push_back(client);
	}
	return 0;
}
int mythStreamServer::mainthread()
{
	PacketQueue* tmp = NULL;
	connect();
	while(isrunning == 0){
		if (decoder){
			if (baselist.size() <= 0){
				SDL_Delay(100);
			}
			else{
				tmp = decoder->get();
				if (tmp != NULL){
					//add omp version
//#pragma omp parallel for
					for (int i = 0; i < baselist.size(); i++){
						mythBaseClient* tmpclient = baselist.at(i);
						if (tmpclient != NULL){
							tmpclient->DataCallBack(tmp->h264Packet, tmp->h264PacketLength);
						}
					}
					//baselist.push_back(this);
				}
				else{
					SDL_PollEvent(NULL);
					SDL_Delay(1);
				}
			}
		}
	}
	if (decoder)
		decoder->stop();
	delete decoder;
	//printf("stream server delete success! - %d\n",this->m_cameraid);
	return 0;
}

int mythStreamServer::start()
{
	isrunning = 0;
	if (!streamserverthread)
		streamserverthread = SDL_CreateThread(mainthreadstatic,"static",this);
	return 0;
}

int mythStreamServer::stop()
{
	isrunning = 1;
	if (streamserverthread)
		SDL_WaitThread(this->streamserverthread,NULL);
	streamserverthread = NULL;
	//printf("server stop finished\n");
	return 0;
}

int mythStreamServer::getClientNumber()
{
	return baselist.size();
}

int mythStreamServer::DropClient(mythBaseClient* client)
{
	//if (FindClient(baselist.begin(), baselist.end(), client)){
	//	baselist.erase(client);
	//}
	for (vector<mythBaseClient*>::iterator iter = baselist.begin(); iter != baselist.end();iter++)
	{
		if (*iter == client){
			iter = baselist.erase(iter);
			break;
		}
	}
	return 0;
}

/*
char* mythStreamServer::getTop( int* length )
{
	if (decoder){
		*length = toplength;
		return this->topchar;
	}else
		return NULL;
}
int mythStreamServer::getClientNumber()
{

	if (decoder){
		int ret = 0;
		SDL_LockMutex(numbermutex);
		ret = ClientNumber;
		SDL_UnlockMutex(numbermutex);
		return ret;
	}else
		return 1;
}

int mythStreamServer::setClientNumber( int num )
{

	if (decoder){
		int ret = 0;
		SDL_LockMutex(numbermutex);
		ClientNumber = num;
		SDL_UnlockMutex(numbermutex);
		return ret;
	}
	else
	{
		return 1;
	}
}

int mythStreamServer::addClientNumber()
{

	SDL_LockMutex(numbermutex);
	ClientNumber++;
	SDL_UnlockMutex(numbermutex);
	return 0;
}

int mythStreamServer::minClientNumber()
{

	SDL_LockMutex(numbermutex);
	ClientNumber--;
	SDL_UnlockMutex(numbermutex);
	return 0;
}
*/