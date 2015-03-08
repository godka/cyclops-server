/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// "liveMedia"
// Copyright (c) 1996-2013 Live Networks, Inc.  All rights reserved.
// H.264 Video File Sinks
// C++ header

#ifndef _H264_VIDEO_STEAM_SINK_HH
#define _H264_VIDEO_STEAM_SINK_HH

#ifndef _STREAM_SINK_HH
#include "StreamSink.hh"
#endif

class H264VideoStreamSink: public StreamSink {
public:
  //typedef void (live555responseHandler)(void *myth,unsigned char* data, unsigned int length);
  static H264VideoStreamSink* createNew(UsageEnvironment& env,
					  char const* sPropParameterSetsStr = NULL,
  // An optional 'SDP format' string (comma-separated Base64-encoded) representing SPS and/or PPS NAL-units to prepend to the output
				      unsigned bufferSize = 2000000,live555responseHandler* response = NULL,void* hwnd = NULL);
  // See "FileSink.hh" for a description of these parameters.

protected:
  H264VideoStreamSink(UsageEnvironment& env,char const* sPropParameterSetsStr,
		    unsigned bufferSize,live555responseHandler* response,void* hwnd);
      // called only by createNew()
  virtual ~H264VideoStreamSink();

protected: // redefined virtual functions:
  virtual void afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes, struct timeval presentationTime);

private:
  char const* fSPropParameterSetsStr;
  Boolean fHaveWrittenFirstFrame;
  Boolean finital;
};

#endif
