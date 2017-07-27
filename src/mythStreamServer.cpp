#include "mythStreamServer.hh"
#include "mythVirtualSqlite.hh"

mythStreamServer* mythStreamServer::CreateNew(int cameraid,void* args){
	return new mythStreamServer(cameraid,args);
}
mythStreamServer* mythStreamServer::CreateNew(mythRequestParser* parser, int cameraid){
	return new mythStreamServer(parser,cameraid);
}

mythStreamServer* mythStreamServer::CreateNew(cJSON* parser, int cameraid /*= -1*/)
{
	return new mythStreamServer(parser, cameraid);
}

mythStreamServer::mythStreamServer(int cameraid, void* args)
{
	streamserverthread = nullptr;
	decoder = nullptr;
	m_cameraid = cameraid;
	additionalargs = args;
	_baselist = new mythBaseClient*[STREAMSERVERMAX];
	SetStart(false);
	memset(_baselist, 0, sizeof(mythBaseClient*) * STREAMSERVERMAX);
	connect();
}

mythStreamServer::mythStreamServer(mythRequestParser* parser, int cameraid)
{
	m_cameraid = cameraid;
	streamserverthread = nullptr;
	decoder = nullptr;
	_baselist = new mythBaseClient*[STREAMSERVERMAX];
	SetStart(false);
	memset(_baselist, 0, sizeof(mythBaseClient*) * STREAMSERVERMAX);
	connectViaUrl(parser);
}

mythStreamServer::mythStreamServer(cJSON* parser, int cameraid /*= -1*/)
{
	m_cameraid = cameraid;
	streamserverthread = nullptr;
	decoder = nullptr;
	_baselist = new mythBaseClient*[STREAMSERVERMAX];
	SetStart(false);
	memset(_baselist, 0, sizeof(mythBaseClient*) * STREAMSERVERMAX);
	connectViaUrl(parser);
}

mythStreamServer::~mythStreamServer(void)
{
	delete [] _baselist;
}
void mythStreamServer::connectViaUrl(cJSON* parser)
{
	auto url = cJSON_GetObjectItem(parser, "url");
	if (!url)
		return;
	std::string urlstr = url->valuestring;
	_parseUrl = urlstr;
	auto urlheader = parseUrlHeader(urlstr.c_str());
	if (strcmp(urlheader, "rtsp") == 0){
		std::string rtsptransport = cJSON_GetObjectItem(parser, "transport")?cJSON_GetObjectItem(parser, "transport")->valuestring : "";
		decoder = mythLive555Decoder::CreateNew((char*) urlstr.c_str(), rtsptransport == "tcp" ? true : false);
	}
	else if (strcmp(urlheader, "stream") == 0){
		decoder = mythStreamDecoder::CreateNew((char*) urlstr.c_str());
	}
	else if (strcmp(urlheader, "file") == 0){
#ifdef USEPIPELINE
		decoder = mythFFmpegDecoder::CreateNew(urlstr.c_str());
#else
		decoder = mythH264Decoder::CreateNew(urlstr.c_str());
#endif
	}
	else{
#ifdef USEPIPELINE
		std::string inputstr = "file://" + urlstr;
		decoder = mythFFmpegDecoder::CreateNew(inputstr.c_str());
#else
		mythLog::GetInstance()->printf("Error in Create Decoder,Please use --enable-pipeline to support:%s\n", urlstr.c_str());
#endif
	}
	delete [] urlheader;
	if (decoder)
		decoder->start();
}
void mythStreamServer::connectViaUrl(mythRequestParser* parser)
{
	auto url = parser->Parse("url");
	if (url == "")
		return;
	auto urlheader = parseUrlHeader(url.c_str());
	_parseUrl = url;
	if (strcmp(urlheader, "rtsp") == 0){
		auto rtsptransport = parser->Parse("transport");
		decoder = mythLive555Decoder::CreateNew((char*) url.c_str(), rtsptransport == "tcp" ? true : false);
	}
	else if (strcmp(urlheader, "stream") == 0){
		decoder = mythStreamDecoder::CreateNew((char*) url.c_str());
	}
	else if (strcmp(urlheader, "file") == 0){
#ifdef USEPIPELINE
		decoder = mythFFmpegDecoder::CreateNew(url.c_str());
#else
		decoder = mythH264Decoder::CreateNew(url.c_str());
#endif
	}
	else{
#ifdef USEPIPELINE
		std::string inputstr = "file://" + url;
		decoder = mythFFmpegDecoder::CreateNew(inputstr.c_str());
#else
		mythLog::GetInstance()->printf("Error in Create Decoder,Please use --enable-pipeline to support:%s\n",url.c_str());
#endif
	}
	delete [] urlheader;
	if (decoder)
		decoder->start();
}

char* mythStreamServer::parseUrlHeader(const char* url){
	unsigned int i = 0;
	for (i = 0; i < strlen(url); i++){
		if (url[i] == ':'){
			break;
		}
	}
	char* ret = new char[i + 1];
	memcpy(ret, url, i);
	ret[i] = '\0';
	return ret;
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
							_parseUrl = "stream://" + ip + "/" + realcameraid;
							break;
						default:
							std::string strRecordUrl = "rtsp://" + ip + ":" + httpport + FullSize;
							int iFind = strRecordUrl.find("$camera");
							if (iFind >= 0)
								strRecordUrl.replace(iFind, iFind + strlen("$camera"), realcameraid);
							this->decoder = mythLive555Decoder::CreateNew((char*) strRecordUrl.c_str(), (char*) username.c_str(), (char*) password.c_str());
							_parseUrl = strRecordUrl;
							break;
						}
						if (decoder){
							decoder->SetMagic((void*) m_cameraid);	//set magic
							decoder->start();
						}
					}
				}
				delete result;
			}
			else{
				//only for test
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
	mythLog::GetInstance()->printf("Work start,ID:%d\n", m_cameraid);
	PacketQueue* tmp = NULL;
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
							if (tmpclient->isfirst){
								auto Iframe_packet = decoder->getIframe();
								if (Iframe_packet){
									if (tmpclient->DataCallBack(Iframe_packet) < 0){
										DropClient(tmpclient);
									}
									delete[] Iframe_packet->Packet;
									delete Iframe_packet;
								}
								else{
#ifdef WIN32
									Sleep(1);
#else
									std::this_thread::sleep_for(std::chrono::milliseconds(1));
#endif
									//don't send;
									continue;
								}
							}
							streamcount++;
							if (tmpclient->DataCallBack(tmp) < 0){
								DropClient(tmpclient);
							}
						}
					}
				}
				decoder->release(tmp);
			}
			else{
#ifdef WIN32
				Sleep(1);
#else
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
#endif
			}
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
	return 0;
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

int mythStreamServer::DropClient(int SockID){
	mythLog::GetInstance()->printf("Dropping Client,socketid = %d\n", SockID);
	for (int i = 0; i < STREAMSERVERMAX; i++){
		if (_baselist[i]){
			if (_baselist[i]->GetSockID() == SockID){
				_baselist[i] = NULL;
				break;
			}
		}
	}
	return 0;

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