#ifndef OURRTSPCLIENT_HH_INCLUDED
#define OURRTSPCLIENT_HH_INCLUDED
#ifndef _MEDIA_SINK_HH
#include "MediaSink.hh"
#endif

typedef void (live555responseHandler)(void *myth,unsigned char* data, unsigned int length);
// If you're streaming just a single stream (i.e., just from a single URL, once), then you can define and use just a single
// "StreamClientState" structure, as a global variable in your application.  However, because - in this demo application - we're
// showing how to play multiple streams, concurrently, we can't do that.  Instead, we have to have a separate "StreamClientState"
// structure for each "RTSPClient".  To do this, we subclass "RTSPClient", and add a "StreamClientState" field to the subclass:
class StreamClientState {
public:
  StreamClientState();
  virtual ~StreamClientState();

public:
  void* phwnd;
  live555responseHandler* myresponse;
  MediaSubsessionIterator* iter;
  MediaSession* session;
  MediaSubsession* subsession;
  TaskToken streamTimerTask;
  MediaSink *currentSink;
  Authenticator *authenticator;
  double duration;
};

class ourRTSPClient: public RTSPClient {
public:
  static ourRTSPClient* createNew(UsageEnvironment& env, char const* rtspURL,
				  int verbosityLevel = 0,
				  char const* applicationName = NULL,
				  portNumBits tunnelOverHTTPPortNum = 0,Authenticator* authenticatorName = NULL,
				  live555responseHandler* response = NULL,void* hwnd = NULL);

protected:
  ourRTSPClient(UsageEnvironment& env, char const* rtspURL,
		int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum
		,Authenticator* authenticatorName,live555responseHandler* response,void* hwnd);
    // called only by createNew();
  virtual ~ourRTSPClient();

public:
  StreamClientState scs;
};


#endif // OURRTSPCLIENT_HH_INCLUDED
