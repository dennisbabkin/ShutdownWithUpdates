/*
 * ShutdownWithUpdates
 * "Utility To Install Pre-Downloaded Windows Updates & Shutdown/Reboot"
 * Copyright (c) 2016-2019 www.dennisbabkin.com
 *
 *     https://dennisbabkin.com/utilities/#ShutdownWithUpdates
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




#include "StdAfx.h"
#include "Main.h"


int CMain::doWork(int argc, _TCHAR* argv[])
{
	//RETURN:
	//		= See _tmain()
	int nRes = ERROR_GEN_FAILURE;

	__try
	{
		nRes = doWork_RAW(argc, argv);
	}
	__except(1)
	{
		//Exception
		nRes = -1;

		_tprintf(L"FATAL ERROR: Contact developers: support@dennisbabkin.com\n");
	}

	return nRes;
}

int CMain::doWork_RAW(int argc, _TCHAR* argv[])
{
	//DO NOT CALL directly
	//RETURN: = See doWork()
	int nResOSErrCode = ERROR_GEN_FAILURE;

	//Check if this file has a "mark of the web" stream attached to it & remove it
	CheckForXP_SP2_FileBlockAndRemoveIt();



	if(argc > 1)
	{
		//Check for arguments provided

		BOOL bDoActions = FALSE;				//TRUE to do power op actions

		ACTIONS_INFO ai;

		//See what version of Windows we're running on
		OSVERSIONINFO osi;
		osi.dwOSVersionInfoSize = sizeof(osi);
		BOOL bGV = RTL_OS_VERSION::GetVersionEx2(&osi);

		//Is it Windows 8 or later?
		BOOL bWin8 = bGV && ((osi.dwMajorVersion == 6 && osi.dwMinorVersion >= 2) || osi.dwMajorVersion > 6);


		//Go through command line arguments
		for(int c = 1; c < argc; c++)
		{
			//Identify a command
			LPCTSTR pStrCmd = argv[c];
			CMD_TYPE cmdType = getCommandType(pStrCmd);

			if(cmdType == CTP_SHOW_HELP)
			{
				//Show help -- and don't do anything else
				bDoActions = FALSE;
				nResOSErrCode = NO_ERROR;

				outputMainLogo();
				ShowHelpInfo();

				break;
			}
			else if(cmdType == CTP_SHUT_DOWN)
			{
				//Shut-down
				if(ai.pwrAction == PWR_OP_NONE)
				{
					bDoActions = TRUE;
					ai.pwrAction = PWR_OP_SHUT_DOWN;
				}
				else
				{
					//Error
					_tprintf(L"ERROR: Power action is already defined before parameter: %s\n", pStrCmd);

					bDoActions = FALSE;
					nResOSErrCode = ERROR_DATATYPE_MISMATCH;
					break;
				}
			}
			else if(cmdType == CTP_REBOOT)
			{
				//Reboot
				if(ai.pwrAction == PWR_OP_NONE)
				{
					bDoActions = TRUE;
					ai.pwrAction = PWR_OP_REBOOT;
				}
				else
				{
					//Error
					_tprintf(L"ERROR: Power action is already defined before parameter: %s\n", pStrCmd);

					bDoActions = FALSE;
					nResOSErrCode = ERROR_DATATYPE_MISMATCH;
					break;
				}
			}
			else if(cmdType == CTP_HYBRID_SHUT_DOWN)
			{
				//Hybrid shut-down

				//Parameter only supported on Windows 8 and later OS
				if(bWin8)
				{
					if(ai.pwrAction == PWR_OP_NONE)
					{
						bDoActions = TRUE;
						ai.pwrAction = PWR_OP_HYBRID_SHUT_DOWN;
					}
					else
					{
						//Error
						_tprintf(L"ERROR: Power action is already defined before parameter: %s\n", pStrCmd);

						bDoActions = FALSE;
						nResOSErrCode = ERROR_DATATYPE_MISMATCH;
						break;
					}
				}
				else
				{
					//Error
					_tprintf(L"ERROR: Parameter not supported on this OS: %s\n", pStrCmd);

					bDoActions = FALSE;
					nResOSErrCode = ERROR_NOT_SUPPORTED;
					break;
				}
			}
			else if(cmdType == CTP_REBOOT_WITH_APP_RESTART)
			{
				//Reboot & restart apps
				if(ai.pwrAction == PWR_OP_NONE)
				{
					bDoActions = TRUE;
					ai.pwrAction = PWR_OP_REBOOT_WITH_APP_RESTART;
				}
				else
				{
					//Error
					_tprintf(L"ERROR: Power action is already defined before parameter: %s\n", pStrCmd);

					bDoActions = FALSE;
					nResOSErrCode = ERROR_DATATYPE_MISMATCH;
					break;
				}
			}
			else if(cmdType == CTP_ADVANCED_BOOT_MENU)
			{
				//Advanced boot options menu

				//Parameter only supported on Windows 8 and later OS
				if(bWin8)
				{
					if(ai.pwrAction == PWR_OP_NONE)
					{
						bDoActions = TRUE;
						ai.pwrAction = PWR_OP_ADVANCED_BOOT_OPTIONS_MENU;
					}
					else
					{
						//Error
						_tprintf(L"ERROR: Power action is already defined before parameter: %s\n", pStrCmd);

						bDoActions = FALSE;
						nResOSErrCode = ERROR_DATATYPE_MISMATCH;
						break;
					}
				}
				else
				{
					//Error
					_tprintf(L"ERROR: Parameter not supported on this OS: %s\n", pStrCmd);

					bDoActions = FALSE;
					nResOSErrCode = ERROR_NOT_SUPPORTED;
					break;
				}
			}
			else if(cmdType == CTP_ABORT_SHUT_DOWN)
			{
				//Abort previous power action
				if(ai.pwrAction == PWR_OP_NONE)
				{
					bDoActions = TRUE;
					ai.pwrAction = PWR_OP_ABORT_SHUT_DOWN;
				}
				else
				{
					//Error
					_tprintf(L"ERROR: Power action is already defined before parameter: %s\n", pStrCmd);

					bDoActions = FALSE;
					nResOSErrCode = ERROR_DATATYPE_MISMATCH;
					break;
				}
			}
			else if(cmdType == CTP_FORCED)
			{
				//Forced
				ai.bForced = TRUE;
			}
			else if(cmdType == CTP_VERBOSE)
			{
				//Verbose
				ai.bVerbose = TRUE;
			}
			else if(cmdType == CTP_NO_UPDATES)
			{
				//No updates
				ai.bNoUpdates = TRUE;
			}
			else if(cmdType == CTP_REMOTE_COMPUTER)
			{
				//Remove computer
				if(c + 1 < argc)
				{
					//Get next param
					ai.pStrRemoteCompName = argv[++c];
				}
				else
				{
					//Error
					_tprintf(L"ERROR: Remote computer name is required for parameter: %s\n", pStrCmd);

					bDoActions = FALSE;
					nResOSErrCode = ERROR_DATATYPE_MISMATCH;
					break;
				}
			}
			else if(cmdType == CTP_TIMEOUT)
			{
				//Timeout
				if(c + 1 < argc)
				{
					//Get next param as timeout value
					int nValTO = 0;
					LPCTSTR pStrParam = argv[++c];
					if(CMain::parseDecimalInt32(pStrParam, &nValTO))
					{
						if(nValTO >= 0)
						{
							//Use it
							ai.nTimeoutSec = nValTO;
						}
						else
						{
							//Error
							_tprintf(L"ERROR: Invalid time-out value (%s) for parameter: %s\n", pStrParam, pStrCmd);

							bDoActions = FALSE;
							nResOSErrCode = ERROR_DATATYPE_MISMATCH;
							break;
						}
					}
					else
					{
						//Error
						_tprintf(L"ERROR: Failed to parse time-out value for parameter: %s\n", pStrCmd);

						bDoActions = FALSE;
						nResOSErrCode = ERROR_DATATYPE_MISMATCH;
						break;
					}
				}
				else
				{
					//Error
					_tprintf(L"ERROR: Time-out value in seconds is required for parameter: %s\n", pStrCmd);

					bDoActions = FALSE;
					nResOSErrCode = ERROR_DATATYPE_MISMATCH;
					break;
				}
			}
			else if(cmdType == CTP_SHOW_MESSAGE)
			{
				//Message
				if(c + 1 < argc)
				{
					//Get next param
					ai.pStrMessage = argv[++c];
				}
				else
				{
					//Error
					_tprintf(L"ERROR: Message text is required for parameter: %s\n", pStrCmd);

					bDoActions = FALSE;
					nResOSErrCode = ERROR_DATATYPE_MISMATCH;
					break;
				}
			}
			else if(cmdType == CTP_REASON)
			{
				//Get reason that follows
				if(c + 1 < argc)
				{
					//Get next param
					LPCTSTR pStrParam = argv[++c];

					if(!CMain::parseReasonParam(pStrParam, &ai.dwReason))
					{
						//Failed to parse
						_tprintf(L"ERROR: Failed to parse shut-down/rebooting reason parameter: %s\n", pStrParam);

						bDoActions = FALSE;
						nResOSErrCode = ERROR_DATATYPE_MISMATCH;
						break;
					}
				}
				else
				{
					//Error
					_tprintf(L"ERROR: Shut-down/rebooting reason value is required for parameter: %s\n", pStrCmd);

					bDoActions = FALSE;
					nResOSErrCode = ERROR_DATATYPE_MISMATCH;
					break;
				}
			}
			else if(cmdType == CTP_NONE)
			{
				//Not a command
				if(pStrCmd[0])
				{
					//Show warning
					_tprintf(L"WARNING: Unexpected argument: %s\n", pStrCmd);
				}
				else
					ASSERT(NULL);
			}
			else if(cmdType == CTP_UNKNOWN)
			{
				//Unknown command
				if(pStrCmd[0])
				{
					//Show warning
					_tprintf(L"WARNING: Unknown command: %s\n", pStrCmd);
				}
				else
					ASSERT(NULL);
			}
			else
			{
				//Error
				ASSERT(NULL);

				_tprintf(L"ERROR: Bad command: %s\n", pStrCmd);

				bDoActions = FALSE;
				nResOSErrCode = ERROR_UNSUPPORTED_TYPE;
				break;
			}
		}


		//Are we doing the power op
		if(bDoActions)
		{
			//Do we need to ask user?
			if(ai.bVerbose)
			{
				//Need to ask by showing UI
				//		= 1 if we're allowed to continue
				//		= 0 if user canceled (and we're not allowed to continue)
				//		= -1 if error (check GetLastError() for info) -- we're not allowed to continue
				int nResUI = ShowUserConfirmation(&ai);
				if(nResUI == 0)
				{
					//User canceled
					bDoActions = FALSE;
					nResOSErrCode = ERROR_CANCELLED;
				}
				else if(nResUI != 1)
				{
					//Error
					bDoActions = FALSE;
					nResOSErrCode = ::GetLastError();
				}
			}

			//Still allowed
			if(bDoActions)
			{
				//Perform actions
				nResOSErrCode = doActions(&ai);

			}
		}
		else
		{
			//Did we get an error?
			if(nResOSErrCode == ERROR_GEN_FAILURE)
			{
				//Not enough parameters
#ifndef ERROR_INVALID_FIELD_IN_PARAMETER_LIST
#define ERROR_INVALID_FIELD_IN_PARAMETER_LIST 328
#endif
				nResOSErrCode = ERROR_INVALID_FIELD_IN_PARAMETER_LIST;
				_tprintf(L"ERROR: Not enough parameters\n");
			}
		}
	}
	else
	{
		//No arguments
		outputMainLogo();

		//Show how to get to help
		_tprintf(L"Use -? to see command line options.\n");

		nResOSErrCode = ERROR_EMPTY;
	}

	return nResOSErrCode;
}


CMD_TYPE CMain::getCommandType(LPCTSTR pStrCmd)
{
	//Determine type of command in 'pStrCmd'
	//RETURN:
	//		= Command type
	CMD_TYPE res = CTP_NONE;

	if(pStrCmd &&
		pStrCmd[0])
	{
		TCHAR z = pStrCmd[0];
		if(z == '/' ||
			z == '\\' ||
			z == '-')
		{
			pStrCmd++;

			//Assume Unidentified
			res = CTP_UNKNOWN;

			static CMD_STR cmds[] = {
				{CTP_SHOW_HELP,		L"?"},
				{CTP_SHOW_HELP,		L"help"},
				{CTP_SHUT_DOWN,		L"s"},
				{CTP_REBOOT,		L"r"},
				{CTP_HYBRID_SHUT_DOWN,		L"hs"},
				{CTP_REBOOT_WITH_APP_RESTART,		L"g"},
				{CTP_ABORT_SHUT_DOWN,		L"a"},
				{CTP_FORCED,		L"f"},
				{CTP_VERBOSE,		L"v"},
				{CTP_NO_UPDATES,		L"nu"},
				{CTP_REMOTE_COMPUTER,		L"m"},
				{CTP_TIMEOUT,		L"t"},
				{CTP_SHOW_MESSAGE,		L"c"},
				{CTP_REASON,		L"d"},
				{CTP_ADVANCED_BOOT_MENU,		L"abo"},
			};

			for(int c = 0; c < SIZEOF(cmds); c++)
			{
				if(::CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, pStrCmd, -1, cmds[c].pStrCmd, -1) == CSTR_EQUAL)
				{
					//Got a match
					res = cmds[c].cmdID;
					break;
				}
			}

		}
	}

	return res;
}

int CMain::CheckForXP_SP2_FileBlockAndRemoveIt()
{
	//Checks if the "mark of the web" stream is on the 'this' file & removes it if so
	//(It will be there if file was downloaded from the Internet, or from another computer)
	//RETURN:
	//		= 0 if not such block was found
	//		= 1 if block was found (and removed if 'bRemoveIt' == TRUE)
	//		= 2 if block was found, but COULD NOT remove it (if 'bRemoveIt' == TRUE)
	//		= -1 if error
	//		= -2 if not supported

	BOOL bRemoveIt = TRUE;

	//No, get current file
	TCHAR strFilePath[MAX_PATH * 4];
	strFilePath[0] = 0;
	if(!::GetModuleFileName(NULL, strFilePath, SIZEOF(strFilePath)))
		return -1;

	strFilePath[SIZEOF(strFilePath) - 1] = 0;


	//Load API needed for the process
	NTQUERYINFORMATIONFILE pfnNtQueryInformationFile;
    (FARPROC&)pfnNtQueryInformationFile = ::GetProcAddress(::GetModuleHandle(_T("ntdll.dll")), "NtQueryInformationFile");
	if(!pfnNtQueryInformationFile)
		return -2;

    //Obtain SE_BACKUP_NAME privilege (required for opening a directory)
    HANDLE hToken = NULL;
    TOKEN_PRIVILEGES tp;
	if(OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		if(LookupPrivilegeValue(NULL, SE_BACKUP_NAME, &tp.Privileges[0].Luid))
		{
			tp.PrivilegeCount = 1;
			tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if(AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
			{
				//All done
			}
		}
    }
    if(hToken)
		::CloseHandle(hToken);

	//Now open the file
	HANDLE hFile = ::CreateFile(strFilePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, 
		OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return -1;

	//Get stream information block (assume memory size in the beginning)
    LPBYTE pInfoBlock = NULL;
    ULONG uInfoBlockSize = 0;
	IO_STATUS_BLOCK ioStatus;
	NTSTATUS status;
	do 
	{
		uInfoBlockSize += 16 * 1024;
		if(pInfoBlock)
			delete [] pInfoBlock;

		pInfoBlock = new BYTE [uInfoBlockSize];
		((PFILE_STREAM_INFORMATION)pInfoBlock)->StreamNameLength = 0;
		status = pfnNtQueryInformationFile(hFile, &ioStatus, (LPVOID)pInfoBlock, uInfoBlockSize, FileStreamInformation);
	}
	while(status == STATUS_BUFFER_OVERFLOW);

	//Close file handle 
	::CloseHandle(hFile);

	//Go through stream info and look for our stream
	int nRet = 0;
    WCHAR wszStreamName[MAX_PATH];
	TCHAR szStreamName[MAX_PATH];
	PFILE_STREAM_INFORMATION pStreamInfo = (PFILE_STREAM_INFORMATION)(LPVOID)pInfoBlock;
    for(;;) 
	{
		// Check if stream info block is empty (directory may have no stream)
		if(pStreamInfo->StreamNameLength == 0)
			break;

		// Get stream name
		memcpy(wszStreamName, pStreamInfo->StreamName, pStreamInfo->StreamNameLength);
		wszStreamName[pStreamInfo->StreamNameLength / sizeof(WCHAR)] = L'\0';

		// Remove attribute tag and convert to char
		LPWSTR pTag = wcsstr(wszStreamName, L":$DATA");
		if (pTag)
			*pTag = L'\0';
		::StringCchCopy(szStreamName, SIZEOF(szStreamName), (const TCHAR*)wszStreamName);

		//Is it the name we're looking for?
		if (lstrcmpi(szStreamName, _T(":Zone.Identifier")) == 0) 
		{
			//Our Named stream?
			nRet = 1;

			//Do we need to remove it?
			if(!bRemoveIt)
				break;

			//Make file name for the stream
			TCHAR strStreamFileName[MAX_PATH * 4];
			strStreamFileName[0] = 0;
			::StringCchCopy(strStreamFileName, SIZEOF(strStreamFileName), strFilePath);
			::StringCchCat(strStreamFileName, SIZEOF(strStreamFileName), szStreamName);
			strStreamFileName[SIZEOF(strStreamFileName) - 1] = 0;

			//Delete this stream
			hFile = ::CreateFile(strStreamFileName, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, 
				OPEN_EXISTING, FILE_FLAG_DELETE_ON_CLOSE, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				//Erased it
				if(!::CloseHandle(hFile))
				{
					//Error removing
					nRet = 2;
				}
			}
			else
			{
				//Error removing
				nRet = 2;
			}
		}

		//Do we have more stream records?
		if(pStreamInfo->NextEntryOffset == 0)
			break;

		//Go to the next one
		pStreamInfo = (PFILE_STREAM_INFORMATION)((LPBYTE)pStreamInfo + pStreamInfo->NextEntryOffset);
	}

	//Free memory
	if(pInfoBlock)
		delete [] pInfoBlock;

	return nRet;
}



void CMain::ShowHelpInfo()
{
	//Show help information

	_tprintf(
		L"Usage: ShutdownWithUpdates [/s | /r | /hs | /g | /a | /?] [/f] [/v] [/nu]\n"
		L"        [/m \\\\computer] [/t x] [/c \"msg\"] [/d [p|u:]xx:yy]\n"
		L"\n"
		L"  /s    Install updates & shut down computer.\n"
		L"         (Updates must be already downloaded on computer being shut down.)\n"
		L"  /r    Install updates & reboot computer.\n"
		L"         (Updates must be already downloaded on computer being rebooted.)\n"
		L"  /hs   Install updates & initiate hybrid shut-down of computer. (Windows 8,10)\n"
		L"         (Updates must be already downloaded on computer being shut down.)\n"
		L"  /g    Install updates & reboot computer & restart registered applications.\n"
		L"         (Updates must be already downloaded on computer being rebooted.)\n"
		L"  /abo  Go to advanced boot options menu. (Windows 8,10)\n"
		L"         (Pre-Windows 10: Updates will not be installed.)\n"
		L"  /a    Abort previous shut-down/rebooting.\n"
		L"         (Can be used only during previous time-out period.)\n"
		L"  /?    Show command line help.\n"
		L"  /f    Use forced action.\n"
		L"         WARNING: May result in the loss of unsaved data on target computer!\n"
		L"  /v    Show user confirmation before proceeding.\n"
		L"         (Local computer only. It is shown before time-out is initiated.)\n"
		L"  /nu   Not to install updates.\n"
		L"         (Windows 10: This option is not supported.)\n"
		L"  /m \\\\computer    Specify target/remote computer.\n"
		L"  /t x  Set time-out before performing action to x seconds.\n"
		L"         (Valid range is 0-%d, or 10 yrs, with a default of 0.)\n"
		L"  /c \"msg\"      Message to be displayed in the interactive shutdown dialog box.\n"
		L"                 (Maximum of 512 characters is allowed.)\n"
		L"  /d [p|u:]xx:yy  Reason for shut-down or rebooting (used for logging):\n"
		L"                   p if action was planned.\n"
		L"                   u if action was user-defined.\n"
		L"                   (If neither p or u is used, assumes unplanned.)\n"
		L"                   xx = major reason number (less than %d.)\n"
		L"                   yy = minor reason number (greater than %d.)\n"
		L"                        (Reason numbers can be decimal or hex if begin with 0x)\n"
		L"        For major and minor reason values check \"System Shutdown Reason Codes\":\n"
		L"         msdn.microsoft.com/en-us/library/windows/desktop/aa376885(v=vs.85).aspx\n"
		L"\n"
		L"Exit Codes:\n"
		L" 0      if success.\n"
		L" -1     if general failure in the module.\n"
		L" Other  if error, will contain \"System Error Code\". For details check:\n"
		L"         msdn.microsoft.com/en-us/library/windows/desktop/ms681381(v=vs.85).aspx\n"
		L"\n"
		L"Examples:\n"
		L"(1) Install updates and reboot local computer without a delay:\n"
		L"    (Fail if unsaved user data on computer.)\n"
		L"\n"
		L"      ShutdownWithUpdates /r\n"
		L"\n"
		L"(2) Install updates and shut down local computer after 30 sec delay:\n"
		L"    (Force applications with unsaved data to close & lose data! Show message.)\n"
		L"\n"
		L"      ShutdownWithUpdates /s /f /t 30 /c \"Forced shut-down in 30 sec!\"\n"
		L"\n"
		L"(3) Do not install updates and reboot remote computer after a 20 sec delay:\n"
		L"    (Not supported under Windows 10.)\n"
		L"    (Fail if unsaved user data on remote computer.)\n"
		L"    (Specify reason as planned, application issue, installation.)\n"
		L"\n"
		L"      ShutdownWithUpdates /r /nu /m \\\\MYSERVER /t 20 /d p:0x00040000:0x00000002\n"
		L"\n"
		L"\n"
		L"** This is an Open Source project. **\n"
		L"   You can download its source code at:\n"
		L"    https://github.com/dennisbabkin/ShutdownWithUpdates\n"
		L"\n"
		,

		MAX_SHUTDOWN_TIMEOUT,
		0x10000,
		0x10000
		);
	
}


int CMain::findCharInStrCaseSensitive(LPCTSTR pStr, TCHAR chSearch, int nIndBegin)
{
	//'nIndBegin' = 0-based index in 'pStr' to begin search from
	//RETURN:
	//		= Index of 'chSearch' in 'pStr'
	//		= -1 if none
	int nInd = -1;

	if(pStr)
	{
		for(int i = nIndBegin;; i++)
		{
			TCHAR z = pStr[i];
			if(z == 0)
			{
				//End of string
				break;
			}
			else if(z == chSearch)
			{
				//Got it
				nInd = i;
				break;
			}
		}
	}

	return nInd;
}


BOOL CMain::parseReasonInt32(LPCTSTR pStr, int* pnOutVal)
{
	//'pStr' = can be signed 32-bit integer, as decimal or hex
	//'pnOutVal' = if not NULL, Value read
	//RETURN:
	//		= TRUE if read OK
	BOOL bRes = FALSE;

	int nV = 0;

	//First assume hex
	UINT nValHex = 0;
	if(parseHexUInt32(pStr, &nValHex))
	{
		//Got it
		nV = nValHex;
		bRes = TRUE;
	}
	else
	{
		//Parse it as decimal
		int nValDecimal = 0;
		if(parseDecimalInt32(pStr, &nValDecimal))
		{
			//Got it
			nV = nValDecimal;
			bRes = TRUE;
		}
	}

	if(pnOutVal)
		*pnOutVal = nV;

	return bRes;
}

BOOL CMain::parseDecimalInt32(LPCTSTR pStr, int* pnOutVal)
{
	//'pStr' = can be signed 32-bit integer
	//'pnOutVal' = if not NULL, Value read
	//RETURN:
	//		= TRUE if read OK
	BOOL bRes = FALSE;

	int nV = 0;

	if(pStr &&
		pStr[0])
	{
		int i = 0;

		//Check for sign first
		TCHAR chSign = pStr[0];
		if(chSign == '+' ||
			chSign == '-')
		{
			i++;
		}

		//Flag if we found at least one digit
		BOOL bDigitFound = FALSE;

		//See if it's valid
		for(;; i++)
		{
			TCHAR z = pStr[i];
			if(z == 0)
			{
				//End of line
				if(bDigitFound)
				{
					//Scan it
					int val = 0;
					if(_stscanf_s(pStr, L"%d", &val) == 1)
					{
						//Done
						nV = val;

						bRes = TRUE;
					}
				}

				break;
			}
			else if(z >= '0' && z <= '9')
			{
				//Found a digit
				bDigitFound = TRUE;
			}
			else
			{
				//Something else -- wrong char
				break;
			}
		}
	}

	if(pnOutVal)
		*pnOutVal = nV;

	return bRes;
}


BOOL CMain::parseHexUInt32(LPCTSTR pStr, UINT* pnOutVal)
{
	//'pStr' = can be unsigned 32-bit hex integer
	//'pnOutVal' = if not NULL, Value read
	//RETURN:
	//		= TRUE if read OK
	BOOL bRes = FALSE;

	UINT nV = 0;

	if(pStr &&
		pStr[0])
	{
		//Must begin with 0x
		if(pStr[0] == '0')
		{
			if(pStr[1] == 'x')
			{
				//Flag if we found at least one digit
				BOOL bDigitFound = FALSE;

				//See if it's valid
				for(int i = 2;; i++)
				{
					TCHAR z = pStr[i];
					if(z == 0)
					{
						//End of line
						if(bDigitFound)
						{
							//Scan it
							int val = 0;
							if(_stscanf_s(pStr, L"%x", &val) == 1)
							{
								//Done
								nV = val;

								bRes = TRUE;
							}
						}

						break;
					}
					else if((z >= '0' && z <= '9') ||
						(z >= 'a' && z <= 'f') ||
						(z >= 'A' && z <= 'F'))
					{
						//Found a digit
						bDigitFound = TRUE;
					}
					else
					{
						//Something else -- wrong char
						break;
					}
				}
			}
		}
	}

	if(pnOutVal)
		*pnOutVal = nV;

	return bRes;
}

BOOL CMain::parseReasonParam(LPCTSTR pStrParam, DWORD* pnOutReason)
{
	//Parse reason parameter from 'pStrParam'
	//'pnOutReason' = if not NULL, receives the reason if success
	//RETURN:
	//		= TRUE if success
	//		= FALSE if failed to parse
	BOOL bRes = FALSE;

	DWORD dwReason = 0;

	//[p|u:]xx:yy
	if(pStrParam &&
		pStrParam[0])
	{
		//Split into segments by :
		int nFnd0 = findCharInStrCaseSensitive(pStrParam, L':', 0);
		if(nFnd0 >= 0)
		{
			TCHAR* pBuff0 = NULL;
			TCHAR* pBuff1 = NULL;

			int nFnd1 = findCharInStrCaseSensitive(pStrParam, L':', nFnd0 + 1);
			if(nFnd1 >= 0)
			{
				int nFnd2 = findCharInStrCaseSensitive(pStrParam, L':', nFnd1 + 1);
				if(nFnd2 < 0)
				{
					//[p|u:]xx:yy
					if(nFnd0 == 1)
					{
						TCHAR chPU = pStrParam[0];

						int nchLn0 = nFnd1 - nFnd0;
						pBuff0 = new (std::nothrow) TCHAR[nchLn0];
						if(pBuff0)
						{
							memcpy(pBuff0, pStrParam + nFnd0 + 1, (nchLn0 - 1) * sizeof(TCHAR));
							pBuff0[nchLn0 - 1] = 0;

							int nchLn1 = lstrlen(pStrParam) - nFnd1;
							pBuff1 = new (std::nothrow) TCHAR[nchLn1];
							if(pBuff1)
							{
								memcpy(pBuff1, pStrParam + nFnd1 + 1, (nchLn1 - 1) * sizeof(TCHAR));
								pBuff1[nchLn1 - 1] = 0;

								//Parse values
								UINT uiMajor = 0;
								if(parseReasonInt32(pBuff0, (int*)&uiMajor))
								{
									UINT uiMinor = 0;
									if(parseReasonInt32(pBuff1, (int*)&uiMinor))
									{
										//Check reason itself
										if(chPU == 'p' ||
											chPU == 'P')
										{
											//Planned
											dwReason = SHTDN_REASON_FLAG_PLANNED;

											goto lbl_use_rsn;
										}
										else if(chPU == 'u' ||
											chPU == 'U')
										{
											//User-defined
											dwReason = SHTDN_REASON_FLAG_USER_DEFINED;
lbl_use_rsn:
											//Use it
											dwReason |= (uiMajor & 0xFFFF0000) | (uiMinor & 0xFFFF);

											bRes = TRUE;
										}
									}
								}
							}
						}
					}
				}
			}
			else
			{
				//xx:yy
				pBuff0 = new (std::nothrow) TCHAR[nFnd0 + 1];
				if(pBuff0)
				{
					memcpy(pBuff0, pStrParam, nFnd0 * sizeof(TCHAR));
					pBuff0[nFnd0] = 0;

					int nchLn1 = lstrlen(pStrParam) - nFnd0;
					pBuff1 = new (std::nothrow) TCHAR[nchLn1];
					if(pBuff1)
					{
						memcpy(pBuff1, pStrParam + nFnd0 + 1, (nchLn1 - 1) * sizeof(TCHAR));
						pBuff1[nchLn1 - 1] = 0;

						//Parse two digits
						UINT uiMajor = 0;
						if(parseReasonInt32(pBuff0, (int*)&uiMajor))
						{
							UINT uiMinor = 0;
							if(parseReasonInt32(pBuff1, (int*)&uiMinor))
							{
								//Got both
								dwReason = (uiMajor & 0xFFFF0000) | (uiMinor & 0xFFFF);

								bRes = TRUE;
							}
						}
					}

				}
			}


			//Free mem
			if(pBuff0)
			{
				delete[] pBuff0;
				pBuff0 = NULL;
			}

			if(pBuff1)
			{
				delete[] pBuff1;
				pBuff1 = NULL;
			}


		}
	}

	if(pnOutReason)
		*pnOutReason = dwReason;

	return bRes;
}


TCHAR* CMain::CapitalizeFirstLetter(TCHAR* pBuff)
{
	//Capitalize first letter in 'pBuff'
	//RETURN:
	//		= Pointer to the buffer in 'pBuff'

	if(pBuff &&
		pBuff[0])
	{
		TCHAR buff[2] = {pBuff[0], 0};
		::CharUpper(buff);

		pBuff[0] = buff[0];
	}

	return pBuff;
}


BOOL CMain::FormatSecSmart(TCHAR* pBuff, int nchBuffSz, int nSeconds)
{
	//Format 'nSeconds' into days:hours:minutes:seconds
	//'pBuff' = receives formatted string
	//'nchBuffSz' = size of 'pBuff' in TCHARs
	//'nSeconds' = number of seconds, can be negative
	//RETURN:
	//		= TRUE if success
	BOOL bRes = FALSE;

	if(pBuff &&
		nchBuffSz > 0)
	{
		BOOL bNegative = FALSE;
		pBuff[0] = 0;

		//Is it negative?
		if(nSeconds < 0)
		{
			nSeconds = -nSeconds;
			bNegative = TRUE;

			if(FAILED(::StringCchCat(pBuff, nchBuffSz, L"-")))
				return FALSE;
		}
		
		int n_Secs = nSeconds % 60;

		nSeconds /= 60;
		int n_Mins = nSeconds % 60;

		nSeconds /= 60;
		int n_Hrs = nSeconds % 24;

		nSeconds /= 24;
		int ii_Days = nSeconds;

		//Format it
		if(ii_Days != 0)
		{
			int nLn = lstrlen(pBuff);
			if(FAILED(::StringCchPrintf(pBuff + nLn, nchBuffSz - nLn, L"%d%s", ii_Days, LOC_STRING(IDS_STRING118))))
				return FALSE;
		}

		if(n_Hrs != 0 || pBuff[0])
		{
			int nLn = lstrlen(pBuff);
			if(FAILED(::StringCchPrintf(pBuff + nLn, nchBuffSz - nLn, L"%s%d%s", !pBuff[0] ? L"" : L":", n_Hrs, LOC_STRING(IDS_STRING117))))
				return FALSE;
		}

		if(n_Mins != 0 || pBuff[0])
		{
			int nLn = lstrlen(pBuff);
			if(FAILED(::StringCchPrintf(pBuff + nLn, nchBuffSz - nLn, L"%s%d%s", !pBuff[0] ? L"" : L":", n_Mins, LOC_STRING(IDS_STRING116))))
				return FALSE;
		}

		if(n_Secs != 0 || pBuff[0])
		{
			int nLn = lstrlen(pBuff);
			if(FAILED(::StringCchPrintf(pBuff + nLn, nchBuffSz - nLn, L"%s%d%s", !pBuff[0] ? L"" : L":", n_Secs, LOC_STRING(IDS_STRING110))))
				return FALSE;
		}

		if(!pBuff[0])
		{
			//If nothing there
			if(FAILED(::StringCchPrintf(pBuff, nchBuffSz, L"0%s", LOC_STRING(IDS_STRING110))))
				return FALSE;
		}

		//Done
		bRes = TRUE;
	}

	return bRes;
}


int CMain::ShowUserConfirmation(ACTIONS_INFO* pAI)
{
	//Request user confirmation for action in 'pAI'
	//RETURN:
	//		= 1 if we're allowed to continue
	//		= 0 if user canceled (and we're not allowed to continue)
	//		= -1 if error (check GetLastError() for info) -- we're not allowed to continue
	int nRes = -1;
	int nOSError = NO_ERROR;

	if(pAI)
	{
		//Compose message
		TCHAR buffMsg[1024 * 2 + 512] = {0};

		//Power op
		TCHAR buffPowerOp[1024] = {0};

		//Are we aborting?
		BOOL bAborting = pAI->pwrAction == PWR_OP_ABORT_SHUT_DOWN;
		BOOL bAdvBootMenu = pAI->pwrAction == PWR_OP_ADVANCED_BOOT_OPTIONS_MENU;

		//Are we installing updates?
		if(!pAI->bNoUpdates &&
			!bAborting &&
			!bAdvBootMenu)
		{
			::StringCchPrintf(buffPowerOp, SIZEOF(buffPowerOp), L"%s, ", LOC_STRING(IDS_STRING101));		//L"install downloaded updates"
		}


		switch(pAI->pwrAction)
		{
		case PWR_OP_SHUT_DOWN:
			::StringCchCat(buffPowerOp, SIZEOF(buffPowerOp), LOC_STRING(IDS_STRING102));		//L"shut down computer"
			break;
		case PWR_OP_REBOOT:
			::StringCchCat(buffPowerOp, SIZEOF(buffPowerOp), LOC_STRING(IDS_STRING103));		//L"reboot computer"
			break;
		case PWR_OP_HYBRID_SHUT_DOWN:
			::StringCchCat(buffPowerOp, SIZEOF(buffPowerOp), LOC_STRING(IDS_STRING104));		//L"hybrid shut-down of computer"
			break;
		case PWR_OP_REBOOT_WITH_APP_RESTART:
			::StringCchCat(buffPowerOp, SIZEOF(buffPowerOp), LOC_STRING(IDS_STRING105));		//L"reboot & restart registered apps"
			break;

		case PWR_OP_ADVANCED_BOOT_OPTIONS_MENU:
			::StringCchCopy(buffPowerOp, SIZEOF(buffPowerOp), LOC_STRING(IDS_STRING119));		//L"go to advanced boot options menu"
			break;

		case PWR_OP_ABORT_SHUT_DOWN:
			::StringCchCopy(buffPowerOp, SIZEOF(buffPowerOp), LOC_STRING(IDS_STRING106));		//L"abort pending shut-down or rebooting"
			break;

		default:
			ASSERT(NULL);
			buffPowerOp[0] = 0;
			break;
		}

		if(buffPowerOp[0])
		{
			//Is it forced
			TCHAR buffForced[256] = {0};
			if(!bAborting)
			{
				if(pAI->bForced)
				{
					::StringCchPrintf(buffForced, SIZEOF(buffForced), L" (%s)", LOC_STRING(IDS_STRING107));		//L"forced"
				}
			}

			//Computer
			TCHAR buffComp[256] = {0};
			if(pAI->pStrRemoteCompName &&
				pAI->pStrRemoteCompName[0])
			{
				//Use name
				::StringCchCopy(buffComp, SIZEOF(buffComp), pAI->pStrRemoteCompName);
			}
			else
			{
				//Local
				::StringCchCopy(buffComp, SIZEOF(buffComp), CapitalizeFirstLetter(LOC_STRING(IDS_STRING108)));		//L"local"
			}

			//Time-out
			TCHAR buffTimeout[256] = {0};
			if(!bAborting)
			{
				if(pAI->nTimeoutSec > 0)
				{
					//Set timeout
					TCHAR buffTO_val[256];
					if(FormatSecSmart(buffTO_val, SIZEOF(buffTO_val), pAI->nTimeoutSec))
					{
						::StringCchPrintf(buffTimeout, SIZEOF(buffTimeout), L"\n%s: %s", 
							CapitalizeFirstLetter(LOC_STRING(IDS_STRING109)),		//L"timeout"
							buffTO_val
							);
					}
					else
					{
						//Failed -- use just seconds
						ASSERT(NULL);
						::StringCchPrintf(buffTimeout, SIZEOF(buffTimeout), L"\n%s: %d %s", 
							CapitalizeFirstLetter(LOC_STRING(IDS_STRING109)),		//L"timeout"
							pAI->nTimeoutSec,
							LOC_STRING(IDS_STRING110)		//L"sec"
							);
					}
				}
				else
				{
					//No timeout
					::StringCchPrintf(buffTimeout, SIZEOF(buffTimeout), L"\n%s: %s", 
						CapitalizeFirstLetter(LOC_STRING(IDS_STRING109)),		//L"timeout"
						CapitalizeFirstLetter(LOC_STRING(IDS_STRING111))		//L"none"
						);
				}
			}

			//Message to display
			TCHAR buffUserMsg[512] = {0};
			if(!bAborting)
			{
				if(pAI->pStrMessage &&
					pAI->pStrMessage[0])
				{
					::StringCchPrintf(buffUserMsg, SIZEOF(buffUserMsg), L"\n%s: %s"
						, 
						CapitalizeFirstLetter(LOC_STRING(IDS_STRING112)),		//L"User Message"
						pAI->pStrMessage);
				}
			}


			//Make main message
			::StringCchPrintf(buffMsg, SIZEOF(buffMsg), 
				L"%s\n\n"
				L"\"%s%s\"\n\n"
				L"%s: %s"
				L"%s"
				L"%s"
				,
				LOC_STRING(IDS_STRING113),		//Do you want to perform the following operation?
				CapitalizeFirstLetter(buffPowerOp), buffForced,
				CapitalizeFirstLetter(LOC_STRING(IDS_STRING114)),		//Computer
				buffComp,
				buffTimeout,
				buffUserMsg
				);

			//Show UI
			::SetLastError(NO_ERROR);
			int nResMB = ::MessageBox(NULL, buffMsg,
				LOC_STRING(IDS_STRING115),
				MB_YESNOCANCEL | MB_DEFBUTTON2 | MB_ICONQUESTION | MB_SYSTEMMODAL | MB_SETFOREGROUND);

			if(nResMB == IDYES)
			{
				//User chose to continue
				nRes = 1;
			}
			else if(nResMB == IDNO ||
				nResMB == IDCANCEL)
			{
				//User chose not to continue
				nRes = 0;
			}
			else
			{
				//Error
				nOSError = ::GetLastError();
				if(nOSError == NO_ERROR)
					nOSError = ERROR_INVALID_LEVEL;
			}
		}
		else
		{
			//Error
			nOSError = ERROR_INVALID_PARAMETER;
		}
	}
	else
		nOSError = ERROR_INVALID_PARAMETER;

	::SetLastError(nOSError);
	return nRes;
}


BOOL CMain::AdjustPrivilege(LPCTSTR pStrMachine, LPCTSTR pPrivilegeName, BOOL bEnable, HANDLE hProcess)
{
	//Tries to adjust the 'pPrivilegeName' privilege for the process
	//'pStrMachine' = computer name, or NULL for local computer
	//'bEnable' = TRUE to enable, FALSE to disable a privilege
	//'hProcess' = Process to adjust privilege for, or NULL for current process
	//RETURN: - TRUE if done;
	//		  - FALSE if privileges were not adjusted (check GetLastError() for info)
	BOOL bRes = FALSE;
	int nOSError = NO_ERROR;

	HANDLE hToken; 
	TOKEN_PRIVILEGES tkp; 

	//Get a token for this process. 
	if(!OpenProcessToken(hProcess ? hProcess : GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return FALSE; 

	//Get the LUID for the shutdown privilege. 
	if(LookupPrivilegeValue(pStrMachine, pPrivilegeName, &tkp.Privileges[0].Luid))
	{
		//One privilege to set
		tkp.PrivilegeCount = 1;  
		tkp.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : SE_PRIVILEGE_REMOVED; 

		//Adjust it now
		bRes = AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
		nOSError = GetLastError();
		if(bRes)
		{
			//See if no error
			if(nOSError != ERROR_SUCCESS)
				bRes = FALSE;
		}
	}
	else
	{
		//Failed
		nOSError = ::GetLastError();
	}

	//Close handle
	CloseHandle(hToken);

	::SetLastError(nOSError);
	return bRes;
}


TCHAR* CMain::trimBuffer(TCHAR* pBuff)
{
	//Remove spaces from left and right side of 'pBuff'
	//RETURN:
	//		= Pointer to the buffer from 'pBuff'

	//Do left & then right
	return trimBufferRight(trimBufferLeft(pBuff));
}

TCHAR* CMain::trimBufferLeft(TCHAR* pBuff)
{
	//Remove spaces from left side of 'pBuff'
	//RETURN:
	//		= Pointer to the buffer from 'pBuff'
	if(pBuff)
	{
		for(int i = 0;; i++)
		{
			TCHAR z = pBuff[i];
			if(z == ' ' ||
				z == '\t' ||
				z == '\n' ||
				z == '\r')
			{
			}
			else
			{
				//See if we need to remove it
				if(i > 0)
				{
					//Find the end of string
					int nEndInd = i;
					for(;; nEndInd++)
					{
						if(pBuff[nEndInd] == 0)
							break;
					}

					memcpy(pBuff, pBuff + i, (nEndInd - i + 1) * sizeof(TCHAR));
				}

				break;
			}
		}
	}

	return pBuff;
}

TCHAR* CMain::trimBufferRight(TCHAR* pBuff)
{
	//Remove spaces from right side of 'pBuff'
	//RETURN:
	//		= Pointer to the buffer from 'pBuff'
	if(pBuff)
	{
		//Find the end of string
		int nLn = 0;
		while(pBuff[nLn])
		{
			nLn++;
		}

		for(int i = nLn - 1; i >= 0; i--)
		{
			TCHAR z = pBuff[i];
			if(z == ' ' ||
				z == '\t' ||
				z == '\n' ||
				z == '\r')
			{
			}
			else
			{
				//See if we need to remove it
				if(i < nLn - 1)
				{
					pBuff[i + 1] = 0;
				}

				break;
			}
		}
	}

	return pBuff;
}


TCHAR* CMain::FormatErrorMessage(int nOSError, TCHAR* pBuff, int nchLnBuff, BOOL bUseDescriptionForNoError)
{
	//Convert OS error code to string
	//'nOSError' = OS error code to format
	//'pBuff' = text buffer to use for formatting
	//'nchLnBuff' = length of 'pBuff' in TCHARs
	//'bUseDescriptionForNoError' = TRUE to use system provided description for NO_ERROR (that is "Operation completed successfully")
	//							  = FALSE to use empty string for NO_ERROR
	//RETURN: = Error description buffer in 'pBuff'
	int nPrev_OSError = ::GetLastError();
	LPVOID lpMsgBuf = NULL;
	DWORD dwRes;

	if(pBuff &&
		nchLnBuff > 0)
	{
		//Reset buffer
		pBuff[0] = 0;

		if(nOSError != NO_ERROR ||
			bUseDescriptionForNoError)
		{
			//Convert error code
			if(nOSError >= INTERNET_ERROR_BASE && nOSError <= INTERNET_ERROR_LAST)
			{
				//Get system folder
				TCHAR buffSysFolder[MAX_PATH * 2];
				buffSysFolder[0] = 0;
				if(int nLn = ::GetSystemDirectory(buffSysFolder, SIZEOF(buffSysFolder)))
				{
					//Make sure folder ends with slash
					if(buffSysFolder[nLn - 1] != '/' &&
						buffSysFolder[nLn - 1] != '\\')
					{
						::StringCchCat(buffSysFolder, SIZEOF(buffSysFolder), L"\\");
					}

					::StringCchCat(buffSysFolder, SIZEOF(buffSysFolder), L"wininet.dll");
				}

				if(buffSysFolder[0])
				{
					//Spec code
					HMODULE hMod = LoadLibrary(buffSysFolder);
					dwRes = ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS,
						hMod, nOSError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL);
					FreeLibrary(hMod);
				}
			}
			else
			{
				//Regular
				dwRes = ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					nOSError,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					(LPTSTR) &lpMsgBuf, 0, NULL);
			}

			if(lpMsgBuf)
			{
				//Copy data to our buffer
				::StringCchCopy(pBuff, nchLnBuff, (LPCTSTR)lpMsgBuf);

				//Free mem
				LocalFree(lpMsgBuf);
			}

			//Did we get something?
			trimBuffer(pBuff);
			if(!pBuff[0])
			{
				//If we get nothing, then use just the code
				::StringCchPrintf(pBuff, nchLnBuff, L"Error %d", nOSError);
			}
		}
	}

	::SetLastError(nPrev_OSError);
	return pBuff;
}




BOOL CMain::SetNeededShutdownPrivileges(LPCTSTR pStrRemoteCompName)
{
	//Set privileges needed for a shut-down
	//RETURN:
	//		= TRUE if success
	BOOL bRes = TRUE;

	//Set SE_DEBUG_NAME privilege  
	if(!AdjustPrivilege(NULL, L"SeShutdownPrivilege", TRUE))
	{
		//Error
		//int nErr = ::GetLastError();
		//TCHAR buffErr[1024];

		//_tprintf(L"WARNING: Failed to set shut-down privilege: (%d) %s\n"
		//	,
		//	nErr,
		//	FormatErrorMessage(nErr, buffErr, SIZEOF(buffErr))
		//	);

		bRes = FALSE;
	}

	//Is it a remove computer?
	if(pStrRemoteCompName &&
		pStrRemoteCompName[0])
	{
		//Set privilege  
		//INFO: You can grant SeRemoteShutdownPrivilege through secpol.msc > Local Policies > User Rights Assignment,
		//      by editing the "Force shutdown from remote system" entry.
		//
		if(!AdjustPrivilege(NULL /*pStrRemoteCompName*/, L"SeRemoteShutdownPrivilege", TRUE))
		{
			//Error
			//int nErr = ::GetLastError();
			//TCHAR buffErr[1024];

			//_tprintf(L"WARNING: Failed to set shut-down privilege for computer: \"%s\", (%d) %s\n"
			//	,
			//	pStrRemoteCompName,
			//	nErr,
			//	FormatErrorMessage(nErr, buffErr, SIZEOF(buffErr))
			//	);

			bRes = FALSE;
		}

	}

	return bRes;
}




int CMain::doActions(ACTIONS_INFO* pAI)
{
	//Perform power actions in 'pAI'
	//RETURN:
	//		= Value to return _tmain()
	int nResOSErrCode = ERROR_INTERNAL_ERROR;


	//Make shut-down flags
	DWORD dwShutDownFlags = 0;

	BOOL bSetShutdownPriv = FALSE;

	//Is it a remote computer
	BOOL bRemoteComp = pAI->pStrRemoteCompName && pAI->pStrRemoteCompName[0];


	//For some reason the API needs non-constant strings
	TCHAR* pStrMessage = NULL;
	int nLnRemoteCompName = pAI->pStrRemoteCompName && pAI->pStrRemoteCompName[0] ? lstrlen(pAI->pStrRemoteCompName) : 0;
	TCHAR* pStrRemoteCompName = new (std::nothrow) TCHAR[nLnRemoteCompName + 1];
	if(pStrRemoteCompName)
	{
		//Copy it
		if(nLnRemoteCompName > 0)
			memcpy(pStrRemoteCompName, pAI->pStrRemoteCompName, nLnRemoteCompName * sizeof(TCHAR));

		pStrRemoteCompName[nLnRemoteCompName] = 0;

		//Now the message to the user
		int nLnMessage = pAI->pStrMessage && pAI->pStrMessage[0] ? lstrlen(pAI->pStrMessage) : 0;
		pStrMessage = new (std::nothrow) TCHAR[nLnMessage + 1];
		if(pStrMessage)
		{
			//Copy it
			if(nLnMessage > 0)
				memcpy(pStrMessage, pAI->pStrMessage, nLnMessage * sizeof(TCHAR));

			pStrMessage[nLnMessage] = 0;


			//Error message buffer
			TCHAR buffErrMsg[1024];
			buffErrMsg[0] = 0;

			LPCTSTR pStrPwrOpName = L"<?>";


			//Go by power op type
			switch(pAI->pwrAction)
			{
			case PWR_OP_SHUT_DOWN:
				{
					pStrPwrOpName = L"shut-down";
					dwShutDownFlags = SHUTDOWN_POWEROFF;
					bSetShutdownPriv = TRUE;
				}
				break;

			case PWR_OP_REBOOT:
				{
					pStrPwrOpName = L"rebooting";
					dwShutDownFlags = SHUTDOWN_RESTART;
				}
				break;

			case PWR_OP_HYBRID_SHUT_DOWN:
				{
					pStrPwrOpName = L"hybrid shut-down";
					dwShutDownFlags = SHUTDOWN_POWEROFF | SHUTDOWN_HYBRID;
					bSetShutdownPriv = TRUE;
				}
				break;

			case PWR_OP_REBOOT_WITH_APP_RESTART:
				{
					pStrPwrOpName = L"rebooting & restart of registered apps";
					dwShutDownFlags = SHUTDOWN_RESTART | SHUTDOWN_RESTARTAPPS;
				}
				break;

			case PWR_OP_ADVANCED_BOOT_OPTIONS_MENU:
				{
					pStrPwrOpName = L"going to advanced boot options menu";
					dwShutDownFlags = SHUTDOWN_RESTART;
					dwShutDownFlags |= 0x400;		//Special flag for Windows 10

					//Cannot install updates, but it can be forced
					pAI->bNoUpdates = TRUE;
				}
				break;

			case PWR_OP_ABORT_SHUT_DOWN:
				{
					//Abort previous

					//Don't run main action
					dwShutDownFlags = 0;

					//Set needed privelge (it will post warnings internally)
					SetNeededShutdownPrivileges(pAI->pStrRemoteCompName);

					//Abort
					if(::AbortSystemShutdown(nLnRemoteCompName > 0 ? pStrRemoteCompName : NULL))
					{
						//Success
						nResOSErrCode = NO_ERROR;
						_tprintf(L"Success aborting power operation...\n");
					}
					else
					{
						//Failed
						nResOSErrCode = ::GetLastError();
						_tprintf(L"ERROR: Failed to abort power operation: (%d) %s\n", 
							nResOSErrCode,
							CMain::FormatErrorMessage(nResOSErrCode, buffErrMsg, SIZEOF(buffErrMsg))
							);
					}
				}
				break;

			default:
				{
					//Wrong flag
					ASSERT(NULL);
					nResOSErrCode = ERROR_INVALID_USER_BUFFER;
					_tprintf(L"INTERNAL ERROR: Wrong action type=%d\n", pAI->pwrAction);
				}
				break;
			}


			if(dwShutDownFlags != 0)
			{
				//Adjust computer name
				LPTSTR pStrCompNm = nLnRemoteCompName > 0 ? pStrRemoteCompName : NULL;

				//See if we have first two leading slashes
				if(nLnRemoteCompName > 2)
				{
					if(pStrCompNm[0] == '\\' ||
						pStrCompNm[1] == '\\')
					{
						//Skip slashes
						pStrCompNm = pStrCompNm + 2;
					}
				}

				//Set needed privelge (it will post warnings internally)
				SetNeededShutdownPrivileges(pStrCompNm);


				//See what API do we need to use?
				if(!pAI->bNoUpdates ||
					(pAI->pwrAction != PWR_OP_SHUT_DOWN &&
					pAI->pwrAction != PWR_OP_REBOOT)
					)
				{
					//Installing updates?
					if(!pAI->bNoUpdates)
					{
						dwShutDownFlags |= SHUTDOWN_INSTALL_UPDATES;
					}

					//Forced?
					if(pAI->bForced)
					{
						dwShutDownFlags |= SHUTDOWN_FORCE_SELF | SHUTDOWN_FORCE_OTHERS;
					}


					//Now execute power op
					::SetLastError(NO_ERROR);
					nResOSErrCode = ::InitiateShutdown(
						pStrCompNm,
						nLnMessage > 0 ? pStrMessage : NULL,
						pAI->nTimeoutSec, 
						dwShutDownFlags,
						pAI->dwReason);
				}
				else
				{
					//Use different API
					//INFO: We need to do this because InitiateShutdown() does not work on remote computers!

					//Pick operation
					BOOL bRebootAfter = -1;
					if(pAI->pwrAction == PWR_OP_SHUT_DOWN)
					{
						bRebootAfter = FALSE;
					}
					else if(pAI->pwrAction == PWR_OP_REBOOT)
					{
						bRebootAfter = TRUE;
					}
					
					if(bRebootAfter == TRUE ||
						bRebootAfter == FALSE)
					{
						//Initiate now
						::SetLastError(NO_ERROR);
						if(::InitiateSystemShutdownEx(
							pStrCompNm,
							nLnMessage > 0 ? pStrMessage : NULL,
							pAI->nTimeoutSec,
							pAI->bForced,
							bRebootAfter,
							pAI->dwReason))
						{
							//Success
							nResOSErrCode = NO_ERROR;
						}
						else
						{
							//Failed
							nResOSErrCode = ::GetLastError();
						}
					}
					else
					{
						//Bad power op
						nResOSErrCode = ERROR_INVALID_FUNCTION;
					}
				}

				//Check result
				if(nResOSErrCode == NO_ERROR)
				{
					//Success
					_tprintf(L"Success initiating %s...\n", pStrPwrOpName);
				}
				else
				{
					//Failed
					_tprintf(L"ERROR: Failed to initiate %s: (%d) %s\n",
						pStrPwrOpName,
						nResOSErrCode,
						CMain::FormatErrorMessage(nResOSErrCode, buffErrMsg, SIZEOF(buffErrMsg))
						);
				}
			}

		}
		else
			nResOSErrCode = ERROR_OUTOFMEMORY;
	}
	else
		nResOSErrCode = ERROR_OUTOFMEMORY;



	//Free mem
	if(pStrRemoteCompName)
	{
		delete[] pStrRemoteCompName;
		pStrRemoteCompName = NULL;
	}

	if(pStrMessage)
	{
		delete[] pStrMessage;
		pStrMessage = NULL;
	}

	return nResOSErrCode;
}



void CMain::outputMainLogo()
{
	SYSTEMTIME st;
	::GetSystemTime(&st);

	_tprintf(
		L"Utility To Install Pre-Downloaded Windows Updates & Shutdown/Reboot\n"
		L"v.%s\n"
		L"Copyright (c) %s%d by www.dennisbabkin.com. All rights reserved.\n"
		L"\n"
		,
		PROG_VER,
		st.wYear > 2016 ? L"2016-" : L"",
		st.wYear
		);

}





