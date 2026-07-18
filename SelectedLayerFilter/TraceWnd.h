//#include <windows.h>

#pragma once

#include <stdio.h>
#include <io.h>
#include <errno.h>
#include <tchar.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "aced.h"
#include "acestext.h"

void ExTrace(const TCHAR* lpszFormat, ...);
Acad::ErrorStatus ExTrace(Acad::ErrorStatus es);
bool TraceActive();
void SetTraceEnabled(bool bEnable);
bool GetTraceEnabled();

