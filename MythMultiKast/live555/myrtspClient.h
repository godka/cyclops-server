#ifndef RTSPCLIENT_HH_INCLUDED
#define RTSPCLIENT_HH_INCLUDED
#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include "OurRTSPClient.hh"
// Forward function definitions:
struct listRTSPclient
{
    RTSPClient *rtspclient;
    struct list_head list;
};
void continueAfterDESCRIBE(RTSPClient* rtspClient, int resultCode, char* resultString);
void continueAfterSETUP(RTSPClient* rtspClient, int resultCode, char* resultString);
void continueAfterPLAY(RTSPClient* rtspClient, int resultCode, char* resultString);

// Other event handler functions:
void subsessionAfterPlaying(void* clientData); // called when a stream's subsession (e.g., audio or video substream) ends
void subsessionByeHandler(void* clientData); // called when a RTCP "BYE" is received for a subsession
void streamTimerHandler(void* clientData);
  // called at the end of a stream's expected duration (if the stream has not already signaled its end using a RTCP "BYE")

// The main streaming routine (for each "rtsp://" URL):

extern struct list_head LISTRTSPCLIENT;
extern int timestamp;
// Used to iterate through each stream's 'subsessions', setting up each one:
void setupNextSubsession(RTSPClient* rtspClient);

// Used to shut down and close a stream (including its "RTSPClient" object):
void shutdownStream(RTSPClient* rtspClient, int exitCode = 1);

void beginDescrible(RTSPClient *rtspClient);
void startLoop(UsageEnvironment* env,char* watchVariable);
UsageEnvironment* initalAllcommons();
RTSPClient* openURL(UsageEnvironment& env, char const* rtspURL,char const* UserName,char const* Password,live555responseHandler* response,void* hwnd);

#endif // RTSPCLIENT_HH_INCLUDED
