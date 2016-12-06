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
	streamserverthread = nullptr;
	decoder = NULL;
	ClientNumber = 0;
	m_cameraid = cameraid;
	PeopleAdd = 0;
	//baselist.reserve(100);
	additionalargs = args;
	_handler = NULL;
	_handlerdata = NULL;
	//_hasstart = false;
	//open("myth.db");

	SetStart(false);
	memset(_baselist, 0, sizeof(mythBaseClient*) * STREAMSERVERMAX);
}


mythStreamServer::~mythStreamServer(void)
{

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
		int list_type = mythIniFile::GetInstance()->GetInt("config", "list_type"); 
		switch (list_type)
		{
		case 0:
		case 1:
			sprintf(sqltmp, FINDCAMERA, m_cameraid);
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
							std::string strRecordUrl = "rtsp://" + ip + ":" + httpport + FullSize;
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
			else{
				this->decoder = mythStreamDecoder::CreateNew("120.204.70.218", 1017);
				//this->decoder = mythLive555Decoder::CreateNew("rtsp://192.168.31.128:554/tcp/av0_0", "admin", "888888");
				if (decoder){
					decoder->SetMagic((void*) m_cameraid);	//set magic
					decoder->start();
				}
			}
			//if (result)
			break;
		default:
			break;
		}
	}
}

void mythStreamServer::SetStart(bool foo)
{
	_hasstart = foo;
}

bool mythStreamServer::GetStart()
{
	return _hasstart;
}

bool mythStreamServer::FindClient(mythBaseClient* ival){
	for (int i = 0; i < STREAMSERVERMAX; i++){
		if (_baselist[i] == ival)
			return true;
	}
	return false;
}


int mythStreamServer::AppendClient(mythBaseClient* client){
	mythLog::GetInstance()->printf("Appending Client,%u\n", client);
	if (!FindClient(client)){
		for (int i = 0; i < STREAMSERVERMAX; i++){
			if (_baselist[i] == NULL){
				_baselist[i] = client;
				break;
			}
		}
	}
	return 0;
}
long long mythStreamServer::mythTickCount(){
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}
bool mythStreamServer::CheckTime(long long &foo,int timeout){
	if (foo == 0){
		foo = mythTickCount();
		return false;
	}
	auto  _nowtime = mythTickCount();
	auto _duration = _nowtime - foo;
	return (_duration < timeout);
}
int mythStreamServer::mainthread()
{
	PacketQueue* tmp = NULL;
	connect();
	long long closetick = 0;
	long long recvtick = 0;
	while (isrunning == 0){
		if (decoder){
			tmp = decoder->get();
			if (tmp){
				if (tmp->PacketLength > 0){
					auto timestart = mythTickCount();
					int streamcount = 0;
					for (int i = 0; i < STREAMSERVERMAX; i++){
						mythBaseClient* tmpclient = _baselist[i];
						if (tmpclient){
							streamcount++;
							if (tmpclient->DataCallBack(tmp) < 0){
								DropClient(tmpclient);
							}
						}
					}
					if (streamcount == 0){
						//add ticks
						//if (CheckTime(closetick, 5000));
							//break;
					}
					else{
						//refresh ticks
						closetick = mythTickCount();
					}
					if (CheckTime(recvtick, 1000)){
						
					}
				}
				decoder->release(tmp);
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
	if (decoder)
		decoder->stop();
	delete decoder;
	SetStart(false);
	decoder = nullptr;
	streamserverthread = nullptr;
	mythLog::GetInstance()->printf("%d is closed\n", m_cameraid);
	return 0;
}

int mythStreamServer::start(bool canthread)
{
	if (!GetStart()){
		SetStart(true);
		if (!canthread){
			mainthread();
		}
		else{
			isrunning = 0;
			if (!streamserverthread)
				streamserverthread = new std::thread(&mythStreamServer::mainthread, this);
		}
	}
	else{
		mythLog::GetInstance()->printf("Work has started,ID:%d\n", m_cameraid);
	}
	return 0;
}

int mythStreamServer::stop()
{
	SetStart(false);
	isrunning = 1;
	if (streamserverthread)
		streamserverthread->join();
	streamserverthread = nullptr;
	//delete this;
	return 0;
}

void mythStreamServer::SetOnCloseHandler(OnCloseHandler* handler,void* handlerdata)
{
	_handler = handler;
	_handlerdata = handlerdata;
}

int mythStreamServer::getClientNumber()
{
	unsigned int ret_size = 0;
	for (int i = 0; i < STREAMSERVERMAX; i++){
		if (_baselist[i]){
			ret_size++;
		}
	}
	return ret_size;
}
int mythStreamServer::DropClient(mythBaseClient* client)
{
	mythLog::GetInstance()->printf("Dropping Client,%u\n", client);
	for (int i = 0; i < STREAMSERVERMAX; i++){
		if (_baselist[i] == client){
			_baselist[i] = NULL;
			break;
		}
	}
	return 0;
}