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
			else{
				this->decoder = mythStreamDecoder::CreateNew("192.168.31.198", 1017);
				//this->decoder = mythLive555Decoder::CreateNew("rtsp://192.168.31.128:554/tcp/av0_0", "admin", "888888");
				if (decoder){
					decoder->SetMagic((void*) m_cameraid);	//set magic
					decoder->start();
				}
			}
			//if (result)
			break;
		case 1:
			decoder = mythRedisDecoder::CreateNew(m_cameraid);
			if (decoder){
				decoder->SetMagic((void*) m_cameraid);	//set magic
				decoder->start();
			}
			break;
		case 2:
			this->decoder = mythRedisDecoder::CreateNew(m_cameraid);
			if (decoder){
				decoder->SetMagic((void*) m_cameraid);	//set magic
				decoder->start();
			}
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
	printf("Appending Client,%d\n", client);
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
int mythStreamServer::mainthread()
{
	//int msize = 0;
	long long duration = 0;
	PacketQueue* tmp = NULL;
	connect();
	long long tickstart = 0;
	while (isrunning == 0){
		if (decoder){
			tmp = decoder->get();
			if (tmp){
				if (tmp->h264PacketLength > 0){
					auto timestart = mythTickCount();
					int streamcount = 0;
					for (int i = 0; i < STREAMSERVERMAX; i++){
						mythBaseClient* tmpclient = _baselist[i];
						if (tmpclient){
							streamcount++;
							//if (tmpclient->isfirst){
							//	if (tmp->isIframe)
							tmpclient->DataCallBack(tmp->h264Packet, tmp->h264PacketLength);
							//}
							//else{
							//	tmpclient->DataCallBack(tmp->h264Packet, tmp->h264PacketLength);
							//}
						}
					}
					if(streamcount == 0){
						//add ticks
						if (tickstart == 0)
							tickstart = mythTickCount();
						else{
							duration = mythTickCount() - tickstart;
							if (duration > 5000){
								break;
							}
						}
					}
					else{
						//refresh ticks
						tickstart = mythTickCount();
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
	streamserverthread = NULL;
	printf("%d is closed\n", m_cameraid);
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
		printf("Work has started,ID:%d\n", m_cameraid);
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
	printf("Dropping Client,%d\n", client);
	for (int i = 0; i < STREAMSERVERMAX; i++){
		if (_baselist[i] == client){
			_baselist[i] = NULL;
			break;
		}
	}
	return 0;
}