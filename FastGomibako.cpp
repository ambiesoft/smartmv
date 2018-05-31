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

// FastGomibako.cpp : Defines the entry point for the application.
//

// http://www.flickr.com/photos/bossco/2854452799/

#include "stdafx.h"
#include "../lsMisc/CommandLineString.h"
#include "../lsMisc/stlScopedClear.h"
#include "../lsMisc/CommandLineParser.h"

#include "resource.h"

#include "main.h"

#include "MainDlgProc.h"
#include "RetryDlgProc.h"







using namespace Ambiesoft;
using namespace stdwin32;
using namespace std;

//tstring GetUsageString()
//{
//	tstring ret;
//	ret += _T("\r\n\r\n");
//	ret += I18N(_T("Usage"));
//	ret += _T(":\r\n");
//	ret += _T("FastGomibako [File or Folder]");
//	return ret;
//}


bool tryAndArchive(LPCTSTR pFileOrig, LPCTSTR pRenameFull)
{
	bool movedone = false;
	while (!movedone)
	{
		if (!PathFileExists(pFileOrig))
		{
			tstring message = string_format(
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
				tstring message = string_format(
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

				RetryDialogData data;
				data.file = pFileOrig;
				data.message = message;
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
					wstring app = stdGetModuleFileName();
					int nArgc;
					LPCWSTR* ppArgv = (LPCWSTR*)CommandLineToArgvW(GetCommandLineW(), &nArgc);
					wstring arg = stdSplitCommandLine(nArgc, 1, ppArgv);
					LocalFree(ppArgv);

					OpenCommon(NULL,
						app.c_str(),
						arg.c_str(),
						NULL,
						NULL,
						L"runas");
					return false;
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

int doRemoveWork(const TCHAR root, LPCTSTR pFileOrig, MainDialogData& data)
{
	TCHAR szGomiDir[32];
	szGomiDir[0] = root;
	szGomiDir[1] = _T(':');
	szGomiDir[2] = _T('\\');
	lstrcpy(&szGomiDir[3], _T(".FastGomibako"));

	CreateDirectory(szGomiDir, NULL);
	DWORD dwAttr = GetFileAttributes(szGomiDir);
	if (dwAttr == 0xffffffff || (dwAttr & FILE_ATTRIBUTE_DIRECTORY) == 0)
	{
		throw I18N(L"Failed to create FastGomibako directory");
	}

	SetFileAttributes(szGomiDir, dwAttr | FILE_ATTRIBUTE_HIDDEN);

	LPCTSTR pFileName = _tcsrchr(pFileOrig, _T('\\'));
	++pFileName;

	TCHAR szGomiFile[MAX_PATH];
	lstrcpy(szGomiFile, szGomiDir);
	lstrcat(szGomiFile, _T("\\"));
	lstrcat(szGomiFile, pFileName);

	if (!tryAndArchive(pFileOrig, szGomiFile))
		return 0;

	if (!SetPriorityClass(GetCurrentProcess(), data.m_dwRetPri))
	{
		MessageBox(NULL, I18N(L"SetPriorityClass failed."), APPNAME, MB_ICONASTERISK);
	}

	FILEOP_FLAGS foFlags = FOF_NOCONFIRMATION;
	if (!data.IsComplete())
		foFlags |= FOF_ALLOWUNDO;
	if (0 != SHDeleteFile(szGomiFile, foFlags))
	{
		if (!data.IsComplete())
			throw I18N(L"Failed to trash file");
		else
			throw I18N(L"Failed to delete file");
	}

	if (!RemoveDirectory(szGomiDir))
	{
		if (gCount <= 1)
			throw I18N(L"Failed to remove FastGomibako directory");
	}
	return 0;
}

int doRename(MainDialogData& data)
{
	if (PathFileExists(data.renameefull().c_str()))
	{
		MessageBox(NULL, 
			string_format(I18N(L"\"%s\" already exists."), data.renameefull().c_str()).c_str(),
			APPNAME,
			MB_ICONEXCLAMATION);
		return 1;
	}
	if (!tryAndArchive(data.m_pTarget_, data.renameefull().c_str()))
		return 0;

	return 0;
}
int dowork()
{
	CCommandLineParser parser(CaseFlags_Default, I18N(L"Move and remove folder"));

	COption optionDefault(L"",
		ArgCount_Infinite,
		ArgEncodingFlags_Default,
		I18N(L"specify directory"));
	parser.AddOption(&optionDefault);

	bool bHelp = false;
	parser.AddOption( L"-h", L"/h" , 
		0,
		&bHelp,
		ArgEncodingFlags_Default,
		I18N(L"show help"));
	
	bool bVersion = false;
	parser.AddOption(L"-v", L"/v",
		0,
		&bVersion,
		ArgEncodingFlags_Default,
		I18N(L"show version"));

	parser.Parse(__argc, __targv);

	if (bHelp)
	{
		MessageBox(NULL, parser.getHelpMessage().c_str(), APPNAME, MB_ICONINFORMATION);
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

	if (optionDefault.getValueCount() > 1)
	{
		tstring exe = stdGetModuleFileName();
		for (size_t i = 0; i < optionDefault.getValueCount(); ++i)
		{
			tstring input = optionDefault.getValue(i);
			
			if (!OpenCommon(NULL,
				stdApplyDQ(exe).c_str(),
				stdApplyDQ(input).c_str()))
			{
				MessageBox(NULL, I18N(L"Failed to launch self"), APPNAME, MB_ICONERROR);
				return 1;
			}
		}
		return 0;
	}
	
	tstring inputfilename = optionDefault.getFirstValue();

	if(parser.hadUnknownOption())
	{
		tstring message = I18N(L"Unknown Option");
		message += L":\r\n";
		message += parser.getUnknowOptionStrings();
		
		MessageBox(NULL, message.c_str(), APPNAME, MB_OK|MB_ICONQUESTION);
		return 1;
	}
	
	inputfilename = stdGetFullPathName(inputfilename);

	LPCTSTR pFileOrig = _tcsdup(inputfilename.c_str());
	STLSOFT_SCODEDFREE_CRT(pFileOrig);

	LPTSTR pFile = _tcsdup(inputfilename.c_str());
	STLSOFT_SCODEDFREE_CRT(pFile);
	_tcslwr_s(pFile, _tcslen(pFile)+1);
	
	try
	{
		if( !(_T('A') <= pFile[0] || pFile[0] <= _T('Z')) )
		{
			throw I18N(L"Invalid Argument");
		}

		if(pFile[1] != _T(':') || pFile[2] != _T('\\'))
		{
			wstring full=stdGetFullPathName(pFile);
			free((void*)pFileOrig);
			pFileOrig = _tcsdup(full.c_str());

			free(pFile);
			pFile = _tcsdup(full.c_str());
			_tcslwr_s(pFile,_tcslen(pFile)+1);
			//_tcslwr(pFile);
		}

		if(pFile[3]==0)
		{
			throw I18N(L"Root Drive unacceptable");
		}

		DWORD dwAttr = GetFileAttributes(pFile);
		if(dwAttr == 0xffffffff)
		{
			throw string_format(I18N(L"\"%s\" is not found."), pFile);
		}

		const TCHAR root = pFile[0];

		MainDialogData data;
		data.m_pTarget_ = pFileOrig;
		if(IDOK != DialogBoxParam(GetModuleHandle(NULL),
			MAKEINTRESOURCE(IDD_DIALOG_ASK),
			NULL,
			MainDlgProc,
			(LPARAM)&data))
		{
			return 100;
		}


		if (data.IsRemove())
			return doRemoveWork(root, pFileOrig, data);
		else
		{
			doRename(data);
		}



	}
	catch(tstring& message)
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


