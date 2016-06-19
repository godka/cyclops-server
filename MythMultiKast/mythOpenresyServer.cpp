#include "mythOpenresyServer.hh"
#include "mythStreamSQLresult.hh"
#include "mythVirtualSqlite.hh"
#include "mythStreamDecoder.hh"
#include "mythLive555Decoder.hh"
#include "mythRedisDecoder.hh"


mythOpenresyServer::mythOpenresyServer(int port)
	:mythVirtualServer(port)
{
	mapmutex = SDL_CreateMutex();
	mythVirtualDecoder* server = connectServer(1017);			//add a new server into map list,not found ,so create 
	servermap[1017] = server;
	server->start();
}

mythOpenresyServer* mythOpenresyServer::CreateNew(int port)
{
	return new mythOpenresyServer(port);
}
mythOpenresyServer::~mythOpenresyServer()
{
}
mythVirtualDecoder* mythOpenresyServer::connectServer(int cameraid){
	char sqltmp[4096] = { 0 };
	mythVirtualDecoder* decoder = NULL;
	mythStreamSQLresult* result = NULL;
	SDL_snprintf(sqltmp, 4096, FINDCAMERA, cameraid);
	result = mythVirtualSqlite::GetInstance()->doSQLFromStream(sqltmp);
	if (result){
		while (result->MoveNext()){
			string username = result->TryPrase("username");
			string password = result->TryPrase("password");
			string httpport = result->TryPrase("SubName");
			string FullSize = result->TryPrase("FullSize");
			string vstypeid = result->TryPrase("vstypeid");
			string ip = result->TryPrase("ip");
			string realcameraid = result->TryPrase("Port");
			switch (atoi(vstypeid.c_str()))
			{
			case 88:
				decoder = mythStreamDecoder::CreateNew((char*) ip.c_str(), atoi(realcameraid.c_str()));
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
				decoder = mythLive555Decoder::CreateNew((char*) strRecordUrl.c_str(), (char*) username.c_str(), (char*) password.c_str());
				break;
			}
			//SDL_LockMutex(decodemutex);
			if (decoder){
				decoder->SetMagic((void*) cameraid);	//set magic
			}
		}
		delete result;
	}
	else{
		decoder = mythStreamDecoder::CreateNew("120.204.70.218", 1017);

		if (decoder){
			decoder->SetMagic((void*) cameraid);	//set magic
		}
	}
	return decoder;
}

void mythOpenresyServer::ServerDecodeCallBack(MythSocket* people, char* data, int datalength)
{

	map<int, mythVirtualDecoder*>::iterator Iter;
	int cameraid = -1;
	char cameratype[20] = { 0 };
	SDL_sscanf(data, "GET /CameraID=%d&Type=%s ", &cameraid, cameratype);
	if (cameraid != -1){
		mythVirtualDecoder* server;
		Iter = servermap.find(cameraid);
		if (Iter == servermap.end() || Iter->second == NULL){
			SDL_LockMutex(mapmutex);
			server = connectServer(cameraid);			//add a new server into map list,not found ,so create 
			servermap[cameraid] = server;
			SDL_UnlockMutex(mapmutex);
			server->start();
		}
	}
	return;
}

void mythOpenresyServer::ServerCloseCallBack(MythSocket* people)
{

}
