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
#include "../lsMisc/stlScopedClear.h"

#include "resource.h"
#include "version.h"
#include "MainDlgProc.h"

using namespace Ambiesoft;
using namespace Ambiesoft::stdosd;
using namespace std;

bool MainDialogData::IsRenameeExists() const 
{
	return !!PathFileExists(renameefull().c_str());
}
wstring MainDialogData::renameefull() const
{
	assert(targets_.size() == 1);
	return stdCombinePath(stdGetParentDirectory(targets_[0]), renamee_);
}
static void updateDialog(HWND hDlg, bool isSingle)
{
	LRESULT comboValue = SendDlgItemMessage(hDlg, IDC_COMBO_DELETEMETHOD, CB_GETCURSEL, 0, 0);
	switch (comboValue)
	{
	case MainDialogData::Operation_MoveToTrashCan:
		SetWindowText(GetDlgItem(hDlg, IDOK), I18N(L"Remo&ve"));
		EnableWindow(GetDlgItem(hDlg, IDC_EDIT_RENAME), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDC_COMBO_PRIORITY), TRUE);
		break;
	case MainDialogData::Operation_Delete:
		SetWindowText(GetDlgItem(hDlg, IDOK), I18N(L"&Delete"));
		EnableWindow(GetDlgItem(hDlg, IDC_EDIT_RENAME), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDC_COMBO_PRIORITY), TRUE);
		break;
	case MainDialogData::Operation_Rename:
		SetWindowText(GetDlgItem(hDlg, IDOK), I18N(L"Re&name"));
		EnableWindow(GetDlgItem(hDlg, IDC_EDIT_RENAME), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDC_COMBO_PRIORITY), FALSE);
		break;
	default:
		assert(false);
	}
	
	if (!isSingle)
	{
		if (comboValue == MainDialogData::Operation_Rename)
		{
			EnableWindow(GetDlgItem(hDlg, IDOK), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_EDIT_RENAME), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_COMBO_PRIORITY), FALSE);
		}
		else
		{
			EnableWindow(GetDlgItem(hDlg, IDOK), TRUE);
		}
	}
}
INT_PTR CALLBACK MainDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static MainDialogData* spData;
	static wstring sIni;
	switch(uMsg)
	{
		case WM_INITDIALOG:
		{
			spData = (MainDialogData*)lParam;
			
			i18nChangeChildWindowText(hDlg);

			wstring editTexts;
			for (auto&& path : spData->targets_)
			{
				editTexts += L"\"" + path + L"\"";
				editTexts += L" ";
			}
			editTexts=stdTrim(editTexts);
			SetDlgItemText(hDlg, IDC_EDIT_TARGET, editTexts.c_str());

			SendDlgItemMessage(hDlg, IDC_COMBO_DELETEMETHOD, CB_ADDSTRING, 0, (LPARAM)I18N(L"Rename"));
			SendDlgItemMessage(hDlg, IDC_COMBO_DELETEMETHOD, CB_ADDSTRING, 0, (LPARAM)I18N(L"Move to trashcan"));
			SendDlgItemMessage(hDlg, IDC_COMBO_DELETEMETHOD, CB_ADDSTRING, 0, (LPARAM)I18N(L"Delete completely"));


			SendDlgItemMessage(hDlg, IDC_COMBO_PRIORITY, CB_ADDSTRING, 0, (LPARAM)I18N(L"High"));
			SendDlgItemMessage(hDlg, IDC_COMBO_PRIORITY, CB_ADDSTRING, 0, (LPARAM)I18N(L"Normal"));
			SendDlgItemMessage(hDlg, IDC_COMBO_PRIORITY, CB_ADDSTRING, 0, (LPARAM)I18N(L"Low"));
			SendDlgItemMessage(hDlg, IDC_COMBO_PRIORITY, CB_ADDSTRING, 0, (LPARAM)I18N(L"Background"));
			
			// set default rename text
			if (spData->IsSingleFile())
			{
				// wstring renameText = stdGetFileName(spData->targets_[0]);
				wstring renameText = !spData->renamee().empty() ? spData->renamee() : stdGetFileName(spData->targets_[0]);
				SetDlgItemText(hDlg, IDC_EDIT_RENAME, renameText.c_str());
			}

			sIni = stdGetModuleFileName<wchar_t>() + L".ini";
			MainDialogData::Operation operation = spData->m_op;
			if(operation == MainDialogData::Operation::Operation_Default)
				operation = (MainDialogData::Operation)GetPrivateProfileInt(APPNAME, KEY_DELETEMETHOD, 0, sIni.c_str());
			switch (operation)
			{
			case MainDialogData::Operation::Operation_Rename:
			case MainDialogData::Operation::Operation_MoveToTrashCan:
			case MainDialogData::Operation::Operation_Delete:
				SendDlgItemMessage(hDlg, IDC_COMBO_DELETEMETHOD, CB_SETCURSEL, operation, 0);
				break;
			}

			int nPriority = GetPrivateProfileInt(APPNAME, KEY_PRIORITY, 1, sIni.c_str());
			SendDlgItemMessage(hDlg, IDC_COMBO_PRIORITY, CB_SETCURSEL, nPriority, 0);

			wstring title = APPNAME;
			if (IsUserAnAdmin())
			{
				title += L" (";
				title += I18N(L"Admin");
				title += L")";
			}
			SetWindowText(hDlg, title.c_str());
			

			updateDialog(hDlg,spData->IsSingleFile());

			BOOL ret = TRUE;
			if (operation == MainDialogData::Operation::Operation_Rename)
			{
				// focus edit and select all
				SetFocus(GetDlgItem(hDlg, IDC_EDIT_RENAME));
				SendMessage(GetDlgItem(hDlg, IDC_EDIT_RENAME), EM_SETSEL, 0, -1);

				// keep focus to this
				ret = FALSE;
			}

			CenterWindow(hDlg);

			HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON_MAIN));
			SendMessage(hDlg, WM_SETICON, TRUE, (LPARAM)hIcon);
			SendMessage(hDlg, WM_SETICON, FALSE, (LPARAM)hIcon);


			return ret;
		}
		break;

		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case IDC_COMBO_DELETEMETHOD:
				{
					switch (HIWORD(wParam))
					{
						case CBN_SELENDOK:
						{
							// combo op selected
							updateDialog(hDlg,spData->IsSingleFile());
						}
						break;
					}
				}
				break;

				case IDOK:
				{
					const int nDel = (int)SendDlgItemMessage(hDlg, IDC_COMBO_DELETEMETHOD, CB_GETCURSEL, 0, 0);
					const int nPri = (int)SendDlgItemMessage(hDlg, IDC_COMBO_PRIORITY, CB_GETCURSEL, 0, 0);
					if(nDel==CB_ERR || nPri==CB_ERR)
					{
						EndDialog(hDlg, IDCANCEL);
						return 0;
					}

					spData->m_op = (MainDialogData::Operation)nDel;
					if (spData->IsRename())
					{
						int len = GetWindowTextLength(GetDlgItem(hDlg, IDC_EDIT_RENAME));
						if (len <= 0)
						{
							MessageBox(hDlg, I18N(L"Please enter renaming name."), APPNAME, MB_ICONEXCLAMATION);
							break;
						}
						LPTSTR pBuff = (LPTSTR)LocalAlloc(0, (len + 1)*sizeof(TCHAR));
						STLSOFT_SCOPEDFREE(pBuff, HLOCAL, LocalFree);
						GetWindowText(GetDlgItem(hDlg, IDC_EDIT_RENAME), pBuff, len+1);
						spData->setRenamee(pBuff);
						if (spData->IsRenameeExists())
						{
							MessageBox(NULL,
								stdFormat(I18N(L"\"%s\" already exists."), spData->renameefull().c_str()).c_str(),
								APPNAME,
								MB_ICONEXCLAMATION);
							break;
						}
					}
					switch(nPri)
					{
						case 0:
							spData->m_dwRetPri = HIGH_PRIORITY_CLASS;
							break;
						case 1:
							spData->m_dwRetPri = NORMAL_PRIORITY_CLASS;
							break;
						case 2:
							spData->m_dwRetPri = 0x00004000; // BELOW_NORMAL_PRIORITY_CLASS;
							break;
						case 3:
							if(IsWinVistaOrHigher())
								spData->m_dwRetPri = 0x00100000; // PROCESS_MODE_BACKGROUND_BEGIN;
							else
								spData->m_dwRetPri = IDLE_PRIORITY_CLASS;
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

				case IDC_BUTTON_ABOUT:
				{
					MessageBox(hDlg, APPNAME L" ver " VERSION, APPNAME, MB_ICONINFORMATION);
				}
				break;

			}
			break;
		}
		break;
	}
	return FALSE;
}
