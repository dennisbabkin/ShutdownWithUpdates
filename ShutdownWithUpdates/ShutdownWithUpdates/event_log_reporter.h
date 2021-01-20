/*
 * ShutdownWithUpdates
 * "Utility To Install Pre-Downloaded Windows Updates & Shutdown/Reboot"
 * Copyright (c) 2016-2020 www.dennisbabkin.com
 *
 *     https://dennisbabkin.com/shutdownwithupdates/
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <stdlib.h>
#include <tchar.h>
#include <strsafe.h>


#define EVENT_LOG_APP_NAME PROG_NAME
#define EVENT_LOG_APP_VERSION PROG_VER




struct EVENT_LOG_REPORTS{

	static BOOL ReportEventLogMsgERROR_Spec_WithFormat(int nSpecErrCode, LPCTSTR pDescFormat = L"", ...)
	{
		//Send error message to the event log
		//'nSpecErrCode' = special error code -- see https://dennisbabkin.com/seqidgen
		//INFO: It will automatically add last OS Error code, time when logging was done, and this app version
		//INFO: Value of GetLastError() will be preserved across this call
		//'pDesc' = Description of the message, can contain %s, %d, etc.
		//'args' = variable length arguments for 'pDesc'
		//RETURN:
		//		= TRUE if message was logged
		int nOSErr = ::GetLastError();

		va_list argList;
		va_start(argList, pDescFormat);

		BOOL bRes = __reportEventLogMsgWithFormatV(EVENTLOG_ERROR_TYPE, pDescFormat, nOSErr, argList, TRUE, nSpecErrCode);

		va_end(argList);

		//Restore last error
		::SetLastError(nOSErr);
		return bRes;
	}

	static BOOL ReportEventLogMsgInfo_WithFormat(LPCTSTR pDescFormat, ...)
	{
		//Send info message to the event log
		//INFO: It will automatically add last OS Error code, time when logging was done, and this app version
		//INFO: Value of GetLastError() will be preserved across this call
		//'pDesc' = Description of the message, can contain %s, %d, etc.
		//'args' = variable length arguments for 'pDesc'
		//RETURN:
		//		= TRUE if message was logged
		int nOSErr = ::GetLastError();

		va_list argList;
		va_start(argList, pDescFormat);

		BOOL bRes = __reportEventLogMsgWithFormatV(EVENTLOG_INFORMATION_TYPE, pDescFormat, nOSErr, argList, FALSE, 0);

		va_end(argList);

		//Restore last error
		::SetLastError(nOSErr);
		return bRes;
	}

	static BOOL ReportEventLogMsgWARNING_WithFormat(LPCTSTR pDescFormat, ...)
	{
		//Send warning message to the event log
		//INFO: It will automatically add last OS Error code, time when logging was done, and this app version
		//INFO: Value of GetLastError() will be preserved across this call
		//'pDesc' = Description of the message, can contain %s, %d, etc.
		//'args' = variable length arguments for 'pDesc'
		//RETURN:
		//		= TRUE if message was logged
		int nOSErr = ::GetLastError();

		va_list argList;
		va_start(argList, pDescFormat);

		BOOL bRes = __reportEventLogMsgWithFormatV(EVENTLOG_WARNING_TYPE, pDescFormat, nOSErr, argList, FALSE, 0);

		va_end(argList);

		//Restore last error
		::SetLastError(nOSErr);
		return bRes;
	}

	static BOOL ReportEventLogMsgERROR_WithFormat(LPCTSTR pDescFormat, ...)
	{
		//Send error message to the event log
		//INFO: It will automatically add last OS Error code, time when logging was done, and this app version
		//INFO: Value of GetLastError() will be preserved across this call
		//'pDesc' = Description of the message, can contain %s, %d, etc.
		//'args' = variable length arguments for 'pDesc'
		//RETURN:
		//		= TRUE if message was logged
		int nOSErr = ::GetLastError();

		va_list argList;
		va_start(argList, pDescFormat);

		BOOL bRes = __reportEventLogMsgWithFormatV(EVENTLOG_ERROR_TYPE, pDescFormat, nOSErr, argList, FALSE, 0);

		va_end(argList);

		//Restore last error
		::SetLastError(nOSErr);
		return bRes;
	}

	static BOOL ReportEventLogMsg(UINT nType, LPCTSTR pDesc, SYSTEMTIME* pSystemTime = NULL)
	{
		//Send error/warning/info message to the event log
		//INFO: It will automatically add last OS Error code, time when logging was done, and this app version
		//INFO: Value of GetLastError() will be preserved across this call
		//'nType' = Can be one of the following:
		//			= EVENTLOG_ERROR_TYPE
		//			= EVENTLOG_WARNING_TYPE
		//			= EVENTLOG_INFORMATION_TYPE
		//'pDesc' = Description of the message
		//'pSystemTime' = if not NULL, UTC system time when message was logged
		//RETURN:
		//		= TRUE if message was logged
		int nOSErr = ::GetLastError();

		SYSTEMTIME st;
		if(!pSystemTime)
		{
			::GetLocalTime(&st);
			pSystemTime = &st;
		}

		//See the process calling it
		DWORD dwPid = ::GetCurrentProcessId();
		DWORD dwThId = ::GetCurrentThreadId();

		//Get command line
		LPCTSTR pStrCmdLine = ::GetCommandLineW();

		WCHAR buffErrMsg[1024];
		buffErrMsg[0] = 0;


		BOOL bRes = __reportEventLogMsg(nType,
			L"%04d-%02d-%02dT%02d:%02d:%02d.%03d {%s|%s|%s} (%d) %s\n"
			L"\n"
			L"%s%s"
			L"PID=%u\n"
			L"ThreadID=%u\n"
			L"CmdLn: %s"
			,
			pSystemTime->wYear,
			pSystemTime->wMonth,
			pSystemTime->wDay,
			pSystemTime->wHour,
			pSystemTime->wMinute,
			pSystemTime->wSecond,
			pSystemTime->wMilliseconds,
			EVENT_LOG_APP_NAME,
			EVENT_LOG_APP_VERSION,
#ifdef _M_X64
			L"x64",
#else
			L"x32",
#endif
			nOSErr,
			pDesc ? pDesc : L"",
			nOSErr != ERROR_SUCCESS ? getFormattedErrorMsg(nOSErr, buffErrMsg, _countof(buffErrMsg)) : L"",
			nOSErr != ERROR_SUCCESS ? L"\n" : L"",
			dwPid,
			dwThId,
			pStrCmdLine);

		//Restore last error
		::SetLastError(nOSErr);
		return bRes;
	}


	static const WCHAR* getFormattedErrorMsg(int nOSError, WCHAR* pBuffer, size_t szchBuffer)
	{
		//Format error message as a string
		//'pBuffer' = buffer to fill in with error description
		//'szchBuffer' = size of 'pBuffer' in WCHARs
		//RETURN:
		//		= Pointer to 'pBuffer' (always NULL-terminated)
		int nPrev_OSError = ::GetLastError();

		if(szchBuffer)
		{
			if(nOSError)
			{
				LPVOID lpMsgBuf = NULL;
				DWORD dwRes;

				pBuffer[0] = 0;

				dwRes = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					nOSError,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					(LPTSTR) &lpMsgBuf, 0, NULL);

				if(lpMsgBuf)
				{	
					StringCchCopy(pBuffer, szchBuffer, (LPCTSTR)lpMsgBuf);
					LocalFree(lpMsgBuf);
				}

				//Safety null
				pBuffer[szchBuffer - 1] = 0;
			}
			else
			{
				//No errors
				pBuffer[0] = 0;
			}
		}

		::SetLastError(nPrev_OSError);
		return pBuffer;
	}


private:

	static BOOL __reportEventLogMsgWithFormatV(UINT nType, LPCTSTR pDescFormat, int nOSError, va_list argList, BOOL bAddSpecCode, int nSpecCode)
	{
		//Send error/warning/info message to the event log
		//INFO: It will automatically add last OS Error code, UTC time when logging was done, and this app version
		//INFO: Value of GetLastError() will be preserved across this call
		//'nType' = Can be one of the following:
		//			= EVENTLOG_ERROR_TYPE
		//			= EVENTLOG_WARNING_TYPE
		//			= EVENTLOG_INFORMATION_TYPE
		//'pDesc' = Description of the message, can contain %s, %d, etc.
		//'argList' = variable length arguments for 'pDesc'
		//'bAddSpecCode' = TRUE to add 'nSpecCode' to description
		//'nSpecCode' = special error code
		//RETURN:
		//		= TRUE if message was logged
		BOOL bRes = FALSE;

		if(!pDescFormat)
			pDescFormat = L"";

		HANDLE hHeap = ::GetProcessHeap();
		TCHAR* pBuffDesc = NULL;
		TCHAR* pBuffFmt = NULL;
		HRESULT hr;

		__try
		{
			__try
			{
				//Are we adding a special code?
				if(bAddSpecCode)
				{
					//Get length of formatting string
					UINT nLnDescFmt = 0;
					while(pDescFormat[nLnDescFmt])
						nLnDescFmt++;

					WCHAR buffSpecErr[64];
					buffSpecErr[0] = 0;
					if(FAILED(hr = ::StringCchPrintf(buffSpecErr, _countof(buffSpecErr), L"[%d] ", nSpecCode)))
					{
						//Failed
						::SetLastError(nOSError);
						ReportEventLogMsg(nType, pDescFormat);
#ifdef _DEBUG
						//Debugger assertion
						DebugBreak();
#endif
						__leave;
					}

					buffSpecErr[_countof(buffSpecErr) - 1] = 0;
					UINT nLnBuffSpecErr = 0;
					while(buffSpecErr[nLnBuffSpecErr])
						nLnBuffSpecErr++;

					SIZE_T szchLnBuffFmt = (nLnDescFmt + nLnBuffSpecErr + 1) * sizeof(TCHAR);
					pBuffFmt = (TCHAR*)::HeapAlloc(hHeap, 0, szchLnBuffFmt);
					if(!pBuffFmt)
					{
						//Failed
						::SetLastError(nOSError);
						ReportEventLogMsg(nType, pDescFormat);
#ifdef _DEBUG
						//Debugger assertion
						DebugBreak();
#endif
						__leave;
					}

					//Make sure the amended description
					if(SUCCEEDED(hr = ::StringCchCopy(pBuffFmt, szchLnBuffFmt, buffSpecErr)) &&
						SUCCEEDED(hr = ::StringCchCat(pBuffFmt, szchLnBuffFmt, pDescFormat)))
					{
						//All good, can continue
					}
					else
					{
						//Failed to copy
						::SetLastError(nOSError);
						ReportEventLogMsg(nType, pDescFormat);
#ifdef _DEBUG
						//Debugger assertion
						DebugBreak();
#endif
						__leave;
					}
				}
				else
				{
					//Use as provided
					pBuffFmt = (TCHAR*)pDescFormat;
				}


				//Format strings
				int nLnDesc = _vsctprintf(pBuffFmt, argList);

				pBuffDesc = (TCHAR*)::HeapAlloc(hHeap, 0, (nLnDesc + 1) * sizeof(TCHAR));
				if(pBuffDesc)
				{
					//Format
					vswprintf_s(pBuffDesc, nLnDesc + 1, pBuffFmt, argList);

					//Ensure last NULL
					pBuffDesc[nLnDesc] = 0;

					::SetLastError(nOSError);
					bRes = ReportEventLogMsg(nType, pBuffDesc);
				}
				else
				{
					//Low mem
					::SetLastError(nOSError);
					ReportEventLogMsg(nType, pBuffFmt);

#ifdef _DEBUG
					//Debugger assertion
					DebugBreak();
#endif
				}
			}
			__finally
			{
				//Free mem
				if(pBuffDesc)
				{
					::HeapFree(hHeap, 0, pBuffDesc);
					pBuffDesc = NULL;
				}

				if(bAddSpecCode)
				{
					if(pBuffFmt)
					{
						::HeapFree(hHeap, 0, pBuffFmt);
						pBuffFmt = NULL;
					}
				}
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			//Exception
			::SetLastError(nOSError);
			ReportEventLogMsg(nType, pDescFormat);

#ifdef _DEBUG
			//Debugger assertion
			DebugBreak();
#endif
		}

		return bRes;
	}


	static BOOL __reportEventLogMsg(UINT nType, LPCTSTR pDescFormat, ...)
	{
		//Send service error/warning/info to the event log
		//'nType' = Can be one of the following:
		//			= EVENTLOG_ERROR_TYPE
		//			= EVENTLOG_WARNING_TYPE
		//			= EVENTLOG_INFORMATION_TYPE
		//'pDesc' = Description of the message, can contain %s, %d, etc.
		//RETURN:
		//		= TRUE if message was logged
		if(!pDescFormat)
			pDescFormat = L"";

		va_list argList;
		va_start(argList, pDescFormat);

		BOOL bRes = __reportEventLogMsgV(nType, pDescFormat, argList);

		va_end(argList);

		return bRes;
	}

	static BOOL __reportEventLogMsgV(UINT nType, LPCTSTR pDesc, va_list args)
	{
		//Send service error/warning/info to the event log
		//'nType' = Can be one of the following:
		//			= EVENTLOG_ERROR_TYPE
		//			= EVENTLOG_WARNING_TYPE
		//			= EVENTLOG_INFORMATION_TYPE
		//'pDesc' = Description of the message, can contain %s, %d, etc.
		//'args' = variable length arguments for 'pDesc'
		//RETURN:
		//		= TRUE if message was logged
		BOOL bRes = FALSE;
		HANDLE hEventSource = NULL;
		TCHAR* pBuffDesc = NULL;

		HANDLE hHeap = ::GetProcessHeap();

		__try
		{
			__try
			{
				//Get handle to the event log
				hEventSource = ::RegisterEventSource(NULL, EVENT_LOG_APP_NAME);
				if (!hEventSource)
				{
					//Use 'Application' as a fallback source
					hEventSource = ::RegisterEventSource(NULL, L"Application");
				}

				if(hEventSource)
				{
					if(!pDesc)
						pDesc = L"Msg";

					LPCTSTR pUseBuffDesc = NULL;

					__try
					{
						//Format strings
						int nLnDesc = _vsctprintf(pDesc, args);

						pBuffDesc = (TCHAR*)::HeapAlloc(hHeap, 0, (nLnDesc + 1) * sizeof(TCHAR));
						if(pBuffDesc)
						{
							//Format
							vswprintf_s(pBuffDesc, nLnDesc + 1, pDesc, args);

							//Ensure last NULL
							pBuffDesc[nLnDesc] = 0;

							//Use it
							pUseBuffDesc = pBuffDesc;
						}
						else
						{
							//Mem fault
							pUseBuffDesc = L"##LOW_MEM##";
						}
					}
					__except(1)
					{
						//Formatting failed (use it unformatted)
						pUseBuffDesc = pDesc;
#ifdef _DEBUG
						//Debugger assertion
						DebugBreak();
#endif
					}
					
					#define MSG_COMMAND_ERROR                ((DWORD)0xC0020001L)
					#define MSG_COMMAND_WARNING              ((DWORD)0x80020002L)
					#define MSG_COMMAND_INFO                 ((DWORD)0x40020003L)

					//Get event ID
					//(These IDs were precompiled and are stored in the 'EventLog\EventLogMsgs.mc' file originally)
					//(To compile 'EventLogMsgs.mc' file, run 'compile.bat')
					UINT dwEventID;
					switch(nType)
					{
					case EVENTLOG_WARNING_TYPE:
						dwEventID = MSG_COMMAND_WARNING;
						break;
					case EVENTLOG_INFORMATION_TYPE:
						dwEventID = MSG_COMMAND_INFO;
						break;
					default:
						dwEventID = MSG_COMMAND_ERROR;
						break;
					}

					//Set message in the event log
					if(::ReportEvent(hEventSource, (WORD)nType, 0, dwEventID, NULL,
						1, 0, &pUseBuffDesc, NULL))
					{
						//Done
						bRes = TRUE;
					}

				}
			}
			__finally
			{
				//Free resources
				if(hEventSource)
				{
					//Close event log
					::DeregisterEventSource(hEventSource);
					hEventSource = NULL;
				}

				//Free mem
				if(pBuffDesc)
				{
					::HeapFree(hHeap, 0, pBuffDesc);
					pBuffDesc = NULL;
				}
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			//Exception
#ifdef _DEBUG
			//Debugger assertion
			DebugBreak();
#endif
		}

		return bRes;
	}


};



