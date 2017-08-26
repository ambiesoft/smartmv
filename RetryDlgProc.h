
struct RetryDialogData {
	wstring file;
	wstring message;
};

INT_PTR CALLBACK RetryDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);