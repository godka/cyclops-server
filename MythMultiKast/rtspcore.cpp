#include "myrtspClient.h"
char eventLoopWatchVariable = 0;
UsageEnvironment* initalAllcommons()
{

  TaskScheduler* scheduler = BasicTaskScheduler::createNew();
  UsageEnvironment* env = BasicUsageEnvironment::createNew(*scheduler);
  return env;
}
void startLoop(UsageEnvironment* env,char* watchVariable)
{
  env->taskScheduler().doEventLoop(watchVariable);
}
#define RTSP_CLIENT_VERBOSITY_LEVEL 0 // by default, print verbose output from each "RTSPClient"

static unsigned rtspClientCount = 0; // Counts how many streams (i.e., "RTSPClient"s) are currently in use.

RTSPClient* openURL(UsageEnvironment& env,char const* rtspURL,char const* UserName,char const* Password
	,live555responseHandler* response,void* hwnd) {
  // Begin by creating a "RTSPClient" object.  Note that there is a separate "RTSPClient" object for each stream that we wish
  // to receive (even if more than stream uses the same "rtsp://" URL).
  Authenticator *author = new Authenticator(UserName,Password);
  return(RTSPClient*)ourRTSPClient::createNew(env, rtspURL, RTSP_CLIENT_VERBOSITY_LEVEL, "mythkast-live555",0,author
	  ,response,hwnd);
}
void beginDescrible(RTSPClient *rtspClient)
{
    StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias
    rtspClient->sendDescribeCommand(continueAfterDESCRIBE,scs.authenticator);
}
// Implementation of the RTSP 'response handlers':
void continueAfterDESCRIBE(RTSPClient* rtspClient, int resultCode, char* resultString) {
  do {
    UsageEnvironment& env = rtspClient->envir(); // alias
    StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias

    if (resultCode != 0) {
      //env << *rtspClient << "Failed to get a SDP description: " << resultString << "\n";
      delete[] resultString;
      break;
    }

    char* const sdpDescription = resultString;
//    env << *rtspClient << "Got a SDP description:\n" << sdpDescription << "\n";

    // Create a media session object from this SDP description:
    scs.session = MediaSession::createNew(env, sdpDescription);
    delete[] sdpDescription; // because we don't need it anymore
    if (scs.session == NULL) {
      break;
    } else if (!scs.session->hasSubsessions()) {
      break;
    }

    // Then, create and set up our data source objects for the session.  We do this by iterating over the session's 'subsessions',
    // calling "MediaSubsession::initiate()", and then sending a RTSP "SETUP" command, on each one.
    // (Each 'subsession' will have its own data source.)
    scs.iter = new MediaSubsessionIterator(*scs.session);
    setupNextSubsession(rtspClient);
    return;
  } while (0);

  // An unrecoverable error occurred with this stream.
  shutdownStream(rtspClient);
}

// By default, we request that the server stream its data using RTP/UDP.
// If, instead, you want to request that the server stream via RTP-over-TCP, change the following to True:

void setupNextSubsession(RTSPClient* rtspClient) {
  UsageEnvironment& env = rtspClient->envir(); // alias
  StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias

  scs.subsession = scs.iter->next();

  if (scs.subsession != NULL) {
    if (!scs.subsession->initiate()) {
      //env << *rtspClient << "Failed to initiate the \"" << *scs.subsession << "\" subsession: " << env.getResultMsg() << "\n";
      setupNextSubsession(rtspClient); // give up on this subsession; go to the next one
    } else {
      //env << *rtspClient << "Initiated the \"" << *scs.subsession
	  //<< "\" subsession (client ports " << scs.subsession->clientPortNum() << "-" << scs.subsession->clientPortNum()+1 << ")\n";

      // Continue setting up this subsession, by sending a RTSP "SETUP" command:
      rtspClient->sendSetupCommand(*scs.subsession, continueAfterSETUP, False, True,False,scs.authenticator);
    }
    return;
  }

  // We've finished setting up all of the subsessions.  Now, send a RTSP "PLAY" command to start the streaming:
  if (scs.session->absStartTime() != NULL) {
    // Special case: The stream is indexed by 'absolute' time, so send an appropriate "PLAY" command:
    rtspClient->sendPlayCommand(
    *scs.session,
    continueAfterPLAY,
    scs.session->absStartTime(),
    scs.session->absEndTime(),
    1.0f,
    scs.authenticator);
  } else {
    scs.duration = scs.session->playEndTime() - scs.session->playStartTime();
    rtspClient->sendPlayCommand(
    *scs.session,
    continueAfterPLAY,
    0.0f,-1.0f,1.0f,scs.authenticator);
  }
  //puts("send play OK");
}

void continueAfterSETUP(RTSPClient* rtspClient, int resultCode, char* resultString) {
  do {
    UsageEnvironment& env = rtspClient->envir(); // alias
    StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias

    if (resultCode != 0) {
      break;
    }
    MediaSink *fileSink = NULL;
	if (strcmp(scs.subsession->mediumName(), "video") == 0) {
		// For H.264 video stream, we use a special sink that insert start_codes:
        fileSink = H264VideoStreamSink::createNew(env,
			scs.subsession->fmtp_spropparametersets(),2*1024*1024,scs.myresponse,scs.phwnd);
		scs.currentSink = fileSink;//H264sink
		scs.subsession->sink = fileSink;

	}
    if (scs.subsession->sink == NULL) {
      break;
    }
    scs.subsession->miscPtr = rtspClient; // a hack to let subsession handle functions get the "RTSPClient" from the subsession
    scs.subsession->sink->startPlaying(*(scs.subsession->readSource()),
				       subsessionAfterPlaying, scs.subsession);
    if (scs.subsession->rtcpInstance() != NULL) {
		scs.subsession->rtcpInstance()->setByeHandler(subsessionByeHandler, scs.myresponse,scs.phwnd);
    }
  } while (0);
  delete[] resultString;

  // Set up the next subsession, if any:
  setupNextSubsession(rtspClient);
}

void continueAfterPLAY(RTSPClient* rtspClient, int resultCode, char* resultString) {
  Boolean success = False;

  do {
    UsageEnvironment& env = rtspClient->envir(); // alias
    StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias

    if (resultCode != 0) {
      //env << *rtspClient << "Failed to start playing session: " << resultString << "\n";
      break;
    }

    // Set a timer to be handled at the end of the stream's expected duration (if the stream does not already signal its end
    // using a RTCP "BYE").  This is optional.  If, instead, you want to keep the stream active - e.g., so you can later
    // 'seek' back within it and do another RTSP "PLAY" - then you can omit this code.
    // (Alternatively, if you don't want to receive the entire stream, you could set this timer for some shorter value.)
    //if (scs.duration > 0) {
    //  unsigned const delaySlop = 2; // number of seconds extra to delay, after the stream's expected duration.  (This is optional.)
    //  scs.duration += delaySlop;
    //  unsigned uSecsToDelay = (unsigned)(scs.duration*1000000);
    //  scs.streamTimerTask = env.taskScheduler().scheduleDelayedTask(uSecsToDelay, (TaskFunc*)streamTimerHandler, rtspClient);
    //}

    //env << *rtspClient << "Started playing session";
    if (scs.duration > 0) {
      env << " (for up to " << scs.duration << " seconds)";
    }
    //env << "...\n";

    success = True;
	//Sleep(1000);
  } while (0);
  delete[] resultString;

  if (!success) {
    // An unrecoverable error occurred with this stream.
    shutdownStream(rtspClient);
  }
}


// Implementation of the other event handlers:

void subsessionAfterPlaying(void* clientData) {
  MediaSubsession* subsession = (MediaSubsession*)clientData;
  RTSPClient* rtspClient = (RTSPClient*)(subsession->miscPtr);

  // Begin by closing this subsession's stream:
  Medium::close(subsession->sink);
  subsession->sink = NULL;

  // Next, check whether *all* subsessions' streams have now been closed:
  MediaSession& session = subsession->parentSession();
  MediaSubsessionIterator iter(session);
  while ((subsession = iter.next()) != NULL) {
    if (subsession->sink != NULL) return; // this subsession is still active

    //subsession-
  }

  // All subsessions' streams have now been closed, so shutdown the client:
  shutdownStream(rtspClient);
}

void subsessionByeHandler(void* clientData) {
  MediaSubsession* subsession = (MediaSubsession*)clientData;
  RTSPClient* rtspClient = (RTSPClient*)subsession->miscPtr;
  UsageEnvironment& env = rtspClient->envir(); // alias

//  env << *rtspClient << "Received RTCP \"BYE\" on \"" << *subsession << "\" subsession\n";

  // Now act as if the subsession had closed:
  subsessionAfterPlaying(subsession);
}

void streamTimerHandler(void* clientData) {
  ourRTSPClient* rtspClient = (ourRTSPClient*)clientData;
  StreamClientState& scs = rtspClient->scs; // alias

  scs.streamTimerTask = NULL;

  // Shut down the stream:
  shutdownStream(rtspClient);
}

void shutdownStream(RTSPClient* rtspClient, int exitCode) {
	UsageEnvironment& env = rtspClient->envir(); // alias
	StreamClientState& scs = ((ourRTSPClient*) rtspClient)->scs; // alias

	// First, check whether any subsessions have still to be closed:
	if (scs.session != NULL) {
		Boolean someSubsessionsWereActive = False;
		MediaSubsessionIterator iter(*scs.session);
		MediaSubsession* subsession;

		while ((subsession = iter.next()) != NULL) {
			if (subsession->sink != NULL) {
				Medium::close(subsession->sink);
				subsession->sink = NULL;

				if (subsession->rtcpInstance() != NULL) {
					subsession->rtcpInstance()->setByeHandler(NULL, NULL); // in case the server sends a RTCP "BYE" while handling "TEARDOWN"
				}

				someSubsessionsWereActive = True;
			}
		}

		if (someSubsessionsWereActive) {
			// Send a RTSP "TEARDOWN" command, to tell the server to shutdown the stream.
			// Don't bother handling the response to the "TEARDOWN".
			rtspClient->sendTeardownCommand(*scs.session, NULL);
		}
	}

	//  env << *rtspClient << "Closing the stream.\n";
	Medium::close(rtspClient);
	// Note that this will also cause this stream's "StreamClientState" structure to get reclaimed.

	if (--rtspClientCount <= 0) {
		// The final stream has ended, so exit the application now.
		// (Of course, if you're embedding this code into your own application, you might want to comment this out,
		// and replace it with "eventLoopWatchVariable = 1;", so that we leave the LIVE555 event loop, and continue running "main()".)
		exit(exitCode);
	}
}


// Implementation of "ourRTSPClient":

ourRTSPClient* ourRTSPClient::createNew(UsageEnvironment& env, char const* rtspURL,
					int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum
					,Authenticator* authenticatorName,live555responseHandler* response,void* hwnd) {
  return new ourRTSPClient(env, rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum,authenticatorName,response,hwnd);
}

ourRTSPClient::ourRTSPClient(UsageEnvironment& env, char const* rtspURL,
			     int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum
			     ,Authenticator* authenticatorName,live555responseHandler* response,void* hwnd)
  : RTSPClient(env,rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum) {
  scs.authenticator = authenticatorName ;
  scs.phwnd = hwnd;
  scs.myresponse = response;
}


ourRTSPClient::~ourRTSPClient() {
}


// Implementation of "StreamClientState":

StreamClientState::StreamClientState()
  : iter(NULL), session(NULL), subsession(NULL), streamTimerTask(NULL), duration(0.0) {
}

StreamClientState::~StreamClientState() {
  delete iter;
  if (session != NULL) {
    // We also need to delete "session", and unschedule "streamTimerTask" (if set)
    UsageEnvironment& env = session->envir(); // alias

    env.taskScheduler().unscheduleDelayedTask(streamTimerTask);
    Medium::close(session);
  }
}
