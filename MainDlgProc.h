
struct MainDialogData {
	LPCTSTR pTarget_;

	bool bRetComp;
	DWORD dwRetPri;

	MainDialogData() {
		ZeroMemory(this, sizeof(*this));
		dwRetPri=GetPriorityClass(GetCurrentProcess());
	}
};

BOOL CALLBACK MainDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
