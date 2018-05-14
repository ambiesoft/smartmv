
struct RetryDialogData {
	std::wstring file;
	std::wstring message;
};

INT_PTR CALLBACK RetryDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);