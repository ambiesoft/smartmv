#include "stdafx.h"
#include "resource.h"

#include "MainDlgProc.h"

INT_PTR CALLBACK MainDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static MainDialogData* spData;
	static wstring sIni;
	switch(uMsg)
	{
		case WM_INITDIALOG:
		{

			spData = (MainDialogData*)lParam;
			
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

			wstring title = _T("FastGomibako");
			if (IsUserAnAdmin())
			{
				title += L" (";
				title += I18N("Admin");
				title += L")";
			}
			SetWindowText(hDlg, title.c_str());
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
					const int nDel = (int)SendDlgItemMessage(hDlg, IDC_COMBO_DELETEMETHOD, CB_GETCURSEL, 0, 0);
					const int nPri = (int)SendDlgItemMessage(hDlg, IDC_COMBO_PRIORITY, CB_GETCURSEL, 0, 0);
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
