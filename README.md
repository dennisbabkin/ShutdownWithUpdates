# ShutdownWithUpdates
*Utility To Install Pre-Downloaded Windows Updates &amp; Shutdown/Reboot*

### Release Build

Is available [here](https://dennisbabkin.com/utilities/#ShutdownWithUpdates).

### Build Instructions

The source code was built using Visual Studio 2008. Use `ShutdownWithUpdates.sln` file to open this project in VS 2008. Build it for `Win32` (or `x86`) platform.

Note that this project was not designed, nor was tested to be opened in the version of the Visual Studio later than 2008. Although the resulting binary (executable) can run on a 64-bit operating system, its Visual Studio source project was not designed to be built natively for x86-64 (or 64-bit) platform.

---------------------

![Alt text](https://dennisbabkin.com/php/imgs2/scrsht_shutdownwithupdates.png "ShutdownWithUpdates window")

### Description

Utility that initiates installation of pre-downloaded updates on the Windows system & reboots,
or shuts it down. Note that if Windows updates were not downloaded prior to calling this utility,
the OS will simply perform the power operation.

**Windows 10:** Updates will be installed during a reboot regardless of the options described below.
            Some major updates may require user interaction to proceed.

**Additional:** Major updates, such as Windows Feature Updates, may require user interaction in despite
            of the options specified by this tool. Such requirement is stipulated by Microsoft and
            cannot be overridden by this tool.
            

### Basic Operation

Usage:
> ShutdownWithUpdates [/s | /r | /hs | /g | /a | /?] [/f] [/v] [/nu] [/m \\computer] [/t x] [/c "msg"] [/d [p|u:]xx:yy] [/arso]

-  `/s`    Install updates & shut down computer.
         (Updates must be already downloaded on computer being shut down.)
-  `/r`    Install updates & reboot computer.
         (Updates must be already downloaded on computer being rebooted.)
-  `/hs`   Install updates & initiate hybrid shut-down of computer. (Windows 8,10)
         (Updates must be already downloaded on computer being shut down.)
-  `/g`    Install updates & reboot computer & restart registered applications.
         (Updates must be already downloaded on computer being rebooted.)
-  `/abo`  Go to advanced boot options menu. (Windows 8,10)
         (Pre-Windows 10: Updates will not be installed.)
-  `/a`    Abort previous shut-down/rebooting.
         (Can be used only during previous time-out period.)
-  `/?`    Show command line help.
-  `/f`    Use forced action.
         WARNING: May result in the loss of unsaved data on target computer!
-  `/arso` Enables "Winlogon automatic restart sign-on". (Windows 10)
            [Check here](https://dennisbabkin.com/r/?to=arso) for more info.
-  `/v`    Show user confirmation before proceeding.
         (Local computer only. It is shown before time-out is initiated.)
-  `/nu`   Not to install updates.
         (Windows 10: This option is not supported.)
-  `/m \\computer`    Specify target/remote computer.
-  `/t x`  Set time-out before performing action to x seconds.
         (Valid range is 0-315360000, or 10 yrs, with a default of 0.)
-  `/c "msg"`      Message to be displayed in the interactive shutdown dialog box.
                 (Maximum of 512 characters is allowed.)
-  `/d [p|u:]xx:yy`  Reason for shut-down or rebooting (used for logging):
   -  `p` if action was planned.
   -  `u` if action was user-defined.
      (If neither `p` or `u` is used, assumes unplanned.)
   -  `xx` = major reason number (less than 65536.)
   -  `yy` = minor reason number (greater than 65536.)
                        (Reason numbers can be decimal or hex if begin with `0x`)
        For major and minor reason values check "[System Shutdown Reason Codes](https://dennisbabkin.com/r/?to=win32sdrc)".

Exit Codes:
- `0`      if success.
- `-1`     if general failure in the module.
- `Other`  if error, will contain "[System Error Code](https://dennisbabkin.com/r/?to=win32errs)".

-------------

#### Examples:
1. Install updates and reboot local computer without a delay:
    (Fail if unsaved user data on computer.)

>      ShutdownWithUpdates /r

2. Install updates and shut down local computer after 30 sec delay:
    (Force applications with unsaved data to close & lose data! Show message.)

>      ShutdownWithUpdates /s /f /t 30 /c "Forced shut-down in 30 sec!"

3. Do not install updates and reboot remote computer after a 20 sec delay:
    (Not supported under Windows 10. Fail if unsaved user data on remote computer.
    Specify reason as planned, application issue, installation.)

>      ShutdownWithUpdates /r /nu /m \\MYSERVER /t 20 /d p:0x00040000:0x00000002


--------------


Submit feedback & bug reports [here](https://www.dennisbabkin.com/sfb/?what=info&name=ShutdownWithUpdates).


