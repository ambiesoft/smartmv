
struct RetryDialogData {
	wstring file;
	wstring message;
};

BOOL CALLBACK RetryDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);