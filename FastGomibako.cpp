// FastGomibako.cpp : Defines the entry point for the application.
//

// http://www.flickr.com/photos/bossco/2854452799/

#include "stdafx.h"
#include "resource.h"

#include "shlwapi.h"
#pragma comment (lib, "shlwapi.lib")

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
	switch(uMsg)
	{
		case WM_INITDIALOG:
		{
			spData = (MyDialogData*)lParam;
			
			SetDlgItemText(hDlg, IDC_EDIT_TARGET, spData->pTarget_);

			SendDlgItemMessage(hDlg, IDC_COMBO_DELETEMETHOD, CB_ADDSTRING, 0, (LPARAM)I18N("Move to trashcan"));
			SendDlgItemMessage(hDlg, IDC_COMBO_DELETEMETHOD, CB_ADDSTRING, 0, (LPARAM)I18N("Delete completely"));
			SendDlgItemMessage(hDlg, IDC_COMBO_DELETEMETHOD, CB_SETCURSEL, 0, 0);

			SendDlgItemMessage(hDlg, IDC_COMBO_PRIORITY, CB_ADDSTRING, 0, (LPARAM)I18N("High"));
			SendDlgItemMessage(hDlg, IDC_COMBO_PRIORITY, CB_ADDSTRING, 0, (LPARAM)I18N("Normal"));
			SendDlgItemMessage(hDlg, IDC_COMBO_PRIORITY, CB_ADDSTRING, 0, (LPARAM)I18N("Low"));
			SendDlgItemMessage(hDlg, IDC_COMBO_PRIORITY, CB_ADDSTRING, 0, (LPARAM)I18N("Background"));
			SendDlgItemMessage(hDlg, IDC_COMBO_PRIORITY, CB_SETCURSEL, 1, 0);

			SetWindowText(hDlg, _T("FastGomibako"));

			return TRUE;
		}
		break;

		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case IDOK:
				{
					int nDel = SendDlgItemMessage(hDlg, IDC_COMBO_DELETEMETHOD, CB_GETCURSEL, 0, 0);
					int nPri = SendDlgItemMessage(hDlg, IDC_COMBO_PRIORITY, CB_GETCURSEL, 0, 0);
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

int dowork()
{
	if(__argc  <= 1)
	{
		tstring message = I18N("No Arguments");
		message += GetUsageString();
		MessageBox(NULL, message.c_str(), APPLICATION_NAME, MB_OK|MB_ICONQUESTION);
		return 0;
	}

	if(__argc > 2)
	{
		tstring message = I18N("Too many Arguments");
		message += GetUsageString();
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
		SetPriorityClass(GetCurrentProcess(), data.dwRetPri);
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
			throw I18N("Failed to move file");
		}

		if(!SHDeleteFile(szGomiFile, FALSE, bComp ? TRUE : FALSE))
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
