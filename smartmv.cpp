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

// smartmv.cpp : Defines the entry point for the application.
//

// http://www.flickr.com/photos/bossco/2854452799/

#include "stdafx.h"
#include "../lsMisc/CommandLineString.h"
#include "../lsMisc/CommandLineParser.h"

#include "../lsMisc/stdosd/stdosd.h"
#include "../lsMisc/RunAsAdmin.h"

#include "resource.h"

#include "main.h"
#include "version.h"
#include "MainDlgProc.h"
#include "RetryDlgProc.h"







using namespace Ambiesoft;
using namespace Ambiesoft::stdosd;
using namespace stdwin32;
using namespace std;

//tstring GetUsageString()
//{
//	tstring ret;
//	ret += _T("\r\n\r\n");
//	ret += I18N(_T("Usage"));
//	ret += _T(":\r\n");
//	ret += _T("smartmv [File or Folder]");
//	return ret;
//}

void ShowVersionMessageBox(HWND hParent)
{
	wstring message = APPNAME L" ver " VERSION;
	message += L"\r\n\r\n";
	message += L"Copyright 2019 Ambiesoft";
	message += L"\r\n";
	message += L"http://ambiesoft.mooo.com/";
	MessageBox(hParent, message.c_str(), APPNAME, MB_ICONINFORMATION);
}
static bool myPathFileExists(const wchar_t* pFile)
{
	if (GetFileAttributes(pFile) == 0xFFFFFFFF)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
			return false;
	}
	return true;
}
MainDialogData::Operation StringToOperation(const wstring& operation)
{
	MainDialogData::Operation op = MainDialogData::Operation_Default;
	if (operation == L"rename")
		op = MainDialogData::Operation_Rename;
	else if (operation == L"trash")
		op = MainDialogData::Operation_MoveToTrashCan;
	else if (operation == L"delete")
		op = MainDialogData::Operation_Delete;
	return op;
}
static bool IsValidOpValue(const wstring& operation)
{
	return StringToOperation(operation) != MainDialogData::Operation_Default;
}
static wstring getCommandLineArgs(const MainDialogData::Operation operation, const wstring& renamee, int priority)
{
	wstring cmdLine = GetCommandLineW();
	wstring arg;
	try
	{
		CCommandLineString cls(cmdLine.c_str());
		cls.remove(0); // remove exe
		int opIndex = cls.getIndex(L"-op");
		if (opIndex >= 0)
		{
			if (!IsValidOpValue(cls.getArg(opIndex + 1)))
			{
				MessageBox(nullptr, L"Illeval command line", APPNAME, MB_ICONERROR);
				ExitProcess(1);
			}
			cls.remove(opIndex, 2);
		}
		
		int toIndex = cls.getIndex(L"-to");
		if (toIndex >= 0)
		{
			// remove "-to arg"
			cls.remove(toIndex, 2);
		}

		int priIndex = cls.getIndex(L"-pri");
		if (priIndex >= 0)
		{
			cls.remove(priIndex, 2);
		}
		arg = cls.toString();
		arg += L" ";
		
		arg += L"-op ";
		arg += MainDialogData::GetOperationCommandLineString(operation);
		arg += L" ";
		
		arg += L"-to ";
		arg += stdAddDQIfNecessary(renamee);
		arg += L" ";

		arg += L"-pri ";
		arg += to_wstring(priority);
	}
	catch (...)
	{
		CCommandLineString cls(cmdLine);
		arg = cls.subString(1);
	}
	return arg;
}
bool tryMove(const MainDialogData::Operation operation, LPCTSTR pFileOrig, LPCTSTR pRenameFull, LPCTSTR pRenamee, int priority)
{
	bool movedone = false;
	while (!movedone)
	{
		if (!myPathFileExists(pFileOrig))
		{
			tstring message = stdFormat(
				I18N(L"\"%s\" does not exit anymore."),
				pFileOrig);
			throw message;
		}
		if (MoveFile(pFileOrig, pRenameFull))
		{
			break;
		}
		else
		{
			DWORD dwLastError = GetLastError();
			if (ERROR_ALREADY_EXISTS == dwLastError)
			{
				tstring message = stdFormat(
					I18N(L"\"%s\" already exists. You need remove it manually first."),
					pRenameFull);
				throw message;
			}
			else
			{
				tstring t = GetLastErrorString(dwLastError);
				tstring message = I18N(L"Failed to move file: ");
				message += pFileOrig;
				message += L"\r\n";
				message += t;

				assert(pFileOrig);
				RetryDialogData data(pFileOrig, message);

				INT_PTR nDR = DialogBoxParam(GetModuleHandle(NULL),
					MAKEINTRESOURCE(IDD_DIALOG_RETRY),
					NULL,
					RetryDlgProc,
					(LPARAM)&data);

				if (nDR == IDOK)
				{
					// retry
					continue;
				}
				else if (nDR == IDCANCEL)
				{
					return false;
				}
				else if (nDR == IDC_BUTTON_ELEVATE)
				{
					wstring arg = getCommandLineArgs(operation, pRenamee,priority);
					RunThisAsAdmin(nullptr, arg.c_str());
					return false;
				}
				else if (nDR == IDC_BUTTON_RUNAS_DIFFERENTCPU)
				{
					if (Is64BitWindows())
					{
						bool isNow64 = Is64BitProcess();
						wstring exe = stdGetParentDirectory(stdGetModuleFileName<wchar_t>(), true);
						exe += L"smartmv";
#ifdef _DEBUG
						exe += L"D";
#endif
						exe += isNow64 ? L".exe" : L"64.exe";

						wstring arg = getCommandLineArgs(operation, pRenamee,priority);
						OpenCommon(NULL,
							exe.c_str(),
							arg.c_str(),
							NULL,
							NULL);

						return false;
					}
					else
					{
						assert(FALSE);
					}
				}
				else
				{
					assert(FALSE);
				}
			}
		}
	}
	return true;
}

int doRemoveWork(MainDialogData& data)
{
	set<wstring> gomiDirs;
	vector<wstring> gomiFiles;
	
	for (auto&& target : data.targets_)
	{
		TCHAR root = target[0];
		TCHAR szGomiDir[32];
		szGomiDir[0] = root;
		szGomiDir[1] = _T(':');
		szGomiDir[2] = _T('\\');
		lstrcpy(&szGomiDir[3], _T(".smartmv"));

		CreateDirectory(szGomiDir, NULL);
		DWORD dwAttr = GetFileAttributes(szGomiDir);
		if (dwAttr == 0xffffffff || (dwAttr & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			throw I18N(L"Failed to create smartmv directory");
		}

		// SetFileAttributes(szGomiDir, dwAttr | FILE_ATTRIBUTE_HIDDEN);

		LPCTSTR pFileName = _tcsrchr(target.c_str(), _T('\\'));
		++pFileName;

		TCHAR szGomiFile[MAX_PATH];
		lstrcpy(szGomiFile, szGomiDir);
		lstrcat(szGomiFile, _T("\\"));
		lstrcat(szGomiFile, pFileName);

		if (!tryMove(data.operation(), target.c_str(), szGomiFile, data.renamee().c_str(), data.priority()))
			return 0;

		gomiDirs.insert(szGomiDir);
		gomiFiles.push_back(szGomiFile);
	}
	
	if (!SetPriorityClass(GetCurrentProcess(), data.getSystemPriorty()))
	{
		MessageBox(NULL, I18N(L"SetPriorityClass failed."), APPNAME, MB_ICONASTERISK);
	}

	FILEOP_FLAGS foFlags = FOF_NOCONFIRMATION;
	if (!data.IsComplete())
		foFlags |= FOF_ALLOWUNDO;
	if (0 != SHDeleteFileEx(gomiFiles, foFlags))
	{
		if (!data.IsComplete())
			throw I18N(L"Failed to trash file");
		else
			throw I18N(L"Failed to delete file");
	}

	bool failed = false;
	for (auto&& gomiDir : gomiDirs)
	{
		failed |= !RemoveDirectory(gomiDir.c_str());
	}
	if(failed)
	{
		if (gCount <= 1)
			throw I18N(L"Failed to remove smartmv directory");
	}

	return 0;
}


int doRename(MainDialogData& data)
{
	if (PathFileExists(data.renameefull().c_str()))
	{
		MessageBox(NULL, 
			stdFormat(I18N(L"\"%s\" already exists."), data.renameefull().c_str()).c_str(),
			APPNAME,
			MB_ICONEXCLAMATION);
		return 1;
	}
	assert(data.IsSingleFile());
	if (!tryMove(data.operation(), data.targets_[0].c_str(), data.renameefull().c_str(), data.renamee().c_str(), data.priority()))
		return 0;

	return 0;
}
int dowork()
{
	CCommandLineParser parser(CaseFlags_Default, I18N(L"Rename or remove a folder"), APPNAME);

	COption optionDefault(L"",
		ArgCount::ArgCount_ZeroToInfinite,
		ArgEncodingFlags_Default,
		I18N(L"specify directory"));
	parser.AddOption(&optionDefault);

	bool bHelp = false;
	parser.AddOptionRange({ L"-h", L"/h"},
		0,
		&bHelp,
		ArgEncodingFlags_Default,
		I18N(L"show help"));
	
	bool bVersion = false;
	parser.AddOptionRange({ L"-v", L"/v" },
		0,
		&bVersion,
		ArgEncodingFlags_Default,
		I18N(L"show version"));

	wstring operation;
	parser.AddOption(L"-op",
		1,
		&operation,
		ArgEncodingFlags_Default,
		I18N(L"Operation: One of 'rename', 'trash', 'delete'"));

	wstring renameto;
	parser.AddOption(L"-to",
		1,
		&renameto,
		ArgEncodingFlags_Default,
		I18N(L"Specify new name"));

	int priority = -1;
	parser.AddOption(L"-pri",
		1,
		&priority,
		ArgEncodingFlags_Default,
		I18N(L"Specify priority, 0=High 1=Normal 2=Low 3=Idle"));

	parser.Parse(__argc, __targv);

	if (bHelp)
	{
		MessageBox(NULL, parser.getHelpMessage().c_str(), APPNAME, MB_ICONINFORMATION);
		return 0;
	}

	if (bVersion)
	{
		ShowVersionMessageBox(nullptr);
		return 0;
	}
	if(parser.isEmpty())
	{
		tstring message = I18N(_T("No Arguments"));
		message += L"\r\n";
		message += parser.getHelpMessage();
		MessageBox(NULL, message.c_str(), APPNAME, MB_OK|MB_ICONQUESTION);
		return 0;
	}

	if(!optionDefault.hadValue())
	{
		tstring message = I18N(L"No input");
		message += L":\r\n";
		message += parser.getUnknowOptionStrings();
		MessageBox(NULL, message.c_str(), APPNAME, MB_OK|MB_ICONQUESTION);
		return 1;
	}

	MainDialogData::Operation op = MainDialogData::Operation_Default;
	if (!operation.empty())
	{
		op = StringToOperation(operation);
		if (op == MainDialogData::Operation_Default)
		{
			wstring message = stdFormat(I18N(L"Unknown operation: %s"), operation.c_str());
			MessageBox(NULL, message.c_str(), APPNAME, MB_ICONERROR);
			return 1;
		}
	}
	
	if(parser.hadUnknownOption())
	{
		tstring message = I18N(L"Unknown Option");
		message += L":\r\n";
		message += parser.getUnknowOptionStrings();
		
		MessageBox(NULL, message.c_str(), APPNAME, MB_OK|MB_ICONQUESTION);
		return 1;
	}
		
	if (optionDefault.getValueCount() > 1 && op == MainDialogData::Operation_Rename)
	{
		// launch each of them
		CCommandLineString cls;
		wstring app = cls[0];
		wstring arg;
		wstring option;

		// first take options
		for (size_t i = 1; i < cls.getCount(); ++i)
		{
			if (cls[i] == L"-h")
			{
				option += L"-h ";
				continue;
			}
			if (cls[i] == L"-op")
			{
				option += cls[i];
				option += L" ";
				++i;
				option += cls[i];
				option += L" ";
				continue;
			}
		}

		// second, get arg and launch
		for (size_t i = 1; i < cls.getCount(); ++i)
		{
			if (cls[i] == L"-h")
			{
				continue;
			}
			if (cls[i] == L"-op")
			{
				++i;
				continue;
			}

			wstring command;
			command += option;
			command += stdAddDQIfNecessary(cls[i]);
			if (!OpenCommon(nullptr, app.c_str(), command.c_str()))
			{
				throw stdFormat(L"Failed to launch %s", (app + L" " + command).c_str());
			}
		}
		return 0;
	}
	try
	{
		vector<wstring> targetPathes;
		for (size_t i = 0; i < optionDefault.getValueCount(); ++i)
		{
			wstring inputfilename = optionDefault.getValue(i);

			// TODO: extract canonical path
			// inputfilename = stdGetFullPathName(inputfilename);
			inputfilename = resolveLink(inputfilename);

			wstring fileOrig = inputfilename;
			wstring file = stdStringLower(inputfilename);
	
			// if (!(_T('A') <= file[0] || file[0] <= _T('Z')))

			if ((file.length() >= 2 && file[0] == L'\\' && file[1] == '\\') ||
				PathIsNetworkPath(file.c_str()))
			{
				throw I18N(L"Could not process for path of network drive.");
			}
			if (!stdIsAsciiAlpha(file[0]))
			{
				throw I18N(L"Could not obtain fullpath.");
			}

			if (file[1] != _T(':') || file[2] != _T('\\'))
			{
				throw I18N(L"Could not obtain fullpath.");
			}

			if (file[3] == 0)
			{
				throw I18N(L"Root Drive unacceptable");
			}

			DWORD dwAttr = GetFileAttributes(file.c_str());
			if (dwAttr == 0xffffffff)
			{
				DWORD dwLE = GetLastError();
				if (dwLE == ERROR_FILE_NOT_FOUND)  // 'access is denied' means delete pending
					throw stdFormat(wstring(I18N(L"\"%s\" is not found.")) + L"\r\n" + GetLastErrorString(dwLE), fileOrig.c_str());
			}

			targetPathes.push_back(fileOrig);
			// const TCHAR root = pFile[0];
		}
		MainDialogData data;
		data.m_op = op;
		data.targets_ = targetPathes;
		data.renamee_ = renameto;
		data.setPriority(priority);
		if(IDOK != DialogBoxParam(GetModuleHandle(NULL),
			MAKEINTRESOURCE(IDD_DIALOG_ASK),
			NULL,
			MainDlgProc,
			(LPARAM)&data))
		{
			return 100;
		}


		if (data.IsRemove())
		{
			return doRemoveWork(data);
		}
		else
		{
			doRename(data);
		}
	}
	catch(wstring& message)
	{
		MessageBox(NULL, message.c_str(), APPNAME, MB_OK|MB_ICONERROR);
		return 2;
	}
	catch(LPCTSTR pMessage)
	{
		MessageBox(NULL, pMessage, APPNAME, MB_OK|MB_ICONERROR);
		return 2;
	}
	catch(...)
	{
		MessageBox(NULL, I18N(L"Unexpected error"), APPNAME, MB_OK|MB_ICONQUESTION);
		return 3;
	}
	return 0;
}


