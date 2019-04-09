//Custom types

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




#pragma once





enum CMD_TYPE{
	CTP_NONE,
	CTP_UNKNOWN,					//Unknown command
	CTP_SHOW_HELP,					//-?
	CTP_SHUT_DOWN,					//-s
	CTP_REBOOT,						//-r
	CTP_HYBRID_SHUT_DOWN,			//-hs
	CTP_REBOOT_WITH_APP_RESTART,	//-g
	CTP_ADVANCED_BOOT_MENU,			//-abo
	CTP_ABORT_SHUT_DOWN,			//-a
	CTP_FORCED,						//-f
	CTP_VERBOSE,					//-v
	CTP_NO_UPDATES,					//-nu
	CTP_REMOTE_COMPUTER,			//-m
	CTP_TIMEOUT,					//-t
	CTP_SHOW_MESSAGE,				//-c
	CTP_REASON,						//-d
};



struct CMD_STR{
	CMD_TYPE cmdID;
	LPCTSTR pStrCmd;
};



enum POWER_OP{
	PWR_OP_NONE,
	PWR_OP_SHUT_DOWN,
	PWR_OP_REBOOT,
	PWR_OP_HYBRID_SHUT_DOWN,
	PWR_OP_REBOOT_WITH_APP_RESTART,
	PWR_OP_ABORT_SHUT_DOWN,
	PWR_OP_ADVANCED_BOOT_OPTIONS_MENU,
};


#ifndef SHUTDOWN_HYBRID
#define SHUTDOWN_HYBRID 0x00000200
#endif




struct ACTIONS_INFO{
	POWER_OP pwrAction;
	DWORD dwReason;
	BOOL bForced;
	BOOL bVerbose;
	BOOL bNoUpdates;

	LPCTSTR pStrRemoteCompName;
	int nTimeoutSec;

	LPCTSTR pStrMessage;

	ACTIONS_INFO()
	{
		pwrAction = PWR_OP_NONE;
		dwReason = SHTDN_REASON_FLAG_PLANNED | 0xFF;
		bForced = FALSE;
		bVerbose = FALSE;
		bNoUpdates = FALSE;

		pStrRemoteCompName = NULL;
		nTimeoutSec = 0;

		pStrMessage = NULL;
	}
};


#pragma warning(push)
#pragma warning(disable: 4200)
struct MY_STRINGRESOURCEIMAGE
{
	WORD nLength;
	WCHAR achString[];
};
#pragma warning(pop)	// C4200



struct LOC_STRING{
	LOC_STRING(UINT nID)
	{
		//'nID' = string ID from resources
		pString = NULL;
		loadStringFromRsrc(nID);
	}

	~LOC_STRING()
	{
		//Free mem
		freeString();
	}

	operator const TCHAR*()
	{
		return pString ? pString : L"";
	}
	operator TCHAR*()
	{
		return pString ? pString : L"";
	}

private:
	TCHAR* pString;				//Loaded string, or NULL if error

private:
	void freeString()
	{
		if(pString)
		{
			delete[] pString;
			pString = NULL;
		}
	}

	BOOL loadStringFromRsrc(UINT nID)
	{
		//RETURN:
		//		= TRUE if success
		BOOL bRes = FALSE;

		HMODULE hInstance = ::GetModuleHandle(NULL);
		if(hInstance)
		{
			HRSRC hResource = ::FindResource(hInstance, MAKEINTRESOURCE( ((nID>>4)+1) ), RT_STRING);
			if(hResource)
			{
				HGLOBAL hGlobal = ::LoadResource(hInstance, hResource);
				if(hGlobal)
				{
					const MY_STRINGRESOURCEIMAGE* pImage = (const MY_STRINGRESOURCEIMAGE*)::LockResource(hGlobal);
					if(pImage)
					{
						ULONG nResourceSize = ::SizeofResource(hInstance, hResource);

						const MY_STRINGRESOURCEIMAGE* pImageEnd = (const MY_STRINGRESOURCEIMAGE*)(LPBYTE( pImage )+nResourceSize);
						UINT iIndex = nID & 0x000f;

						while( (iIndex > 0) && (pImage < pImageEnd) )
						{
							pImage = (const MY_STRINGRESOURCEIMAGE*)(LPBYTE( pImage )+(sizeof( MY_STRINGRESOURCEIMAGE )+(pImage->nLength * sizeof( WCHAR ))));
							iIndex--;
						}

						if(pImage < pImageEnd)
						{
							if(pImage->nLength != 0)
							{
								//Free old string
								freeString();

								//Reserve mem
								pString = new (std::nothrow) TCHAR[pImage->nLength + 1];
								if(pString)
								{
									//Copy string
									memcpy(pString, pImage->achString, pImage->nLength * sizeof(TCHAR));
									pString[pImage->nLength] = 0;

									bRes = TRUE;
								}
							}
						}
					}
				}
			}
		}

		return bRes;
	}
};






