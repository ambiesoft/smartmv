// FastGomibako.cpp : Defines the entry point for the application.
//

// http://www.flickr.com/photos/bossco/2854452799/

#include "stdafx.h"
#include "shlwapi.h"
#pragma comment (lib, "shlwapi.lib")

#include "../MyUtility/SHDeleteFile.h"

#define APPLICATION_NAME _T("FastGomibako")

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR     lpCmdLine,
                     int       nCmdShow )
{
	if(__argc  <= 1)
	{
		MessageBox(NULL, _T("No Arguments"), APPLICATION_NAME, MB_OK|MB_ICONQUESTION);
		return 0;
	}

	if(__argc > 2)
	{
		MessageBox(NULL, _T("Too many Arguments"), APPLICATION_NAME, MB_OK|MB_ICONQUESTION);
		return 1;
	}

	LPCTSTR pFileOrig = _tcsdup(__targv[1]);
	LPTSTR pFile = _tcsdup(__targv[1]);
	_tcslwr(pFile);
	try
	{
		if( !(_T('A') <= pFile[0] || pFile[0] <= _T('Z')) )
		{
			throw _T("Invalid Argument");
		}

		if(pFile[1] != _T(':') || pFile[2] != _T('\\'))
		{
			throw _T("Invalid Argument");
		}

		if(pFile[3]==0)
		{
			throw _T("Root Drive unacceptable");
		}

		DWORD dwAttr = GetFileAttributes(pFile);
		if(dwAttr == 0xffffffff)
		{
			throw _T("File Not Found");
		}

		TCHAR szGomiDir[32];
		szGomiDir[0] = pFile[0];
		szGomiDir[1] = _T(':');
		szGomiDir[2] = _T('\\');
		lstrcpy(&szGomiDir[3], _T(".FastGomibako"));

		CreateDirectory(szGomiDir, NULL);
		dwAttr = GetFileAttributes(szGomiDir);
		if(dwAttr==0xffffffff || (dwAttr & FILE_ATTRIBUTE_DIRECTORY)==0)
		{
			throw _T("Failed to create FastGomibako directory");
		}

		LPCTSTR pFileName = _tcsrchr(pFileOrig, _T('\\'));
		++pFileName;

		TCHAR szGomiFile[MAX_PATH];
		lstrcpy(szGomiFile, szGomiDir);
		lstrcat(szGomiFile, _T("\\"));
		lstrcat(szGomiFile, pFileName);

		if(!MoveFile(pFileOrig, szGomiFile))
		{
			throw _T("Failed to move file");
		}

		if(!SHDeleteFile(szGomiFile))
		{
			throw _T("Failed to trash file");
		}

		if(!RemoveDirectory(szGomiDir))
		{
			throw _T("Failed to remove FastGomibako directory");
		}

	}
	catch(LPCTSTR pMessage)
	{
		MessageBox(NULL, pMessage, APPLICATION_NAME, MB_OK|MB_ICONERROR);
		return 2;
	}
	catch(...)
	{
		MessageBox(NULL, _T("Invalid Argument"), APPLICATION_NAME, MB_OK|MB_ICONQUESTION);
		return 3;
	}
	return 0;
}



