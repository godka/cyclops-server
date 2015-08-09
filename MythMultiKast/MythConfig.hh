#pragma once
#include <stdio.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_net.h"
#include "mythUdp.hh"

#ifdef _WIN32
	#pragma comment(lib,"ws2_32")
	#ifdef _DEBUG
		#pragma comment(lib,"live555-multikast-debug.lib")
	#else
		#pragma comment(lib,"live555-multikast-release.lib")
	#endif
#endif
#define mythcmp(A) strcmp(input,A) == 0
#define streamserverport 5834

//#define MYTH_CONFIG_SENDMESSAGE_SLOW
#define MYTH_CONFIG_SENDMESSAGE_FAST
#define MYTH_INFORMATIONINI_FILE "mythconfig.ini"
//#define MYTH_STREAM_CLOSE