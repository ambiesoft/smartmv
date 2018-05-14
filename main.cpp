#include "StdAfx.h"
#include "FastGomibako.h"

using namespace Ambiesoft;
using namespace stdwin32;
using namespace std;


Ambiesoft::CSessionGlobalMemory<int> gCount("FastGomibakoCounter");

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR     lpCmdLine,
	int       nCmdShow)
{
	// ResolveNtfsPath(__wargv[1]);
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
