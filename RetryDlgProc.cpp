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

#include "stdafx.h"
#include "../lsMisc/OpenedFiles.h"

#include "resource.h"

#include "RetryDlgProc.h"

using namespace Ambiesoft;
using namespace Ambiesoft::stdosd;
using namespace std;

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
	static set<DWORD> pidsToKill;
	static HWND btnTerminate;
	switch(uMsg)
	{
		case WM_INITDIALOG:
		{
			btnTerminate = GetDlgItem(hDlg, IDC_BUTTON_TERMINATE);
			pidsToKill.clear();
			EnableWindow(btnTerminate, FALSE);

			spData = (RetryDialogData*)lParam;

			i18nChangeChildWindowText(hDlg);

			wstring title = APPNAME;
			if (IsUserAnAdmin())
			{
				title += L" (";
				title += I18N(L"Admin");
				title += L")";
			}
			SetWindowText(hDlg, title.c_str());

			wstring message;
			message.append(spData->message());
			message.append(L"\r\n");
			message.append(L"\r\n");

			message.append(I18N(L"Finding the process grabbing the file..."));

			SetDlgItemText(hDlg, IDC_EDIT_MESSAGE, message.c_str());

			THREADPASSDATA* pThreadPass = new THREADPASSDATA;
			pThreadPass->hDlg=hDlg;
			pThreadPass->file = spData->file();

			EnableDebugPriv();
				
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

			EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_ELEVATE), !IsUserAnAdmin());
			ShowWindow(GetDlgItem(hDlg, IDC_BUTTON_RUNAS_DIFFERENTCPU), Is64BitWindows() ? SW_SHOW : SW_HIDE);
			SetDlgItemText(hDlg, IDC_BUTTON_RUNAS_DIFFERENTCPU,
				Is64BitProcess() ? I18N(L"Run &32bit") : I18N(L"Run &64bit"));

			HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON_MAIN));
			SendMessage(hDlg, WM_SETICON, TRUE, (LPARAM)hIcon);
			SendMessage(hDlg, WM_SETICON, FALSE, (LPARAM)hIcon);


			CenterWindow(hDlg);
			return TRUE;
		}
		break;

		case WM_APP_RETRYDIALOG_FINDCULPLIT_FOUND:
		{
			wstring grabbers;
			set<DWORD> sets;

			vector<OPENEDFILEINFO>* pV = (vector<OPENEDFILEINFO>*)wParam;
			for(vector<OPENEDFILEINFO>::iterator it = pV->begin();
				it != pV->end();
				++it)
			{
				wstring grabber = GetPathFromProcessID(it->dwPID);
				if(!grabber.empty())
				{
					// check already sets
					if (sets.find(it->dwPID) == sets.end())
					{
						sets.insert(it->dwPID);
						pidsToKill.insert(it->dwPID);
						EnableWindow(btnTerminate, TRUE);

						grabbers.append(L"PID:");
						grabbers.append(to_wstring(it->dwPID));
						grabbers.append(L", ");
						grabbers.append(stdApplyDQ(grabber));
						if (it->filename[0])
						{
							wstring t = stdFormat(I18N(L"grabbing \"%s\""), it->filename);
							grabbers.append(L" (");
							grabbers.append(t);
							grabbers.append(L")");
						}
						grabbers.append(L"\r\n");
					}
				}
			}

			wstring message=L"\r\n";
			if(grabbers.empty())
				message.append(I18N(L"No culplits found."));
			else
			{
				message.append(I18N(L"Following applications grab the file(s), close them and try again."));
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
			current.append(I18N(L"Failed to create thread."));
			SetDlgItemText(hDlg, IDC_EDIT_MESSAGE, current.c_str());
		}
		break;

		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case IDOK:
				case IDC_BUTTON_ELEVATE:
				case IDC_BUTTON_RUNAS_DIFFERENTCPU:
				case IDCANCEL:
				{
					EndDialog(hDlg, LOWORD(wParam));
					return 0;
				}
				break;

				case IDC_BUTTON_TERMINATE:
				{
					if (pidsToKill.empty())
						break;

					wstring question = stdFormat(I18N(L"Are you sure to terminate following process(es)?"), pidsToKill.size());
					question += L"\r\n";
					for (DWORD pid : pidsToKill)
					{
						question += to_wstring(pid);
						question += L" ";
					}
					if (IDYES != MessageBox(hDlg,
						question.c_str(),
						APPNAME,
						MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2))
					{
						break;
					}

					int nFailed = 0;
					set<DWORD> tmpPids(pidsToKill);
					for (DWORD pid : tmpPids)
					{
						HANDLE h = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
						if (!h)
						{
							++nFailed;
							continue;
						}
						if (!TerminateProcess(h, -1))
						{
							++nFailed;
							continue;
						}
						pidsToKill.erase(pid);

						wstring message = stdFormat(I18N(L"Process %d has been terminated."), pid);
						message += L"\r\n";
						
						wstring txt = getDlgItemText(hDlg, IDC_EDIT_MESSAGE);
						txt.append(message);
						SetDlgItemText(hDlg, IDC_EDIT_MESSAGE, txt.c_str());
					}
					if (nFailed == 0)
					{
						EnableWindow(btnTerminate, FALSE);
					}

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
