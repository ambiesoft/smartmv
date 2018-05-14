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
#include "../SessionGlobalMemory/SessionGlobalMemory.h"
#include "../lsMisc/CommandLineParser.h"
#include "../lsMisc/Is64.h"
#include "../lsMisc/CommandLineUtil.h"
// #include "../lsMisc/ntfs.h"
using namespace stdwin32;




#define I18N(s) Ambiesoft::I18N(s)

#define APPNAME _T("FastGomibako")
#define KEY_DELETEMETHOD	_T("DeleteMethod")
#define KEY_PRIORITY		_T("Priority")

enum {
	WM_APP_RETRYDIALOG_FINDCULPLIT_THREADCREATIONFAILED = (WM_APP+1),
	WM_APP_RETRYDIALOG_FINDCULPLIT_FOUND,
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
