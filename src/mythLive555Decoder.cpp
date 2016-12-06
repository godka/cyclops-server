#include "mythLive555Decoder.hh"

mythLive555Decoder::mythLive555Decoder(char* rtsplink, char* username, char* password, bool usetcp)
	:mythVirtualDecoder()
{
	if (rtsplink)
		m_rtsplink = rtsplink;
	if (username)
		m_username = username;
	if (password)
		m_password = password;
	flag = 0;
	isrunning = false;
	fusetcp = usetcp;
	_times = 0;
}

void mythLive555Decoder::OnReconnect(int times)
{
	//mythDelay(1000);
#ifdef _DEBUG
	printf("Ready to reconnect %d\n", times);
#endif
}

int mythLive555Decoder::ParseRTSPlink(char* src, char* rtsplink, char* username, char* password)
{
	int len = strlen(src);
	//find @
	int i = 0, j = 0;
	for (i = 0; i < len; i++){
		if (src[i] == '@'){
			break;
		}
	}
	if (i == len){
		strcpy(rtsplink, src);
		username = "";
		password = "";
		return 0;
	}
	else{
		rtsplink[0] = 'r'; rtsplink[1] = 't'; rtsplink[2] = 's'; rtsplink[3] = 'p';
		rtsplink[4] = ':'; rtsplink[5] = '/'; rtsplink[6] = '/';
		strcpy(rtsplink + 7, src + i + 1);
	}
	for (j = 7; j < len; j++){
		if (src[j] == ':'){
			break;
		}
	}
	if (j == len){
		strcpy(rtsplink, src);
		username = "";
		password = "";
	}
	else{
		memcpy(username, &src[7], j - 7);
		memcpy(password, &src[j + 1], i - j - 1);
	}
	return 0;
}

mythLive555Decoder* mythLive555Decoder::CreateNew(char* rtsplink, char* username, char* password, bool usetcp){
	return new mythLive555Decoder(rtsplink, username, password, usetcp);
}

mythLive555Decoder* mythLive555Decoder::CreateNew(char* rtsplink, bool usetcp /*= true*/)
{
	//parse rtsp url start
	//auto rtsp link : rtsp://user:pass@link
	char tmplink[256] = { 0 };
	char tmpusername[32] = { 0 };
	char tmppassword[32] = { 0 };
	ParseRTSPlink(rtsplink, tmplink, tmpusername, tmppassword);
	return CreateNew(tmplink, tmpusername, tmppassword, usetcp);
}
void mythLive555Decoder::callbackdatastatic(void *myth, unsigned char* data, unsigned int length, unsigned int timestamp){
	mythLive555Decoder* mythlive555 = (mythLive555Decoder*) myth;
	mythlive555->callbackdata(data, length, timestamp);
}
void mythLive555Decoder::callbackdata(unsigned char* data, unsigned int length, unsigned int timestamp)
{
	//OnFrameCallback((char*) data, length, timestamp, OnFrameData);
	put(data,length,timestamp);
}
int mythLive555Decoder::MainLoop(){
	isrunning = true;
	while (isrunning){
		rtsp = mythRTSP::CreateNew();
		if (rtsp){
			client = rtsp->openURL(this->m_rtsplink.c_str(), this->m_username.c_str(), this->m_password.c_str(), fusetcp, mythLive555Decoder::callbackdatastatic, (void*)this);
			if (client){
				rtsp->Start(client);
				//rtsp->shutdownStream(client);
				//rtsp->Stop(client);
			}
			//delete rtsp;
		}
		_times++;
		OnReconnect(_times);
	}
	return 0;
}
mythLive555Decoder::~mythLive555Decoder(void)
{
}

void mythLive555Decoder::stop()
{
	isrunning = false;
	if (rtsp && client)
		rtsp->Stop(client);
	delete rtsp;
}
