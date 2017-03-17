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

#include "../MyUtility/I18N.h"
#include "../MyUtility/SHDeleteFile.h"
#include "../MyUtility/IsWindowsNT.h"
#include "../MyUtility/tstring.h"
#include "../MyUtility/OpenCommon.h"
#include "../MyUtility/CenterWindow.h"
#include "../MyUtility/WritePrivateProfileInt.h"
#include "../MyUtility/GetLastErrorString.h"
#include "../MyUtility/stdwin32/stdwin32.h"
using namespace stdwin32;

#include <string>
using namespace std;

#define I18N(s) Ambiesoft::I18N(_T(s))

#define APPNAME _T("FastGomibako")
#define KEY_DELETEMETHOD	_T("DeleteMethod")
#define KEY_PRIORITY		_T("Priority")

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
