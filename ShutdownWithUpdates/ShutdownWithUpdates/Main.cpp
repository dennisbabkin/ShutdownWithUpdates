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
		EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(100);
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
		REBOOT_REQUIRED_CLAUSE rrc;

		BOOL bDoActions = FALSE;				//TRUE to do power op actions

		ACTIONS_INFO ai;

		//See what version of Windows we're running on
		BOOL bWin10 = FALSE;	//TRUE for Windows 10, or later
		BOOL bWin8 = FALSE;		//TRUE for Windows 8, or later

		RTL_OSVERSIONINFOW osi = {};
		if(GetWindowsVersion(&osi))
		{
			if((osi.dwMajorVersion == 6 && osi.dwMinorVersion >= 2) || osi.dwMajorVersion > 6)
			{
				bWin8 = TRUE;
			}

			if(osi.dwMajorVersion >= 10)
			{
				bWin10 = TRUE;
			}
		}
		else
		{
			EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(137);
			ASSERT(NULL);
		}





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
					EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(102, L"cmd: %s", pStrCmd);

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
					EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(103, L"cmd: %s", pStrCmd);

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
						EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(104, L"cmd: %s", pStrCmd);

						_tprintf(L"ERROR: Power action is already defined before parameter: %s\n", pStrCmd);

						bDoActions = FALSE;
						nResOSErrCode = ERROR_DATATYPE_MISMATCH;
						break;
					}
				}
				else
				{
					//Error
					EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(105, L"cmd: %s", pStrCmd);

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
					EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(106, L"cmd: %s", pStrCmd);

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
						EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(107, L"cmd: %s", pStrCmd);

						_tprintf(L"ERROR: Power action is already defined before parameter: %s\n", pStrCmd);

						bDoActions = FALSE;
						nResOSErrCode = ERROR_DATATYPE_MISMATCH;
						break;
					}
				}
				else
				{
					//Error
					EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(108, L"cmd: %s", pStrCmd);

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
					EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(109, L"cmd: %s", pStrCmd);

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
			else if(cmdType == CTP_ARSO)
			{
				//ARSO option
				ai.bUseARSO = TRUE;
			}
			else if(cmdType == CTP_IF_REBOOT_REQUIRED)
			{
				//If reboot required
				rrc = RR_C_IF_REBOOT_REQUIRED;

				goto lbl_chk_rr;
			}
			else if(cmdType == CTP_CHECK_REBOOT_REQUIRED)
			{
				//Check reboot required
				rrc = RR_C_CHECK_REBOOT_REQUIRED;

				goto lbl_chk_rr;
			}
			else if(cmdType == CTP_WAIT_REBOOT_REQUIRED)
			{
				//Wait for reboot required
				rrc = RR_C_WAIT_FOR_REBOOT_REQUIRED;
lbl_chk_rr:
				if(bWin10)
				{
					//Make sure that the user doesn't mix these parameters
					if(ai.rebootReq == RR_C_None ||
						ai.rebootReq == rrc)
					{
						//Set it
						ai.rebootReq = rrc;
					}
					else
					{
						//Error in command line
						EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(134, L"cmd: %s", pStrCmd);

						_tprintf(L"ERROR: Check-updates parameter was already set before: %s\n", pStrCmd);

						bDoActions = FALSE;
						nResOSErrCode = 1783;
						break;
					}
				}
				else
				{
					//Unsupported OS
					EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(138, L"cmd: %s", pStrCmd);

					_tprintf(L"ERROR: Check-updates parameter is not supported on this OS: %s\n", pStrCmd);

					bDoActions = FALSE;
					nResOSErrCode = ERROR_NOT_SUPPORTED;
					break;
				}
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
					EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(110, L"cmd: %s", pStrCmd);

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
							EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(111, L"v=%s, cmd=%s", pStrParam, pStrCmd);

							_tprintf(L"ERROR: Invalid time-out value (%s) for parameter: %s\n", pStrParam, pStrCmd);

							bDoActions = FALSE;
							nResOSErrCode = ERROR_DATATYPE_MISMATCH;
							break;
						}
					}
					else
					{
						//Error
						EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(112, L"cmd: %s", pStrCmd);

						_tprintf(L"ERROR: Failed to parse time-out value for parameter: %s\n", pStrCmd);

						bDoActions = FALSE;
						nResOSErrCode = ERROR_DATATYPE_MISMATCH;
						break;
					}
				}
				else
				{
					//Error
					EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(113, L"cmd: %s", pStrCmd);

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
					EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(114, L"cmd: %s", pStrCmd);

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
						EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(115, L"cmd: %s", pStrCmd);

						_tprintf(L"ERROR: Failed to parse shut-down/rebooting reason parameter: %s\n", pStrParam);

						bDoActions = FALSE;
						nResOSErrCode = ERROR_DATATYPE_MISMATCH;
						break;
					}
				}
				else
				{
					//Error
					EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(116, L"cmd: %s", pStrCmd);

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
					EVENT_LOG_REPORTS::ReportEventLogMsgWARNING_WithFormat(L"[117] cmd: %s", pStrCmd);

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
					EVENT_LOG_REPORTS::ReportEventLogMsgWARNING_WithFormat(L"[118] cmd=%s", pStrCmd);

					_tprintf(L"WARNING: Unknown command: %s\n", pStrCmd);
				}
				else
				{
					EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(119, L"cmd=%s", pStrCmd);
					ASSERT(NULL);
				}
			}
			else
			{
				//Error
				EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(120, L"cmd=%s", pStrCmd);
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
			//See that some special options aren't used
			if(ai.rebootReq != RR_C_CHECK_REBOOT_REQUIRED)
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
						EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(121, L"r=%d", nResUI);
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
				EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(145);

				_tprintf(L"ERROR: Check-updates parameter must be used alone: -crr\n");

				nResOSErrCode = 1783;
			}
		}
		else
		{
			//Did we get an error?
			if(nResOSErrCode == ERROR_GEN_FAILURE)
			{
				//Do we have to check for reboot-required?
				if(ai.rebootReq != RR_C_None)
				{
					if(ai.rebootReq == RR_C_CHECK_REBOOT_REQUIRED)
					{
						//Check if reboot is required
						RES_YES_NO_ERR resRR = CMain::IsRebootRequired();
						if(resRR == RYNE_YES ||
							resRR == RYNE_NO)
						{
							//Output result
							if(resRR == RYNE_YES)
							{
								nResOSErrCode = ERROR_FAIL_NOACTION_REBOOT;		// 350
								_tprintf(L"Reboot is required to install updates\n");
							}
							else
							{
								nResOSErrCode = 0;
								_tprintf(L"Reboot to install updates is not required now\n");
							}
						}
						else
						{
							//Error
							nResOSErrCode = GetLastError();
							EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(136);
							ASSERT(NULL);

							_tprintf(L"ERROR: Failed to determine reboot-required status\n");
						}
					}
					else if(ai.rebootReq == RR_C_WAIT_FOR_REBOOT_REQUIRED)
					{
						//Wait for reboot-enabled status to be on
						nResOSErrCode = CheckOrWaitForRebootRequired(&ai, bWin10);
						
					}
					else
					{
						//Warning
						EVENT_LOG_REPORTS::ReportEventLogMsgWARNING_WithFormat(L"[135] Check-updates ignored: %d", ai.rebootReq);

						_tprintf(L"WARNING: Check-updates parameter was ignored\n");
					}
				}
				else
				{
					//Not enough parameters
					EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(122);

#ifndef ERROR_INVALID_FIELD_IN_PARAMETER_LIST
#define ERROR_INVALID_FIELD_IN_PARAMETER_LIST 328
#endif
					nResOSErrCode = ERROR_INVALID_FIELD_IN_PARAMETER_LIST;
					_tprintf(L"ERROR: Not enough parameters\n");
				}
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
				{CTP_SHOW_HELP,		L"h"},
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
				{CTP_ARSO,		L"arso"},
				{CTP_IF_REBOOT_REQUIRED,		L"irr"},
				{CTP_CHECK_REBOOT_REQUIRED,		L"crr"},
				{CTP_WAIT_REBOOT_REQUIRED,		L"wrr"},
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
		L"Usage:\n"
		L"       ShutdownWithUpdates [/s | /r | /hs | /g | /a | /?] [/f] [/v] [/nu]\n"
		L"        [/m \\\\computer] [/t x] [/c \"msg\"] [/d [p|u:]xx:yy] [/arso]\n"
		L"        [/irr | /crr | /wrr]\n"
		L"\n"
		L" Windows 10: This process may require to run as administrator for some options.\n"
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
		L"  /f    Use forced action.\n"
		L"         WARNING: May result in the loss of unsaved data on target computer!\n"
		L"  /irr  Allow operations only if updates are installed and ready for reboot.\n"
		L"         (Local Windows 10 only.)\n"
		L"         (Will exit with error code 1235 if updates are not ready.)\n"
		L"  /crr  Only check if updates are installed and ready for reboot.\n"
		L"         (Local Windows 10 only. Can't be used with other parameters.)\n"
		L"         (Will exit with code 0 if updates are not ready, or 350 if they are.)\n"
		L"  /wrr  Wait for updates to be installed and ready for reboot before proceeding.\n"
		L"         (Local Windows 10 only.)\n"
		L"  /arso Enables \"Winlogon automatic restart sign-on\". (Local Windows 10 only.)\n"
		L"         INFO: https://dennisbabkin.com/r/?to=arso\n"
		L"  /v    Show user confirmation before proceeding.\n"
		L"         (Local computer only. It is shown before time-out is initiated.)\n"
		L"  /nu   Not to install updates.\n"
		L"         (Local Windows 10 only. Must be running as administrator.)\n"
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
		L"         https://dennisbabkin.com/r/?to=win32sdrc\n"
		L"\n"
		L"  /?    Show command line help.\n"
		L"\n"
		L"Exit Codes:\n"
		L" 0      if success.\n"
		L" -1     if general failure in the module.\n"
		L" Other  if error, will contain \"System Error Code\". For details check:\n"
		L"         https://dennisbabkin.com/r/?to=win32errs\n"
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
		L"(3) Reboot local computer only if updates are ready to install:\n"
		L"    (Fail if unsaved user data on computer, or updates aren't ready.)\n"
		L"\n"
		L"      ShutdownWithUpdates /r /irr\n"
		L"\n"
		L"(4) Simply wait until updates are ready to install on local computer,\n"
		L"     and then start a calculator:\n"
		L"\n"
		L"      ShutdownWithUpdates /wrr\n"
		L"      calc.exe\n"
		L"\n"
		L"(5) Do not install updates and shut down local computer:\n"
		L"    (Fail if unsaved user data on computer. Must run as administrator!)\n"
		L"\n"
		L"      ShutdownWithUpdates /s /nu\n"
		L"\n"
		L"(6) Do not install updates and reboot remote computer after a 20 sec delay:\n"
		L"    (/nu option is not supported on Windows 10 for remote computer.)\n"
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

		TCHAR buff[256];

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
			TCHAR buffAux[256] = {0};
			if(!bAborting)
			{
				if(pAI->bForced)
				{
					::StringCchPrintf(buff, SIZEOF(buff), L" (%s)", LOC_STRING(IDS_STRING107));		//L"forced"
					::StringCchCat(buffAux, SIZEOF(buffAux), buff);
				}

				if(pAI->bUseARSO)
				{
					::StringCchPrintf(buff, SIZEOF(buff), L" (%s)", LOC_STRING(IDS_STRING120));		//L"use ARSO"
					::StringCchCat(buffAux, SIZEOF(buffAux), buff);
				}

				if(pAI->rebootReq == RR_C_IF_REBOOT_REQUIRED)
				{
					::StringCchPrintf(buff, SIZEOF(buff), L" (%s)", LOC_STRING(IDS_STRING121));		//L"only if updates are ready"
					::StringCchCat(buffAux, SIZEOF(buffAux), buff);
				}
				else if(pAI->rebootReq == RR_C_WAIT_FOR_REBOOT_REQUIRED)
				{
					::StringCchPrintf(buff, SIZEOF(buff), L" (%s)", LOC_STRING(IDS_STRING122));		//L"wait for updates to be ready"
					::StringCchCat(buffAux, SIZEOF(buffAux), buff);
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
				CapitalizeFirstLetter(buffPowerOp), buffAux,
				CapitalizeFirstLetter(LOC_STRING(IDS_STRING114)),		//Computer
				buffComp,
				buffTimeout,
				buffUserMsg
				);

			LOC_STRING strTitle(IDS_STRING115);

			//Show UI
			::SetLastError(NO_ERROR);

			DWORD nResMB = 0;
			if(::WTSSendMessage(WTS_CURRENT_SERVER_HANDLE, ::WTSGetActiveConsoleSessionId(),
				strTitle, lstrlen(strTitle) * sizeof(WCHAR),
				buffMsg, lstrlen(buffMsg) * sizeof(WCHAR),
				MB_YESNOCANCEL | MB_DEFBUTTON2 | MB_ICONQUESTION | MB_SYSTEMMODAL | MB_SETFOREGROUND,
				0,
				&nResMB,
				TRUE))
			{
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
					nOSError = GetLastErrorNotNULL(ERROR_INVALID_LEVEL);
					EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(146, L"r=%d", nResMB);
				}
			}
			else
			{
				//Error
				nOSError = GetLastErrorNotNULL(ERROR_INVALID_LEVEL);
				EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(147);
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

	//Set shut-down privilege  
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
	DWORD dwShutDownFlags =	0;

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

					////Cannot install updates, but it can be forced
					//pAI->bNoUpdates = TRUE;
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
						EVENT_LOG_REPORTS::ReportEventLogMsgInfo_WithFormat(L"[126] Power op aborted. Computer=\"%s\"", nLnRemoteCompName > 0 ? pStrRemoteCompName : L"");

						_tprintf(L"Success aborting power operation...\n");
					}
					else
					{
						//Failed
						nResOSErrCode = ::GetLastError();
						EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(125, L"Computer=\"%s\"", nLnRemoteCompName > 0 ? pStrRemoteCompName : L"");

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
					EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(127, L"v=%d", pAI->pwrAction);

					nResOSErrCode = ERROR_INVALID_USER_BUFFER;
					_tprintf(L"INTERNAL ERROR: Wrong action type=%d\n", pAI->pwrAction);

					ASSERT(NULL);
				}
				break;
			}


			if(dwShutDownFlags != 0)
			{
				BOOL bContinueWithPowerOp = TRUE;
				DWORD dwV;

				//Is it Windows 10?
				BOOL bWindows10 = FALSE;

				RTL_OSVERSIONINFOW osi = {};
				if(GetWindowsVersion(&osi))
				{
					if(osi.dwMajorVersion >= 10)
					{
						bWindows10 = TRUE;
					}
				}
				else
				{
					EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(128);
					ASSERT(NULL);
				}



				//Do we need to wait until reboot-required is on, or just to check for it?
				int nErrRR = CheckOrWaitForRebootRequired(pAI, bWindows10);
				if(nErrRR != NO_ERROR)
				{
					//Can't continue
					bContinueWithPowerOp = FALSE;
					nResOSErrCode = nErrRR;
				}



				if(bContinueWithPowerOp)
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



					//Do we have to prevent installation of updates?
					if(pAI->bNoUpdates)
					{
						//Only for Windows 10
						if(bWindows10)
						{
							//Set special registry keys needed for no-updates option

							//For details read the following blog post:
							//
							//	https://dennisbabkin.com/blog/?t=how-to-enable-installation-of-updates-or-to-prevent-it-during-reboot-or-shutdown
							//

							//  HKLM\SOFTWARE\Microsoft\WindowsUpdate\CommitStatus  	DWORD: NoCommit=1
							dwV = 1;
							if(!WriteValueToSystemRegistry(HKEY_LOCAL_MACHINE, TRUE, 
								L"SOFTWARE\\Microsoft\\WindowsUpdate\\CommitStatus", L"NoCommit", REG_DWORD, &dwV, sizeof(dwV)))
							{
								//Failed
								nResOSErrCode = GetLastErrorNotNULL();
								EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(130, L"Failed to set CommitStatus=1");

								//Do not proceed with the power operation as it will begin installing updates
								bContinueWithPowerOp = FALSE;

								ASSERT(NULL);
							}

							if(bContinueWithPowerOp)
							{
								//  HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\WindowsUpdate\Orchestrator\InstallAtShutdown		DWORD: (Default)=0
								dwV = 0;
								if(!WriteValueToSystemRegistry(HKEY_LOCAL_MACHINE, TRUE, 
									L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Orchestrator\\InstallAtShutdown", NULL, REG_DWORD, &dwV, sizeof(dwV)))
								{
									//Failed
									nResOSErrCode = GetLastErrorNotNULL();
									EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(129, L"Failed to set InstallAtShutdown=0");

									//Do not proceed with the power operation as it will begin installing updates
									bContinueWithPowerOp = FALSE;

									ASSERT(NULL);
								}
							}

							if(!bContinueWithPowerOp)
							{
								if(nResOSErrCode == ERROR_ACCESS_DENIED)
								{
									_tprintf(L"ERROR: Failed to prevent installation of updates. "
										L"Make sure that this process is running as administrator.\n");
								}
								else
								{
									//Some other error
									_tprintf(L"ERROR: Failed to prevent installation of updates: (%d) %s\n",
										nResOSErrCode,
										CMain::FormatErrorMessage(nResOSErrCode, buffErrMsg, SIZEOF(buffErrMsg))
										);
								}
							}
						}
					}


					if(bContinueWithPowerOp)
					{
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

								//Is it Windows 10?
								if(bWindows10)
								{
									//For details read the following blog post:
									//
									//	https://dennisbabkin.com/blog/?t=how-to-enable-installation-of-updates-or-to-prevent-it-during-reboot-or-shutdown
									//

									//First need to delete NoCommit value, if it's still there
									if(!DeleteValueAndEmptyKeyFromSystemRegistry(HKEY_LOCAL_MACHINE, TRUE, 
										L"SOFTWARE\\Microsoft\\WindowsUpdate\\CommitStatus", L"NoCommit"))
									{
										//Failed
										nResOSErrCode = GetLastErrorNotNULL();
										EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(148, L"Failed to remove NoCommit");

										goto lbl_fail_02;
									}

									//And set the key that should instruct to install updates
									dwV = 1;
									if(!WriteValueToSystemRegistry(HKEY_LOCAL_MACHINE, TRUE,
										L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Orchestrator\\InstallAtShutdown",
										NULL, REG_DWORD, &dwV, sizeof(dwV)))
									{
										//Error
										nResOSErrCode = GetLastErrorNotNULL();
										EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(131, L"Failed to set InstallAtShutdown=1");

lbl_fail_02:
										//Do not proceed with the power operation as update will not be installed!
										bContinueWithPowerOp = FALSE;

										if(nResOSErrCode == ERROR_ACCESS_DENIED)
										{
											_tprintf(L"ERROR: Failed to enable installation of updates. "
												L"Make sure that this process is running as administrator.\n");
										}
										else
										{
											//Some other error
											_tprintf(L"ERROR: Failed to enable installation of updates: (%d) %s\n",
												nResOSErrCode,
												CMain::FormatErrorMessage(nResOSErrCode, buffErrMsg, SIZEOF(buffErrMsg))
												);
										}
									}
								}
							}

							//Forced?
							if(pAI->bForced)
							{
								dwShutDownFlags |= SHUTDOWN_FORCE_SELF | SHUTDOWN_FORCE_OTHERS;
							}

							//ARSO?
							if(pAI->bUseARSO)
							{
								dwShutDownFlags |= SHUTDOWN_ARSO;
							}


							if(bContinueWithPowerOp)
							{
#ifndef TEST_POWER_OP
								//Now execute power op
								::SetLastError(NO_ERROR);
								nResOSErrCode = ::InitiateShutdown(
									pStrCompNm,
									nLnMessage > 0 ? pStrMessage : NULL,
									pAI->nTimeoutSec, 
									dwShutDownFlags,
									pAI->dwReason);
#else
								//Simulate success
								nResOSErrCode = 0;
#endif
							}
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
#ifndef TEST_POWER_OP
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
									nResOSErrCode = GetLastErrorNotNULL();
								}
#else
								//Simulate syccess
								nResOSErrCode = 0;
#endif
							}
							else
							{
								//Bad power op
								nResOSErrCode = ERROR_INVALID_FUNCTION;
							}
						}
					}


					LPCTSTR pStrNoUpdates = pAI->bNoUpdates ? L" (no updates)" : L"";

					//Check result
					if(nResOSErrCode == NO_ERROR)
					{
						//Success
						EVENT_LOG_REPORTS::ReportEventLogMsgInfo_WithFormat(L"[132] Initiated %s%s", pStrPwrOpName, pStrNoUpdates);

						_tprintf(L"Success initiating %s%s...\n", pStrPwrOpName, pStrNoUpdates);
					}
					else
					{
						//Failed
						SetLastError(nResOSErrCode);
						EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(133, L"Failed to initiate %s%s", pStrPwrOpName, pStrNoUpdates);

						_tprintf(L"ERROR: Failed to initiate %s%s: (%d) %s\n",
							pStrPwrOpName,
							pStrNoUpdates,
							nResOSErrCode,
							CMain::FormatErrorMessage(nResOSErrCode, buffErrMsg, SIZEOF(buffErrMsg))
							);
					}
				}
			}
		}
		else
		{
			EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(124, L"n=%d", nLnMessage);
			nResOSErrCode = ERROR_OUTOFMEMORY;
		}
	}
	else
	{
		EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(123, L"n=%d", nLnRemoteCompName);
		nResOSErrCode = ERROR_OUTOFMEMORY;
	}



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

BOOL CMain::WriteValueToSystemRegistry(HKEY hIniKey, BOOL bWOW64, LPCTSTR lpSubKey, LPCTSTR lpKeyValue, DWORD dwValueType, const void* pData, int ncbDataSz)
{
	//Write value into the 'lpSubKey' key, into 'lpKeyValue' value
	//'hIniKey' = Initial key. Can be also: HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER, etc.
	//'bWOW64' = TRUE if 'lpSubKey' is a redirected WOW64 key under x86 build
	//'dwValueType' = Could be REG_SZ, REG_EXPAND_SZ, REG_MULTI_SZ, REG_DWORD
	//'pData' = data to write
	//'ncbDataSz' = size of 'pData' in BYTEs
	//RETURN:
	//		= TRUE if done
	//		= FALSE if error (check GetLastError() for info)
	HKEY hKey;
	DWORD dwR;
	BOOL bRes = FALSE;
	REGSAM dwSam;

#ifdef _M_X64
	//64-bit
	dwSam = KEY_SET_VALUE;
#else
	//32-bit
	dwSam = KEY_SET_VALUE | (bWOW64 ? KEY_WOW64_64KEY : 0);
#endif

	dwR = RegCreateKeyEx(hIniKey, lpSubKey, NULL, NULL, 0, dwSam, NULL, &hKey, NULL);
	if(dwR == ERROR_SUCCESS)
	{
		//Set value
		if((dwR = RegSetValueEx(hKey, lpKeyValue, NULL, dwValueType, (const BYTE *)pData, ncbDataSz)) == ERROR_SUCCESS)
		{
			//Done
			bRes = TRUE;
		}

		//Close key
		RegCloseKey(hKey);
	}

	//Set last error
	::SetLastError(dwR);
	return bRes;
}


BOOL CMain::DeleteValueAndEmptyKeyFromSystemRegistry(HKEY hIniKey, BOOL bWOW64, LPCTSTR lpSubKey, LPCTSTR lpKeyValue)
{
	//Delete 'lpSubKey' value from the 'lpKeyValue' and then 'lpSubKey' itself if its empty
	//'hIniKey' = Initial key. Can be also: HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER, etc.
	//'bWOW64' = TRUE if 'lpSubKey' is a redirected WOW64 key under x86 build
	//RETURN:
	//		= TRUE if done
	//		= FALSE if error (check GetLastError() for info)
	HKEY hKey;
	DWORD dwR;
	BOOL bRes = FALSE;
	REGSAM dwSam;

#ifdef _M_X64
	//64-bit
	dwSam = KEY_SET_VALUE | KEY_QUERY_VALUE;
#else
	//32-bit
	dwSam = KEY_SET_VALUE | KEY_QUERY_VALUE | (bWOW64 ? KEY_WOW64_64KEY : 0);
#endif

	//Open such key first
	dwR = RegOpenKeyEx(hIniKey, lpSubKey, 0, dwSam, &hKey);
	if(dwR == ERROR_SUCCESS)
	{
		//Delete our value
		dwR = RegDeleteValue(hKey, lpKeyValue);
		if(dwR == ERROR_SUCCESS ||
			dwR == ERROR_FILE_NOT_FOUND)
		{
			//Success so far
			bRes = TRUE;
		}

		BOOL bCanDeleteKey = FALSE;

		//See if there are any values or subkeys left in that key
		DWORD dwCntSubkeys, dwCntVals;
		DWORD dwR2 = RegQueryInfoKey(hKey, NULL, NULL, NULL, &dwCntSubkeys, NULL, NULL, &dwCntVals, NULL, NULL, NULL, NULL);
		if(dwR2 == ERROR_SUCCESS)
		{
			//Can delete this key only if it has no values and no subkeys
			if(dwCntVals == 0 &&
				dwCntSubkeys == 0)
			{
				bCanDeleteKey = TRUE;
			}
		}
		else
		{
			//Error
			if(bRes)
			{
				dwR = dwR2;
				bRes = FALSE;
			}
		}

		//Close key
		RegCloseKey(hKey);

		if(bCanDeleteKey)
		{
			//Can delete this subkey now
			dwR2 = RegDeleteKeyEx(hIniKey, lpSubKey, 
#ifdef _M_X64
				//64-bit
				0,
#else
				//32-bit
				(bWOW64 ? KEY_WOW64_64KEY : 0),
#endif
				NULL);

			if(dwR2 != ERROR_SUCCESS)
			{
				//Error
				dwR = dwR2;
				bRes = FALSE;
				ASSERT(NULL);
			}
		}
	}
	else if(dwR == ERROR_FILE_NOT_FOUND)
	{
		//No such key - then success, as there's nothing to delete
		bRes = TRUE;
	}

	::SetLastError(dwR);
	return bRes;
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

TCHAR* CMain::FormatDateTime(FILETIME* pFtUtc, TCHAR* pBuff, int nchLnBuff, BOOL bConvertToLocal)
{
	//Format date/time from 'pFtUtc' into a string
	//'pFt' = pointer to FILETIME, expressed as a number of 100-ns intervals since midnight of Jan-1-1601, for UTC time zone
	//'pBuff' = buffer to output result to - must be provided! It will always contain a null-terminated string upon return.
	//'nchLnBuff' = size of 'pBuff' in WCHARs
	//'bConvertToLocal' = TRUE to convert 'pFtUtc' from UTC to local time zone
	//RETURN:
	//		= Pointer to 'pBuff'
	ASSERT(pBuff);
	ASSERT(nchLnBuff > 0);

	if(nchLnBuff > 0)
	{
		pBuff[0] = 0;

		if(pFtUtc)
		{
			SYSTEMTIME st = {};
			if(::FileTimeToSystemTime(pFtUtc, &st))
			{
				BOOL bContinue = TRUE;
				if(bConvertToLocal)
				{
					SYSTEMTIME stLoc = {};
					if(::SystemTimeToTzSpecificLocalTime(NULL, &st, &stLoc))
					{
						st = stLoc;
					}
					else
					{
						ASSERT(NULL);
						bContinue = FALSE;
					}
				}

				if(bContinue)
				{
					WCHAR buffD[128] = {};
					WCHAR buffT[128] = {};
					if(::GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &st, NULL, buffD, _countof(buffD)) &&
						::GetTimeFormat(LOCALE_USER_DEFAULT, 0, &st, NULL, buffT, _countof(buffT)))
					{
						if(FAILED(::StringCchPrintf(pBuff, nchLnBuff, L"%s %s", buffD, buffT)))
						{
							ASSERT(NULL);
						}
					}
					else
						ASSERT(NULL);
				}
			}
			else
				ASSERT(NULL);
		}

		//Safety null
		pBuff[nchLnBuff - 1] = 0;
	}

	return pBuff;
}


RES_YES_NO_ERR CMain::IsRebootRequired()
{
	//Check if reboot is now required to finish installing updates
	//RETURN:
	//		RYNE_YES	= if reboot is required to install updates
	//		RYNE_NO		= if reboot is not required
	//		RYNE_ERROR	= if error determining (check GetLastError() for info)
	RES_YES_NO_ERR res = RYNE_ERROR;

	//For details read the following blog post:
	//
	//	https://dennisbabkin.com/blog/?t=how-to-enable-installation-of-updates-or-to-prevent-it-during-reboot-or-shutdown
	//

	HKEY hKey;
	DWORD dwR = RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
		L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Orchestrator\\RebootRequired", 0, 
#ifdef _M_X64
		//64-bit
		KEY_READ,
#else
		//32-bit
		KEY_READ | KEY_WOW64_64KEY,
#endif
		&hKey);

	if (dwR == ERROR_SUCCESS)
	{
		//Yes it is
		res = RYNE_YES;

		RegCloseKey(hKey);
	}
	else if(dwR == ERROR_FILE_NOT_FOUND)
	{
		//No such key
		res = RYNE_NO;
	}
	else
	{
		//Error
		::SetLastError(dwR);
	}

	return res;
}



BOOL CMain::GetWindowsVersion(RTL_OSVERSIONINFOW* pOutInfo)
{
	//Retrieve "Actual" version of the OS
	//'pOutInfo' = if not NULL, receives version info
	//RETURN:
	//		= TRUE if received result
	//		= FALSE if error (check GetLastError() for results)
	BOOL bRes = FALSE;

	RTL_OSVERSIONINFOW osi = {};

	static HMODULE hMod = NULL;
	if(!hMod)
	{
		hMod = GetModuleHandle(L"ntdll.dll");
		ASSERT(hMod);
	}

	static NTSTATUS (WINAPI *pfnRtlGetVersion)(
	  PRTL_OSVERSIONINFOW lpVersionInformation
	  ) = NULL;

	if(!pfnRtlGetVersion)
	{
		(FARPROC&)pfnRtlGetVersion = GetProcAddress(hMod, "RtlGetVersion");
		ASSERT(pfnRtlGetVersion);
	}

	if(pfnRtlGetVersion)
	{
		NTSTATUS status = pfnRtlGetVersion(&osi);
		if(status == 0)
		{
			bRes = TRUE;
		}
		else
			SetLastError((int)status);
	}
	else
		SetLastError(ERROR_INVALID_FUNCTION);

	if(pOutInfo)
		*pOutInfo = osi;

	return bRes;
}

int CMain::GetLastErrorNotNULL(int nFallbackErrorCode)
{
	//RETURN:
	//		= Last error code from GetLastError() if it's not 0, or
	//		= nFallbackErrorCode value
	int nErr = ::GetLastError();
	return nErr != 0 ? nErr : nFallbackErrorCode;
}


int CMain::CheckOrWaitForRebootRequired(ACTIONS_INFO* pAI, BOOL bWin10)
{
	//Wait or check for reboot-required status
	//'pAI' = parameters from command line call
	//'bWin10' = TRUE if running on Windows 10
	//RETURN:
	//		= 0 if no errors (can continue)
	//		= Other - need to quot
	ASSERT(pAI);
	int nResOSErrCode = NO_ERROR;

	//Do we need to wait until reboot-required is on, or just to check for it
	if(pAI->rebootReq == RR_C_WAIT_FOR_REBOOT_REQUIRED ||
		pAI->rebootReq == RR_C_IF_REBOOT_REQUIRED)
	{
		if(bWin10)
		{
			DWORD dwmsIniTicks = ::GetTickCount();

			if(pAI->rebootReq == RR_C_WAIT_FOR_REBOOT_REQUIRED)
			{
				EVENT_LOG_REPORTS::ReportEventLogMsgInfo_WithFormat(L"[143] Started update-reboot wait");
				_tprintf(L"Beginning to wait for update-reboot status ...\n");
			}

			//Go into a waiting loop
			for(;; ::Sleep(500))
			{
				//Check if reboot is required
				RES_YES_NO_ERR resRR = CMain::IsRebootRequired();

				if(resRR == RYNE_YES)
				{
					//Reboot is required now
					if(pAI->rebootReq == RR_C_WAIT_FOR_REBOOT_REQUIRED)
					{
						EVENT_LOG_REPORTS::ReportEventLogMsgInfo_WithFormat(L"[141] Update-reboot is now enabled, elapsed %.1f sec", (::GetTickCount() - dwmsIniTicks) / 1000.0);
					}
					else
					{
						EVENT_LOG_REPORTS::ReportEventLogMsgInfo_WithFormat(L"[144] Update-reboot is enabled");
					}

					_tprintf(L"Update-reboot status is enabled\n");

					break;
				}
				else if(resRR == RYNE_NO)
				{
					//Not enabled yes
					if(pAI->rebootReq == RR_C_IF_REBOOT_REQUIRED)
					{
						//We're just checking here
						EVENT_LOG_REPORTS::ReportEventLogMsgInfo_WithFormat(L"[142] Update-reboot is not enabled, aborting");

						nResOSErrCode = ERROR_REQUEST_ABORTED;		//1235;
						_tprintf(L"Update-reboot status is not enabled yet ... aborting\n");

						break;
					}
				}
				else
				{
					//Error
					nResOSErrCode = GetLastErrorNotNULL();
					EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(140, L"r=%d, rr=%d", resRR, pAI->rebootReq);
					_tprintf(L"ERROR: (%d) Waiting for update-reboot status\n", nResOSErrCode);
					ASSERT(NULL);

					break;
				}
			}
		}
		else
		{
			//Wrong OS - shouldn't be here!
			nResOSErrCode = ERROR_NOT_SUPPORTED;
			EVENT_LOG_REPORTS::ReportEventLogMsgERROR_Spec_WithFormat(139, L"rr=%d", pAI->rebootReq);
			ASSERT(NULL);
		}
	}

	return nResOSErrCode;
}


