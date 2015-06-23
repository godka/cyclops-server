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
/********************************************************************
Created by MythKAst
©2013 MythKAst Some rights reserved.


You can build it with vc2010,gcc.
Anybody who gets this source code is able to modify or rebuild it anyway,
but please keep this section when you want to spread a new version.
It's strongly not recommended to change the original copyright. Adding new version
information, however, is Allowed. Thanks.
For the latest version, please be sure to check my website:
Http://code.google.com/mythkast


你可以用vc2010,gcc编译这些代码
任何得到此代码的人都可以修改或者重新编译这段代码，但是请保留这段文字。
请不要修改原始版权，但是可以添加新的版本信息。
最新版本请留意：Http://code.google.com/mythkast
B
MythKAst(asdic182@sina.com), in 2013 June.
*********************************************************************/
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
