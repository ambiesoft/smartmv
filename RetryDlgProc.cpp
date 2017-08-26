#include "stdafx.h"
#include "../MyUtility/OpenedFiles.h"

#include "resource.h"

#include "RetryDlgProc.h"

struct THREADPASSDATA
{
	HWND hDlg;
	wstring file;
};

unsigned int __stdcall startofthread(void* pContext)
{
	THREADPASSDATA tpd = *(THREADPASSDATA*)pContext;
	delete (THREADPASSDATA*)pContext;
	
	vector<OPENEDFILEINFO> v;
	GetOpenedFilesSimple(tpd.file.c_str(), v);

	if(IsWindow(tpd.hDlg))
	{
		SendMessage(tpd.hDlg, WM_APP_RETRYDIALOG_FINDCULPLIT_FOUND, (WPARAM)&v, NULL);
	}
	return 0;
}

static wstring getDlgItemText(HWND hdlg, int id)
{
	wstring ret;
	HWND h = GetDlgItem(hdlg,id);
	if(!h)
		return ret;

	int len=GetWindowTextLength(h);
	if(len==0)
		return ret;

	vector<TCHAR> szT;
	szT.resize(len+1);
	GetWindowText(h, &szT[0], len+1);

	ret = &szT[0];
	return ret;
}
		
INT_PTR CALLBACK RetryDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static RetryDialogData* spData;
	static HANDLE shFindingCulplit;
	switch(uMsg)
	{
		case WM_INITDIALOG:
		{
			spData = (RetryDialogData*)lParam;

			wstring title = _T("FastGomibako");
			if (IsUserAnAdmin())
			{
				title += L" (";
				title += I18N("Admin");
				title += L")";
			}
			SetWindowText(hDlg, title.c_str());

			wstring message;
			message.append(spData->message);
			message.append(L"\r\n");
			message.append(L"\r\n");

			message.append(I18N("Finding culplit..."));

			SetDlgItemText(hDlg, IDC_EDIT_MESSAGE, message.c_str());

			THREADPASSDATA* pThreadPass = new THREADPASSDATA;
			pThreadPass->hDlg=hDlg;
			pThreadPass->file = spData->file;

			shFindingCulplit = (HANDLE)_beginthreadex(
				NULL,              // security
				0,                 // stack size
				startofthread,     // start func
				(void*)pThreadPass,// parameter
				CREATE_SUSPENDED,  // init flag
				NULL               // thread id (ret)
				);

			if(shFindingCulplit==NULL)
			{
				PostMessage(hDlg, WM_APP_RETRYDIALOG_FINDCULPLIT_THREADCREATIONFAILED,0,0);
			}
			else
			{
				ResumeThread(shFindingCulplit);
			}

			EnableWindow(GetDlgItem(hDlg,IDC_BUTTON_ELEVATE), !IsUserAnAdmin());
			CenterWindow(hDlg);
			return TRUE;
		}
		break;

		case WM_APP_RETRYDIALOG_FINDCULPLIT_FOUND:
		{
			wstring grabbers;
			vector<OPENEDFILEINFO>* pV = (vector<OPENEDFILEINFO>*)wParam;
			for(vector<OPENEDFILEINFO>::iterator it = pV->begin();
				it != pV->end();
				++it)
			{
				wstring grabber = GetPathFromProcessID(it->dwPID);
				if(!grabber.empty())
				{
					grabbers.append(grabber);
					grabbers.append(L"\r\n");
				}
			}

			wstring message=L"\r\n";
			if(grabbers.empty())
				message.append(I18N("No culplits found."));
			else
			{
				message.append(I18N("Following applications grab the file, close them and try again."));
				message.append(L"\r\n");
				message.append(grabbers);
			}

			wstring txt = getDlgItemText(hDlg,IDC_EDIT_MESSAGE);
			txt.append(message);
			SetDlgItemText(hDlg, IDC_EDIT_MESSAGE, txt.c_str());
		}
		break;

		case WM_APP_RETRYDIALOG_FINDCULPLIT_THREADCREATIONFAILED:
		{
			wstring current = getDlgItemText(hDlg, IDC_EDIT_MESSAGE);
			current.append(I18N("Failed to create thread."));
			SetDlgItemText(hDlg, IDC_EDIT_MESSAGE, current.c_str());
		}
		break;

		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case IDOK:
				case IDCANCEL:
				case IDC_BUTTON_ELEVATE:
				{
					EndDialog(hDlg, LOWORD(wParam));
					return 0;
				}
				break;

			}
			break;
		}
		break;

		case WM_DESTROY:
		{
			TerminateThread(shFindingCulplit, -1);
			CloseHandle(shFindingCulplit);
			shFindingCulplit=NULL;
		}
		break;
	}
	return FALSE;
}
