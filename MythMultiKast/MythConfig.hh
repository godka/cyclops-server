#pragma once
#ifdef _WIN32
#	define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdio.h>
#ifndef ANDROID
#	include "SDL2/SDL.h"
#	include "SDL2/SDL_net.h"
#else
#	include "SDL.h"
#	include "SDL_net.h"
#endif
#include "mythUdp.hh"
#include "IniFile.h"

#ifdef _WIN32
#	
#	pragma comment(lib,"ws2_32")
#	ifdef _DEBUG
#		pragma comment(lib,"live555-multikast-debug.lib")
#	else
		#pragma comment(lib,"live555-multikast-release.lib")
#	endif
#	pragma comment(lib,"SDL2.lib")
#	pragma comment(lib,"SDL2main.lib")
#	pragma comment(lib,"SDL2_net.lib")
#endif

#define mythcmp(A) strcmp(input,A) == 0
#define streamserverport 5834

//#define MYTH_CONFIG_SENDMESSAGE_SLOW
#define MYTH_CONFIG_SENDMESSAGE_FAST
#define MYTH_INFORMATIONINI_FILE "mythconfig.ini"
#define AUTOSTART 0
//#define MYTH_STREAM_CLOSE