#include "stdafx.h"
#include "../lsMisc/stlScopedClear.h"

#include "resource.h"
#include "version.h"
#include "MainDlgProc.h"

using namespace Ambiesoft;
using namespace std;

bool MainDialogData::IsRenameeExists() const 
{
	return !!PathFileExists(renameefull().c_str());
}
wstring MainDialogData::renameefull() const
{
	return stdCombinePath(stdGetParentDirectory(m_pTarget_), renamee_);
}
static void updateDialog(HWND hDlg)
{
	switch (SendDlgItemMessage(hDlg, IDC_COMBO_DELETEMETHOD, CB_GETCURSEL, 0, 0))
	{
	case MainDialogData::Operation_MoveToTrashCan:
		SetWindowText(GetDlgItem(hDlg, IDOK), I18N(L"Remove"));
		EnableWindow(GetDlgItem(hDlg, IDC_EDIT_RENAME), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDC_COMBO_PRIORITY), TRUE);
		break;
	case MainDialogData::Operation_Delete:
		SetWindowText(GetDlgItem(hDlg, IDOK), I18N(L"Delete"));
		EnableWindow(GetDlgItem(hDlg, IDC_EDIT_RENAME), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDC_COMBO_PRIORITY), TRUE);
		break;
	case MainDialogData::Operation_Rename:
		SetWindowText(GetDlgItem(hDlg, IDOK), I18N(L"Rename"));
		EnableWindow(GetDlgItem(hDlg, IDC_EDIT_RENAME), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDC_COMBO_PRIORITY), FALSE);
		break;
	default:
		assert(false);
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

			SetDlgItemText(hDlg, IDC_EDIT_TARGET, spData->m_pTarget_);

			SendDlgItemMessage(hDlg, IDC_COMBO_DELETEMETHOD, CB_ADDSTRING, 0, (LPARAM)I18N(L"Rename"));
			SendDlgItemMessage(hDlg, IDC_COMBO_DELETEMETHOD, CB_ADDSTRING, 0, (LPARAM)I18N(L"Move to trashcan"));
			SendDlgItemMessage(hDlg, IDC_COMBO_DELETEMETHOD, CB_ADDSTRING, 0, (LPARAM)I18N(L"Delete completely"));


			SendDlgItemMessage(hDlg, IDC_COMBO_PRIORITY, CB_ADDSTRING, 0, (LPARAM)I18N(L"High"));
			SendDlgItemMessage(hDlg, IDC_COMBO_PRIORITY, CB_ADDSTRING, 0, (LPARAM)I18N(L"Normal"));
			SendDlgItemMessage(hDlg, IDC_COMBO_PRIORITY, CB_ADDSTRING, 0, (LPARAM)I18N(L"Low"));
			SendDlgItemMessage(hDlg, IDC_COMBO_PRIORITY, CB_ADDSTRING, 0, (LPARAM)I18N(L"Background"));
			
			// set default rename text
			wstring renameText = stdGetFileName(spData->m_pTarget_);
			SetDlgItemText(hDlg, IDC_EDIT_RENAME, renameText.c_str());

			sIni = stdGetModuleFileName() + L".ini";
			int nDeleteMethod = GetPrivateProfileInt(APPNAME, KEY_DELETEMETHOD, 0, sIni.c_str());
			int nPriority = GetPrivateProfileInt(APPNAME, KEY_PRIORITY, 1, sIni.c_str());
			SendDlgItemMessage(hDlg, IDC_COMBO_DELETEMETHOD, CB_SETCURSEL, nDeleteMethod, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_PRIORITY, CB_SETCURSEL, nPriority, 0);

			wstring title = _T("FastGomibako");
			if (IsUserAnAdmin())
			{
				title += L" (";
				title += I18N(L"Admin");
				title += L")";
			}
			SetWindowText(hDlg, title.c_str());
			
			updateDialog(hDlg);
			CenterWindow(hDlg);

			HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON_MAIN));
			SendMessage(hDlg, WM_SETICON, TRUE, (LPARAM)hIcon);
			SendMessage(hDlg, WM_SETICON, FALSE, (LPARAM)hIcon);

			return TRUE;
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
							updateDialog(hDlg);
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
								string_format(I18N(L"\"%s\" already exists."), spData->renameefull().c_str()).c_str(),
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
