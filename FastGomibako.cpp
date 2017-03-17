// FastGomibako.cpp : Defines the entry point for the application.
//

// http://www.flickr.com/photos/bossco/2854452799/

#include "stdafx.h"
#include "resource.h"

#include "CSessionGlobalMemory.h"
#include "../MyUtility/CommandLineParser.h"

#include "MainDlgProc.h"
#include "RetryDlgProc.h"

#define APPLICATION_NAME _T("FastGomibako")

CSessionGlobalMemory<int> gCount("FastGomibakoCounter");



using namespace Ambiesoft;


tstring GetUsageString()
{
	tstring ret;
	ret += _T("\r\n\r\n");
	ret += I18N("Usage");
	ret += _T(":\r\n");
	ret += _T("FastGomibako [File or Folder]");
	return ret;
}






int dowork()
{
	COption optionC(L"-c", 0);
	COption optionDefault(L"");
	CCommandLineParser parser;
	parser.AddOption(&optionC);
	parser.AddOption(&optionDefault);
	parser.Parse(__argc, __targv);

	if(parser.isEmpty())
	{
		tstring message = I18N("No Arguments");
		message += GetUsageString();
		MessageBox(NULL, message.c_str(), APPLICATION_NAME, MB_OK|MB_ICONQUESTION);
		return 0;
	}

	bool quitafterremove = false;
	if(optionC.hadOption())
	{
		quitafterremove = true;
	}

	if(!optionDefault.hadValue())
	{
		tstring message = I18N("No input");
		message += GetUsageString();
		message += L":\r\n";
		message += parser.getUnknowOptionStrings();
		MessageBox(NULL, message.c_str(), APPLICATION_NAME, MB_OK|MB_ICONQUESTION);
		return 1;
	}
	tstring inputfilename = optionDefault.getFirstValue();

	if(parser.hadUnknownOption())
	{
		tstring message = I18N("Unknown Option");
		message += GetUsageString();
		message += L":\r\n";
		message += parser.getUnknowOptionStrings();
		MessageBox(NULL, message.c_str(), APPLICATION_NAME, MB_OK|MB_ICONQUESTION);
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
			throw I18N("Must be a fullpath");
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










		MainDialogData data;
		data.pTarget_ = pFile;
		if(IDOK != DialogBoxParam(GetModuleHandle(NULL),
			MAKEINTRESOURCE(IDD_DIALOG_ASK),
			NULL,
			MainDlgProc,
			(LPARAM)&data))
		{
			return 100;
		}


		bool bComp = data.bRetComp;

/***
		tstring message = string_format(I18N("Are you sure to FastGomibako \"%s\"?"), pFileOrig);
		if(IDYES != MessageBox(NULL, message.c_str(),
			APPLICATION_NAME,MB_ICONQUESTION|MB_YESNO))
		{
			return 3;
		}

		
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
***/
































		CreateDirectory(szGomiDir, NULL);
		dwAttr = GetFileAttributes(szGomiDir);
		if(dwAttr==0xffffffff || (dwAttr & FILE_ATTRIBUTE_DIRECTORY)==0)
		{
			throw I18N("Failed to create FastGomibako directory");
		}

		SetFileAttributes(szGomiDir, dwAttr|FILE_ATTRIBUTE_HIDDEN);

		LPCTSTR pFileName = _tcsrchr(pFileOrig, _T('\\'));
		++pFileName;

		TCHAR szGomiFile[MAX_PATH];
		lstrcpy(szGomiFile, szGomiDir);
		lstrcat(szGomiFile, _T("\\"));
		lstrcat(szGomiFile, pFileName);

		bool movedone=false;
		while(!movedone)
		{
			if(!MoveFile(pFileOrig, szGomiFile))
			{
				tstring t = GetLastErrorString(GetLastError());
				tstring message = I18N("Failed to move file: ");
				message += pFileOrig;
				message += L"\r\n";
				message += t;
				
				RetryDialogData data;
				data.message=message;
				int nDR = DialogBoxParam(GetModuleHandle(NULL),
					MAKEINTRESOURCE(IDD_DIALOG_RETRY),
					NULL,
					RetryDlgProc,
					(LPARAM)&data);

				if(nDR==IDOK)
				{
					// retry
					continue;
				}
				else if(nDR==IDCANCEL)
				{
					return 0;
				}
				else if(nDR==IDC_BUTTON_ELEVATE)
				{
					wstring app = stdGetModuleFileName();
					int nArgc;
					LPCWSTR* ppArgv = (LPCWSTR*)CommandLineToArgvW(GetCommandLineW(), &nArgc);
					wstring arg = stdSplitCommandLine(nArgc, 1, ppArgv);
					LocalFree(ppArgv);

					OpenCommon(NULL,
						app.c_str(),
						arg.c_str(),
						NULL,
						NULL,
						L"runas");
					return 0;
				}
				else
				{
					assert(FALSE);
				}
			}
		}

		SetPriorityClass(GetCurrentProcess(), data.dwRetPri);

		if(!SHDeleteFile(szGomiFile, bComp ? SHDELETE_COMPLETEDELETE : SHDELETE_DEFAULT))
		{
			if(!bComp)
				throw I18N("Failed to trash file");
			else
				throw I18N("Failed to delete file");
		}

		if(!RemoveDirectory(szGomiDir))
		{
			if(gCount <= 1)
				throw I18N("Failed to remove FastGomibako directory");
		}

	}
	catch(tstring& message)
	{
		MessageBox(NULL, message.c_str(), APPLICATION_NAME, MB_OK|MB_ICONERROR);
		return 2;
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
	Ambiesoft::i18nInitLangmap(hInstance, NULL, _T(""));
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
