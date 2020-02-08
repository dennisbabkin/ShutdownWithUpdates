/*
 * ShutdownWithUpdates
 * "Utility To Install Pre-Downloaded Windows Updates & Shutdown/Reboot"
 * Copyright (c) 2016-2020 www.dennisbabkin.com
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




#pragma once


#include "types.h"
#include "resource.h"



class CMain
{
private:
	CMain(void);
	~CMain(void);
public:
	static int doWork(int argc, _TCHAR* argv[]);
private:
	static int doWork_RAW(int argc, _TCHAR* argv[]);
protected:
	static int CheckForXP_SP2_FileBlockAndRemoveIt();
	static CMD_TYPE getCommandType(LPCTSTR pStrCmd);
	static void ShowHelpInfo();
	static int findCharInStrCaseSensitive(LPCTSTR pStr, TCHAR chSearch, int nIndBegin = 0);
	static BOOL parseReasonInt32(LPCTSTR pStr, int* pnOutVal = NULL);
	static BOOL parseDecimalInt32(LPCTSTR pStr, int* pnOutVal = NULL);
	static BOOL parseHexUInt32(LPCTSTR pStr, UINT* pnOutVal = NULL);
	static BOOL parseReasonParam(LPCTSTR pStrParam, DWORD* pnOutReason = NULL);
	static TCHAR* CapitalizeFirstLetter(TCHAR* pBuff);
	static BOOL FormatSecSmart(TCHAR* pBuff, int nchBuffSz, int nSeconds);
	static int ShowUserConfirmation(ACTIONS_INFO* pAI);
	static BOOL AdjustPrivilege(LPCTSTR pStrMachine, LPCTSTR pPrivilegeName, BOOL bEnable, HANDLE hProcess = NULL);
	static TCHAR* trimBuffer(TCHAR* pBuff);
	static TCHAR* trimBufferLeft(TCHAR* pBuff);
	static TCHAR* trimBufferRight(TCHAR* pBuff);
	static TCHAR* FormatErrorMessage(int nOSError, TCHAR* pBuff, int nchLnBuff, BOOL bUseDescriptionForNoError = FALSE);
	static BOOL SetNeededShutdownPrivileges(LPCTSTR pStrRemoteCompName);
	static int doActions(ACTIONS_INFO* pAI);
	static REG_WRITE_RES WriteValueToSystemRegistryIfKeyExists(HKEY hIniKey, LPCTSTR lpSubKey, LPCTSTR lpKeyValue, DWORD dwValueType, const void* pData, int ncbDataSz);
	static void outputMainLogo();
	static TCHAR* FormatDateTime(FILETIME* pFtUtc, TCHAR* pBuff, int nchLnBuff, BOOL bConvertToLocal = TRUE);

};
