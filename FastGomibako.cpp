// FastGomibako.cpp : Defines the entry point for the application.
//

// http://www.flickr.com/photos/bossco/2854452799/

#include "stdafx.h"


#include "shlwapi.h"
#pragma comment (lib, "shlwapi.lib")

#include "CSessionGlobalMemory.h"


#define APPLICATION_NAME _T("FastGomibako")

CSessionGlobalMemory<int> gCount("FastGomibakoCounter");

int dowork()
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
			throw I18N("Invalid Argument");
		}

		if(pFile[1] != _T(':') || pFile[2] != _T('\\'))
		{
			throw I18N("Invalid Argument");
		}

		if(pFile[3]==0)
		{
			throw I18N("Root Drive unacceptable");
		}

		DWORD dwAttr = GetFileAttributes(pFile);
		if(dwAttr == 0xffffffff)
		{
			throw I18N("File Not Found");
		}

		TCHAR szGomiDir[32];
		szGomiDir[0] = pFile[0];
		szGomiDir[1] = _T(':');
		szGomiDir[2] = _T('\\');
		lstrcpy(&szGomiDir[3], _T(".FastGomibako"));

		tstring message = string_format(I18N("Are you sure to FastGomibako \"%s\"?"), pFileOrig);
		if(IDYES != MessageBox(NULL, message.c_str(),
			APPLICATION_NAME,MB_ICONQUESTION|MB_YESNO))
		{
			return 3;
		}

		bool bComp = false;
		if(GetAsyncKeyState(VK_SHIFT)<0)
		{
			message = string_format(I18N("Are you sure to delete completely \"%s\"?"), pFileOrig);
			if(IDYES != MessageBox(NULL, message.c_str(),
				APPLICATION_NAME,MB_ICONEXCLAMATION|MB_YESNO|MB_DEFBUTTON2))
			{
				return 4;
			}
			bComp = true;
		}

		CreateDirectory(szGomiDir, NULL);
		dwAttr = GetFileAttributes(szGomiDir);
		if(dwAttr==0xffffffff || (dwAttr & FILE_ATTRIBUTE_DIRECTORY)==0)
		{
			throw _T("Failed to create FastGomibako directory");
		}

		SetFileAttributes(szGomiDir, dwAttr|FILE_ATTRIBUTE_HIDDEN);

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

		if(!SHDeleteFile(szGomiFile, FALSE, bComp ? TRUE : FALSE))
		{
			throw _T("Failed to trash file");
		}

		if(!RemoveDirectory(szGomiDir))
		{
			if(gCount <= 1)
				throw I18N("Failed to remove FastGomibako directory");
		}

	}
	catch(LPCTSTR pMessage)
	{
		MessageBox(NULL, pMessage, APPLICATION_NAME, MB_OK|MB_ICONERROR);
		return 2;
	}
	catch(...)
	{
		MessageBox(NULL, I18N("Invalid Argument"), APPLICATION_NAME, MB_OK|MB_ICONQUESTION);
		return 3;
	}
	return 0;
}



int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR     lpCmdLine,
                     int       nCmdShow )
{

	int ret=0;
	try
	{
		gCount=gCount+1;
		ret= dowork();
	}
	catch(...)
	{
		ret=-1;
	}
	gCount=gCount-1;
	return ret;
}
