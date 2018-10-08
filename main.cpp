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

#include "StdAfx.h"
#include "FastGomibako.h"

using namespace Ambiesoft;
using namespace Ambiesoft::stdosd;
using namespace stdwin32;
using namespace std;


Ambiesoft::CSessionGlobalMemory<int> gCount("FastGomibakoCounter");

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR     lpCmdLine,
	int       nCmdShow)
{
	// ResolveNtfsPath(__wargv[1]);
	// 64bit not work
	if (false && Is64BitWindows() && !Is64BitProcess())
	{
		wstring exe64 = stdGetParentDirectory(stdGetModuleFileName(), true);
		exe64 += L"FastGomibako";
#ifdef _DEBUG
		exe64 += L"D";
#endif
		exe64 += L"64.exe";
		if (!PathFileExists(exe64.c_str()))
		{
			MessageBox(NULL,
				I18N(L"could not find 64bit executable."),
				APPNAME,
				MB_ICONERROR);
			return 1;
		}


		OpenCommon(NULL,
			exe64.c_str(),
			lpCmdLine);

		return 0;
	}
	InitCommonControls();
#if _DEBUG
	Ambiesoft::i18nInitLangmap(hInstance, L"jpn", L"");
#else
	Ambiesoft::i18nInitLangmap(hInstance, L"", L"");
#endif
	int ret = 0;
	try
	{
		gCount = gCount + 1;
		ret = dowork();
	}
	catch (...)
	{
		ret = -1;
	}
	gCount = gCount - 1;
	return ret;
}
