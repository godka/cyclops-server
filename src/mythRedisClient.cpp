#include "mythRedisClient.hh"


mythRedisClient::mythRedisClient(int cameraid)
{
	m_cameraid = cameraid;
	decoder = NULL;
}


mythRedisClient::~mythRedisClient()
{
}

void mythRedisClient::start()
{
	printf("cameraid = %d start\n",m_cameraid);
#if 0
	this->decoder = mythLive555Decoder::CreateNew("rtsp://1029.mythkast.net/test.264",NULL,NULL);
	if (decoder){
		decoder->SetMagic((void*) m_cameraid);	//set magic
		decoder->start(false);
	}
#else
	char sqltmp[4096] = { 0 };
	sprintf(sqltmp, FINDCAMERA, m_cameraid);
	mythStreamSQLresult* result = mythVirtualSqlite::GetInstance()->doSQLFromStream(sqltmp);
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
					decoder->start(false);
				}
				//SDL_UnlockMutex(decodemutex);
			}
		}
		delete result;
	}
	else{
		this->decoder = mythStreamDecoder::CreateNew("120.204.70.218", 1017);

		if (decoder){
			decoder->SetMagic((void*) m_cameraid);	//set magic
			decoder->start(false);
		}
	}
#endif
}

void mythRedisClient::stop()
{

}
