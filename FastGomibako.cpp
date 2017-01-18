// FastGomibako.cpp : Defines the entry point for the application.
//

// http://www.flickr.com/photos/bossco/2854452799/

#include "stdafx.h"
#include "resource.h"




#include "CSessionGlobalMemory.h"


#define APPLICATION_NAME _T("FastGomibako")

CSessionGlobalMemory<int> gCount("FastGomibakoCounter");

tstring GetUsageString()
{
	tstring ret;
	ret += _T("\r\n\r\n");
	ret += I18N("Usage");
	ret += _T(":\r\n");
	ret += _T("FastGomibako [File or Folder]");
	return ret;
}


struct MyDialogData {
	LPCTSTR pTarget_;

	bool bRetComp;
	DWORD dwRetPri;

	MyDialogData() {
		ZeroMemory(this, sizeof(*this));
		dwRetPri=GetPriorityClass(GetCurrentProcess());
	}
};
BOOL CALLBACK MyDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static MyDialogData* spData;
	static wstring sIni;
	switch(uMsg)
	{
		case WM_INITDIALOG:
		{

			spData = (MyDialogData*)lParam;
			
			SetDlgItemText(hDlg, IDC_EDIT_TARGET, spData->pTarget_);

			SendDlgItemMessage(hDlg, IDC_COMBO_DELETEMETHOD, CB_ADDSTRING, 0, (LPARAM)I18N("Move to trashcan"));
			SendDlgItemMessage(hDlg, IDC_COMBO_DELETEMETHOD, CB_ADDSTRING, 0, (LPARAM)I18N("Delete completely"));


			SendDlgItemMessage(hDlg, IDC_COMBO_PRIORITY, CB_ADDSTRING, 0, (LPARAM)I18N("High"));
			SendDlgItemMessage(hDlg, IDC_COMBO_PRIORITY, CB_ADDSTRING, 0, (LPARAM)I18N("Normal"));
			SendDlgItemMessage(hDlg, IDC_COMBO_PRIORITY, CB_ADDSTRING, 0, (LPARAM)I18N("Low"));
			SendDlgItemMessage(hDlg, IDC_COMBO_PRIORITY, CB_ADDSTRING, 0, (LPARAM)I18N("Background"));
			

			sIni = stdGetModuleFileName() + L".ini";
			int nDeleteMethod = GetPrivateProfileInt(APPNAME, KEY_DELETEMETHOD, 0, sIni.c_str());
			int nPriority = GetPrivateProfileInt(APPNAME, KEY_PRIORITY, 1, sIni.c_str());
			SendDlgItemMessage(hDlg, IDC_COMBO_DELETEMETHOD, CB_SETCURSEL, nDeleteMethod, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_PRIORITY, CB_SETCURSEL, nPriority, 0);

			SetWindowText(hDlg, _T("FastGomibako"));
			CenterWindow(hDlg);
			return TRUE;
		}
		break;

		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case IDOK:
				{
					const int nDel = SendDlgItemMessage(hDlg, IDC_COMBO_DELETEMETHOD, CB_GETCURSEL, 0, 0);
					const int nPri = SendDlgItemMessage(hDlg, IDC_COMBO_PRIORITY, CB_GETCURSEL, 0, 0);
					if(nDel==CB_ERR || nPri==CB_ERR)
					{
						EndDialog(hDlg, IDCANCEL);
						return 0;
					}

					spData->bRetComp = nDel==1;
					switch(nPri)
					{
						case 0:
							spData->dwRetPri = HIGH_PRIORITY_CLASS;
							break;
						case 1:
							spData->dwRetPri = NORMAL_PRIORITY_CLASS;
							break;
						case 2:
							spData->dwRetPri = 0x00004000; // BELOW_NORMAL_PRIORITY_CLASS;
							break;
						case 3:
							if(IsWinVistaOrHigher())
								spData->dwRetPri = 0x00100000; // PROCESS_MODE_BACKGROUND_BEGIN;
							else
								spData->dwRetPri = IDLE_PRIORITY_CLASS;
							break;
					}							

					WritePrivateProfileInt(APPNAME, KEY_DELETEMETHOD, nDel, sIni.c_str());
					WritePrivateProfileInt(APPNAME, KEY_PRIORITY, nPri, sIni.c_str());

					EndDialog(hDlg, IDOK);
					return 0;
				}
				break;

				case IDCANCEL:
				{
					EndDialog(hDlg, IDCANCEL);
					return 0;
				}
				break;

			}
			break;
		}
		break;
	}
	return FALSE;
}

#include "../MyUtility/CommandLineParser.h"
using namespace Ambiesoft;

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










		MyDialogData data;
		data.pTarget_ = pFile;
		if(IDOK != DialogBoxParam(GetModuleHandle(NULL),
			MAKEINTRESOURCE(IDD_DIALOG_ASK),
			NULL,
			MyDlgProc,
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


		if(!MoveFile(pFileOrig, szGomiFile))
		{
			tstring t = GetLastErrorString(GetLastError());
			tstring message = I18N("Failed to move file");
			message += L"\r\n";
			message += t;
			throw message;
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
