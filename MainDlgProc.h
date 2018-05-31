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
	std::wstring renamee() const {
		return renamee_;
	}
	void setRenamee(LPCTSTR p) {
		renamee_ = p;
	}
	bool IsRenameeExists() const;
	std::wstring renameefull() const;
};

INT_PTR CALLBACK MainDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
