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




#include "stdafx.h"
#include "Main.h"


int _tmain(int argc, _TCHAR* argv[])
{
	//RETURN:
	//		= Exit code from the main process:
	//			= 0 if success
	//			= -1 if general exception in the process
	//			= Other - OS Error code

	return CMain::doWork(argc, argv);
}

