// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: reference additional headers your program requires here

#include <Windows.h>
#include <Strsafe.h>

#include <WinINet.h>

#include <new>

#include <wtsapi32.h>
#pragma comment(lib, "Wtsapi32.lib")

#include "AltStreams.h"		//Needed for CheckForXP_SP2_FileBlockAndRemoveIt()
#include "ntddstor.h"		//Needed for CheckForXP_SP2_FileBlockAndRemoveIt()

#include "rtlVersion.h"




#define PROG_NAME L"ShutdownWithUpdates"		//Program name
#define PROG_VER L"1.2.4.0"						//Program version

//#define TEST_POWER_OP			//Uncomment this line to simulate power operations

#include "event_log_reporter.h"


#define SIZEOF(f) (sizeof(f)/sizeof(f[0]))

#ifndef ASSERT
#ifdef _DEBUG
#define ASSERT(f) if(!(f)){char ___bf029[1024]; ::StringCchPrintfA(___bf029, 1024, "Assertion Failed:\n\nFile: %s\nLine: %d", __FILE__, __LINE__); ::MessageBoxA(NULL, ___bf029, "** ASSERTION **", MB_ICONERROR | MB_OK);}
#else
#define ASSERT(f) 
#endif
#endif



