
struct MainDialogData {
	LPCTSTR m_pTarget_;
	std::wstring renamee_;
	enum Operation {
		Operation_Rename,
		Operation_MoveToTrashCan,
		Operation_Delete,
	} m_op;
	// bool bRetComp;
	DWORD m_dwRetPri;

	MainDialogData() {
		// ZeroMemory(this, sizeof(*this));
		m_pTarget_ = nullptr;
		m_op = Operation_Rename;
		m_dwRetPri=GetPriorityClass(GetCurrentProcess());
	}
	bool IsRemove() const {
		return m_op == Operation_MoveToTrashCan || m_op == Operation_Delete;
	}
	bool IsComplete() const {
		return m_op == Operation_Delete;
	}
	bool IsRename() const {
		return m_op == Operation_Rename;
	}
	wstring renamee() const {
		return renamee_;
	}
	void setRenamee(LPCTSTR p) {
		renamee_ = p;
	}
	bool IsRenameeExists() const;
	std::wstring renameefull() const;
};

INT_PTR CALLBACK MainDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
