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
// Copyright (c) 1996-2015 Live Networks, Inc.  All rights reserved.
// Basic Usage Environment: for a simple, non-scripted, console application
// Implementation

#include "mythUseageEnvironment.hh"
#include "mythLog.hh"
#include <stdio.h>

////////// BasicUsageEnvironment //////////

#if defined(__WIN32__) || defined(_WIN32)
extern "C" int initializeWinsockIfNecessary();
#endif

mythUseageEnvironment::mythUseageEnvironment(TaskScheduler& taskScheduler)
	: BasicUsageEnvironment0(taskScheduler) {
#if defined(__WIN32__) || defined(_WIN32)
	if (!initializeWinsockIfNecessary()) {
		setResultErrMsg("Failed to initialize 'winsock': ");
		reportBackgroundError();
		internalError();
	}
#endif
}

mythUseageEnvironment::~mythUseageEnvironment() {
}

mythUseageEnvironment*
mythUseageEnvironment::createNew(TaskScheduler& taskScheduler) {
	return new mythUseageEnvironment(taskScheduler);
}

int mythUseageEnvironment::getErrno() const {
#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN32_WCE)
	return WSAGetLastError();
#else
	return errno;
#endif
}

UsageEnvironment& mythUseageEnvironment::operator<<(char const* str) {
	if (str == NULL) str = "(NULL)"; // sanity check
	mythLog::GetInstance()->printf("%s", str);
	return *this;
}

UsageEnvironment& mythUseageEnvironment::operator<<(int i) {
	mythLog::GetInstance()->printf("%d", i);
	fprintf(stderr, "%d", i);
	return *this;
}

UsageEnvironment& mythUseageEnvironment::operator<<(unsigned u) {
	mythLog::GetInstance()->printf("%u", u);
	return *this;
}

UsageEnvironment& mythUseageEnvironment::operator<<(double d) {
	mythLog::GetInstance()->printf("%f", d);
	fprintf(stderr, "%f", d);
	return *this;
}

UsageEnvironment& mythUseageEnvironment::operator<<(void* p) {
	mythLog::GetInstance()->printf("%p", p);
	return *this;
}
