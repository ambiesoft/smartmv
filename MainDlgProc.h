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


struct MainDialogData {
	// LPCTSTR m_pTarget_;
	std::vector<std::wstring> targets_;
	std::wstring renamee_;
	enum Operation {
		Operation_Default = -1,
		Operation_Rename,
		Operation_MoveToTrashCan,
		Operation_Delete,
	} m_op;
	
	int m_priority = -1;


	MainDialogData() {
		// ZeroMemory(this, sizeof(*this));
		// m_pTarget_ = nullptr;
		m_op = Operation_Rename;
	}
	static std::wstring GetOperationCommandLineString(Operation operation) {
		switch (operation)
		{
		case Operation_Default:
			return L"";
		case Operation_Rename:
			return L"rename";
		case Operation_MoveToTrashCan:
			return L"trash";
		case Operation_Delete:
			return L"delete";
		default:
			assert(false);
		}
		return L"";
	}
	Operation operation() const {
		return m_op;
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
	std::wstring renamee() const {
		return renamee_;
	}
	void setRenamee(const std::wstring& s) {
		renamee_ = s;
	}
	bool IsRenameeExists() const;
	std::wstring renameefull() const;

	bool IsSingleFile() const{
		return targets_.size() == 1;
	}
	std::wstring ToTargetString() const;

	void setPriority(int nPri) {
		m_priority = nPri;
	}
	int priority() const {
		return m_priority;
	}
	DWORD getSystemPriorty() const {
		DWORD dwRetPri;
		switch (m_priority)
		{
		case 0:
			dwRetPri = HIGH_PRIORITY_CLASS;
			break;
		case 1:
			dwRetPri = NORMAL_PRIORITY_CLASS;
			break;
		case 2:
			dwRetPri = 0x00004000; // BELOW_NORMAL_PRIORITY_CLASS;
			break;
		case 3:
			if (IsWinVistaOrHigher())
				dwRetPri = 0x00100000; // PROCESS_MODE_BACKGROUND_BEGIN;
			else
				dwRetPri = IDLE_PRIORITY_CLASS;
			break;
		default:
			dwRetPri = GetPriorityClass(GetCurrentProcess());
		}
		return dwRetPri;
	}
};

INT_PTR CALLBACK MainDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
