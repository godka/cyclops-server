
// "liveMedia"
// H.264 Video Stream sinks
// Implementation
#include "H264VideoStreamSink.hh"
//#include "OutputFile.hh"
#include "H264VideoRTPSource.hh"
#ifndef TOTALSIZE
    #define TOTALSIZE  1024*1024;
#endif

////////// H264VideoFileSink //////////
//LIST_HEAD_INIT(listhead);
H264VideoStreamSink::H264VideoStreamSink(UsageEnvironment& env,char const* sPropParameterSetsStr,unsigned bufferSize
	,live555responseHandler* response,void* hwnd)
  : StreamSink(env, bufferSize),
    fSPropParameterSetsStr(sPropParameterSetsStr), fHaveWrittenFirstFrame(False),finital(False) {
    if (!finital) {
        //struct Framestruct *framestruct;
        fBufferTotalSize = TOTALSIZE;
        fBufferTotal = new unsigned char[fBufferTotalSize];
        fBufferPtr = 0;
        fBufferNum = 0;
        finital = !finital;
		live555response = response;
		phwnd = hwnd;
    }

}

H264VideoStreamSink::~H264VideoStreamSink() {
}

H264VideoStreamSink*
H264VideoStreamSink::createNew(UsageEnvironment& env, char const* sPropParameterSetsStr,unsigned bufferSize,
	live555responseHandler* response,void* hwnd) {

  do {
    return new H264VideoStreamSink(env, sPropParameterSetsStr, bufferSize,response,hwnd);
  } while (0);

  return NULL;
}

void H264VideoStreamSink::afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes, struct timeval presentationTime) {
  unsigned char const start_code[4] = {0x00, 0x00, 0x00, 0x01};

  if (!fHaveWrittenFirstFrame) {
    // If we have PPS/SPS NAL units encoded in a "sprop parameter string", prepend these to the file:
    unsigned numSPropRecords;
    SPropRecord* sPropRecords = parseSPropParameterSets(fSPropParameterSetsStr, numSPropRecords);
    for (unsigned i = 0; i < numSPropRecords; ++i) {
      addData(start_code, 4);
      addData(sPropRecords[i].sPropBytes, sPropRecords[i].sPropLength);
    }
    delete[] sPropRecords;
    fHaveWrittenFirstFrame = True; // for next time
  }

  // Write the input data to the file, with the start code in front:
  addData(start_code, 4);

  // Call the parent class to complete the normal file write with the input data:
  StreamSink::afterGettingFrame(frameSize, numTruncatedBytes, presentationTime);
}
