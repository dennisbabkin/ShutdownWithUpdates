//Determines "true" version of the OS

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



#define TXT_SIZEOF(f) ((sizeof(f) / sizeof(f[0])) - 1)


struct RTL_OS_VERSION{
	static BOOL GetVersionEx2(OSVERSIONINFO* pOutVersionInfo, BOOL bAllowOriginalAPIAsFallback = TRUE)
	{
		//Retrieve "real" OS version
		//INFO: It uses kernel-mode API that does not depend on the manifest included in the executable!
		//'pOutVersionInfo' = struct to fill in, may point to OSVERSIONINFO or OSVERSIONINFOEX structs
		//						INFO: Make sure to fill in its 'dwOSVersionInfoSize' to the struct size!
		//'bAllowOriginalAPIAsFallback' = TRUE to use original "deprecated" API if this kernel-mode fails, FALSE - not to
		//RETURN:
		//		= TRUE if success
		//		= FALSE if error (check GetLastError() for info)
		BOOL bRes = FALSE;
		int nOSError = NO_ERROR;

		if(pOutVersionInfo)
		{
			//Get info
			RTL_OSVERSIONINFOEXW ver = {0};
			if(__rtl_get_version(ver))
			{
				//Check struct size
				if(pOutVersionInfo->dwOSVersionInfoSize == sizeof(OSVERSIONINFO))
				{
					//Smaller struct
					pOutVersionInfo->dwMajorVersion = ver.dwMajorVersion;
					pOutVersionInfo->dwMinorVersion = ver.dwMinorVersion;
					pOutVersionInfo->dwBuildNumber = ver.dwBuildNumber;
					pOutVersionInfo->dwPlatformId = ver.dwPlatformId;

					if(TXT_SIZEOF(pOutVersionInfo->szCSDVersion) == TXT_SIZEOF(ver.szCSDVersion))
					{
#ifdef UNICODE
						//Unicode version
						memcpy(pOutVersionInfo->szCSDVersion, ver.szCSDVersion, sizeof(ver.szCSDVersion));
#else
						//ANSI version
						ver.szCSDVersion[(sizeof(ver.szCSDVersion) / sizeof(ver.szCSDVersion[0])) - 1] = 0;

						int nLn = lstrlenW(ver.szCSDVersion);
						if(nLn > TXT_SIZEOF(pOutVersionInfo->szCSDVersion))
							nLn = TXT_SIZEOF(pOutVersionInfo->szCSDVersion);

						nLn++;

						CHAR* pTgt = pOutVersionInfo->szCSDVersion;
						pTgt[0] = 0;
						::WideCharToMultiByte(CP_ACP, 0, ver.szCSDVersion, nLn, pTgt, nLn, NULL, NULL);
						pTgt[(sizeof(pOutVersionInfo->szCSDVersion) / sizeof(pOutVersionInfo->szCSDVersion[0])) - 1] = 0;
#endif

						//Done
						bRes = TRUE;
					}
					else
						nOSError = ERROR_INVALID_BLOCK_LENGTH;
				}
				else if(pOutVersionInfo->dwOSVersionInfoSize == sizeof(OSVERSIONINFOEX))
				{
					//Larger struct
					((OSVERSIONINFOEX*)pOutVersionInfo)->dwMajorVersion = ver.dwMajorVersion;
					((OSVERSIONINFOEX*)pOutVersionInfo)->dwMinorVersion = ver.dwMinorVersion;
					((OSVERSIONINFOEX*)pOutVersionInfo)->dwBuildNumber = ver.dwBuildNumber;
					((OSVERSIONINFOEX*)pOutVersionInfo)->dwPlatformId = ver.dwPlatformId;
					((OSVERSIONINFOEX*)pOutVersionInfo)->wServicePackMajor = ver.wServicePackMajor;
					((OSVERSIONINFOEX*)pOutVersionInfo)->wServicePackMinor = ver.wServicePackMinor;
					((OSVERSIONINFOEX*)pOutVersionInfo)->wSuiteMask = ver.wSuiteMask;
					((OSVERSIONINFOEX*)pOutVersionInfo)->wProductType = ver.wProductType;
					((OSVERSIONINFOEX*)pOutVersionInfo)->wReserved = ver.wReserved;

					if(TXT_SIZEOF(((OSVERSIONINFOEX*)pOutVersionInfo)->szCSDVersion) == TXT_SIZEOF(ver.szCSDVersion))
					{
#ifdef UNICODE
						//Unicode version
						memcpy(((OSVERSIONINFOEX*)pOutVersionInfo)->szCSDVersion, ver.szCSDVersion, sizeof(ver.szCSDVersion));
#else
						//ANSI version
						ver.szCSDVersion[(sizeof(ver.szCSDVersion) / sizeof(ver.szCSDVersion[0])) - 1] = 0;

						int nLn = lstrlenW(ver.szCSDVersion);
						if(nLn > TXT_SIZEOF(((OSVERSIONINFOEX*)pOutVersionInfo)->szCSDVersion))
							nLn = TXT_SIZEOF(((OSVERSIONINFOEX*)pOutVersionInfo)->szCSDVersion);

						nLn++;

						CHAR* pTgt = ((OSVERSIONINFOEX*)pOutVersionInfo)->szCSDVersion;
						pTgt[0] = 0;
						::WideCharToMultiByte(CP_ACP, 0, ver.szCSDVersion, nLn, pTgt, nLn, NULL, NULL);
						pTgt[(sizeof(((OSVERSIONINFOEX*)pOutVersionInfo)->szCSDVersion) / sizeof(((OSVERSIONINFOEX*)pOutVersionInfo)->szCSDVersion[0])) - 1] = 0;
#endif

						//Done
						bRes = TRUE;
					}
					else
						nOSError = ERROR_INVALID_BLOCK_LENGTH;
				}
				else
					nOSError = ERROR_INSUFFICIENT_BUFFER;
			}
			else
				nOSError = ::GetLastError();

			//Did we fail?
			if(!bRes &&
				bAllowOriginalAPIAsFallback)
			{
				//Use original API as a fall-back
				bRes = ::GetVersionEx(pOutVersionInfo);
				nOSError = ::GetLastError();
			}
		}
		else
			nOSError = ERROR_INVALID_PARAMETER;

		::SetLastError(nOSError);
		return bRes;
	}

	static DWORD ConvertNtStatusToWin32Error(LONG ntstatus)
	{
		//For more info on error codes check:
		//		http://msdn.microsoft.com/en-us/library/cc704588.aspx
		//		http://support.microsoft.com/kb/113996
		//		and
		//		http://www.cprogramming.com/snippets/source-code/convert-ntstatus-win32-error
		//RETURN:
		//		= GetLastError() type error for 'ntstatus', or
		//		= unaltered 'ntstatus' as the last resort
		DWORD result;
		DWORD br = 0;
		OVERLAPPED o = {0};

		o.Internal = ntstatus;

		::GetOverlappedResult(NULL, &o, &br, FALSE);
		result = GetLastError();

		if(result == NO_ERROR)
		{
			//Failed
			result = ntstatus;
		}

		::SetLastError(result);
		return result;
	}


private:
	static BOOL __rtl_get_version(RTL_OSVERSIONINFOEXW& verOut)
	{
		//Try to use kernel-mode API that doesn't "lie" about the results
		BOOL bRes = FALSE;
		int nOSError = NO_ERROR;

		__try
		{
			LONG (WINAPI *pfnRtlGetVersion)(RTL_OSVERSIONINFOEXW*);
			(FARPROC&)pfnRtlGetVersion = GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "RtlGetVersion");
			if(pfnRtlGetVersion)
			{
				verOut.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOEXW);

				LONG status = pfnRtlGetVersion(&verOut);
				if(status == 0)		//STATUS_SUCCESS
				{
					bRes = TRUE;
				}
				else
				{
					//Convert to Win32 error code
					nOSError = ConvertNtStatusToWin32Error(status);
				}
			}
			else
				nOSError = ERROR_NOT_SUPPORTED;
		}
		__except(1)
		{
			//Failed
			bRes = FALSE;
			nOSError = ERROR_ARENA_TRASHED;
		}

		::SetLastError(nOSError);
		return bRes;
	}
};




