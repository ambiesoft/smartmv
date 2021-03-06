//BSD 2-Clause License
//
//Copyright (c) 2018, Ambiesoft
//All rights reserved.
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions are met:
//
//* Redistributions of source code must retain the above copyright notice, this
//  list of conditions and the following disclaimer.
//
//* Redistributions in binary form must reproduce the above copyright notice,
//  this list of conditions and the following disclaimer in the documentation
//  and/or other materials provided with the distribution.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
//FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
//OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <shlwapi.h>
#include <Shlobj.h>

#include <assert.h>
#include <process.h>  

#include <string>
#include <set>

#include "../lsMisc/I18N.h"
#include "../lsMisc/SHMoveFile.h"
#include "../lsMisc/IsWindowsNT.h"
#include "../lsMisc/tstring.h"
#include "../lsMisc/OpenCommon.h"
#include "../lsMisc/CenterWindow.h"
#include "../lsMisc/WritePrivateProfileInt.h"
#include "../lsMisc/GetLastErrorString.h"
#include "../lsMisc/stdwin32/stdwin32.h"
#include "../lsMisc/stdosd/stdosd.h"
#include "../lsMisc/SessionGlobalMemory/SessionGlobalMemory.h"
#include "../lsMisc/Is64.h"
#include "../lsMisc/CommandLineUtil.h"
// #include "../lsMisc/ntfs.h"
using namespace Ambiesoft::stdwin32;




#define I18N(s) Ambiesoft::I18N(s)

#define APPNAME _T("smartmv")
#define KEY_DELETEMETHOD	_T("DeleteMethod")
#define KEY_PRIORITY		_T("Priority")

enum {
	WM_APP_RETRYDIALOG_FINDCULPLIT_THREADCREATIONFAILED = (WM_APP+1),
	WM_APP_RETRYDIALOG_FINDCULPLIT_FOUND,
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
