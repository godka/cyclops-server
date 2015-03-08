
// stream sinks
// Implementation
#if (defined(__WIN32__) || defined(_WIN32)) && !defined(_WIN32_WCE)
#include <io.h>
#include <fcntl.h>
#endif
#include "StreamSink.hh"
#include "GroupsockHelper.hh"  
#include "MyDll1.h"
unsigned timestamp;
FILE *file;
//#include "OutputFile.hh"
#define printtime(name) \
	int tmp = GetTickCount();printf(name);printf("time = %dms\n",tmp-timestamp);timestamp = tmp;

////////// FileSink //////////
//unsigned char fBuffer[BUFFERSIZE];
StreamSink::StreamSink(UsageEnvironment& env, unsigned bufferSize)
  : MediaSink(env), fBufferSize(bufferSize) {
  fBuffer = new unsigned char[bufferSize];
  //fBufferPtr = 0;
}

StreamSink::~StreamSink() {
//  delete[] fPerFrameFileNameBuffer;
//  delete[] fPerFrameFileNamePrefix;
  delete[] fBuffer;
  delete[] fBufferTotal;
}

StreamSink* StreamSink::createNew(UsageEnvironment& env,unsigned bufferSize,unsigned buffertotalSize) {
  //fBufferTotal = new unsigned char[buffertotalSize];
 // fBufferTotalSize = buffertotalSize;
  do {	
    return new StreamSink(env,bufferSize);
  } while (0);

  return NULL;
}

Boolean StreamSink::continuePlaying() {
  if (fSource == NULL) return False;
  //printtime("continueplaying");
  fSource->getNextFrame(fBuffer, fBufferSize,
			afterGettingFrame, this,
			onSourceClosure, this);

  return True;
}

void StreamSink::afterGettingFrame(void* clientData, unsigned frameSize,
				 unsigned numTruncatedBytes,
				 struct timeval presentationTime,
				 unsigned /*durationInMicroseconds*/) {
  StreamSink* sink = (StreamSink*)clientData;
  sink->afterGettingFrame(frameSize, numTruncatedBytes, presentationTime);
}

void StreamSink::addDataBase(unsigned char const* data, unsigned dataSize){
    if (fBufferTotal != NULL && data != NULL) {
        if(dataSize < fBufferTotalSize - fBufferPtr){
            memcpy(fBufferTotal+fBufferPtr,data,dataSize);
            fBufferPtr+=dataSize;
        }else{
            memcpy(fBufferTotal+fBufferPtr,data,fBufferTotalSize - fBufferPtr);
            memcpy(fBufferTotal,data,dataSize + fBufferPtr - fBufferTotalSize );
            fBufferPtr = dataSize + fBufferPtr - fBufferTotalSize;

        }
    #ifdef _DEBUG
	  int percent = (int)fBufferPtr * 100 / (int)fBufferTotalSize;
      printf("fBufferPtr=%d,fBufferTotal=%d,%d%\r\n",fBufferPtr,fBufferTotalSize,percent);
	//#else
      //puts((const char*)data);
    #endif
    }
	
}
void StreamSink::addDataString(char const* data) {
  // Write to our file:  // Write to our file:
   // addDataBase((unsigned char const*)data,strlen(data));
}
void StreamSink::addData(unsigned char const* data, unsigned dataSize) {
  // Write to our file:
    addDataBase(data,dataSize);
}

void StreamSink::afterGettingFrame(unsigned frameSize,
				 unsigned numTruncatedBytes,
				 struct timeval presentationTime) {
  //printtime("aftergettingframe");
  if (numTruncatedBytes > 0) {
    envir() << "FileSink::afterGettingFrame(): The input frame data was too large for our buffer size ("
	    << fBufferSize << ").  "
            << numTruncatedBytes << " bytes of trailing data was dropped!  Correct this by increasing the \"bufferSize\" parameter in the \"createNew()\" call to at least "
            << fBufferSize + numTruncatedBytes << "\n";
  }
  addData(fBuffer, frameSize);
  /*
#ifndef _DEBUG
  
  file = fopen("test.h264","ab+");
  fwrite(fBufferTotal,fBufferPtr,1,file);
  fclose(file);
  
  
  //AvCodeGetFrameRGB((unsigned char*)fBufferTotal,fBufferPtr);
#else
  */
  AvCodeGetFrame((unsigned char const*)fBufferTotal,fBufferPtr);
//#endif
  fBufferPtr = 0;
  // Then try getting the next frame:
  continuePlaying();
}
