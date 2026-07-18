#include "stdafx.h"
#include "tracewnd.h"


// #define WINLISP_TRACE_ENABLED TRUE
#define TRACE_WNDCLASS _T("TWinLISPTraceWnd")

#ifdef EXTRACE
	bool TraceEnable = true;  // master switch
#else
	bool TraceEnable = false;  // master switch
#endif

void SetTraceEnabled(bool bEnable)
{
	TraceEnable = bEnable;
}

bool GetTraceEnabled()
{
	return TraceEnable;
}

#define __countof(array) (sizeof(array)/sizeof(array[0]))

bool TraceActive()
{
	return IsWindow(::FindWindow(TRACE_WNDCLASS, NULL)) != FALSE;
}

/*
CString ExtractFilename(const char *s)
{
	char drive[MAX_PATH];
	char path[MAX_PATH];
	char name[MAX_PATH];
	char ext[MAX_PATH];
	_splitpath(s, drive, path, name, ext);
	CString res;
	res.Format("%s.%s", name, ext);
	return res;
}
*/

void ExTrace(const TCHAR* lpszFormat, ...)
{
	if(!TraceEnable)
		return;
	HWND target = ::FindWindow(TRACE_WNDCLASS, NULL);
	if(!IsWindow(target))
		return;
	va_list args;
	va_start(args, lpszFormat);
	TCHAR szBuffer[512];
	for(int i=0;i<512;i++)
		szBuffer[i] = 0;
	int nBuf = _vsnwprintf_s(szBuffer, _countof(szBuffer), lpszFormat, args);
	if (nBuf < 1)
		return;
	::OutputDebugStr( szBuffer );	
	char str[512];
	wcstombs(str, szBuffer, 512);
	COPYDATASTRUCT cds;
	cds.dwData = 1;
	cds.cbData = nBuf;
	cds.lpData = str;
	::SendMessage(target, WM_COPYDATA, 0, (LPARAM)&cds);
}


Acad::ErrorStatus ExTrace(Acad::ErrorStatus es)
{
	if( es != Acad::eOk )
	  ExTrace(_T("Acad::ErrorStatus: %s"), acadErrorStatusText(es));
	return es;
}

