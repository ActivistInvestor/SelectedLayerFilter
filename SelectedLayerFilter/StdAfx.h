// (C) Copyright 2002-2005 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//- StdAfx.h : include file for standard system include files,
//-      or project specific include files that are used frequently,
//-      but are changed infrequently
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
//- 'DEBUG workaround' below prevents the MFC or ATL #include-s from pulling 
//- in "Afx.h" that would force the debug CRT through #pragma-s.
#define USE_DEBUG_WORKAROUND

#ifdef USE_DEBUG_WORKAROUND
	#if defined(_DEBUG) && !defined(AC_FULL_DEBUG)
	#define _DEBUG_WAS_DEFINED
	#undef _DEBUG
	#pragma message ("     Compiling MFC / STL / ATL header files in release mode.")
	#endif
#endif

#pragma pack (push, 8)
#pragma warning(disable: 4786 4996)
//#pragma warning(disable: 4098)

//-----------------------------------------------------------------------------
#include <windows.h>

//- ObjectARX and OMF headers needs this
#include <map>
#include <hash_set>

//-----------------------------------------------------------------------------
#using <mscorlib.dll>
#using <System.dll>

#using <acdbmgd.dll>
#using <acmgd.dll>

#include <vcclr.h>

//-----------------------------------------------------------------------------
//- Include ObjectDBX/ObjectARX headers
//- Uncomment one of the following lines to bring a given library in your project.
//#define _BREP_SUPPORT_			//- Support for the BRep API
//#define _HLR_SUPPORT_				//- Support for the Hidden Line Removal API
//#define _AMODELER_SUPPORT_		//- Support for the AModeler API
//#define _ASE_SUPPORT_				//- Support for the ASI/ASE API
//#define _RENDER_SUPPORT_			//- Support for the AutoCAD Render API
//#define _ARX_CUSTOM_DRAG_N_DROP_	//- Support for the ObjectARX Drag'n Drop API
//#define _INC_LEAGACY_HEADERS_		//- Include legacy headers in this project
#include "arxHeaders.h"

// #include "banner.h"

//#include "LayerFilterManager.h"
//#include "LayerFilterDocData.h"

extern bool AcadIsQuitting();

#define SELECTED_LAYER_FILTER_NAME _T(" Current Selection Layers")
#define NESTED_SELECTED_LAYER_FILTER_NAME _T("Nested Object Layers")
#define RECENT_LAYER_FILTER_NAME _T(" Recently Used Layers")

// #define DEBUG_FILTER_METHOD
// #define DEBUG_LIFETIME
// #define DEBUG_LMANSTATE
#define DEBUG_REFCOUNT
#define CMDECHO 0

#define expired() false

#define BANNER_CHECK       // undefine to disable banner tampering check
#define TIME_LIMIT


//#ifdef _DEBUG_WAS_DEFINED
	#define EXTRACE
//#endif

#ifdef DEBUG_LIFETIME

	#define TRACE_CTOR(X)\
		const ACHAR* s = X->desc()->name();\
		ExTrace( _T("%s::%s()"), s, s );

	#define TRACE_DTOR(X)\
		const ACHAR* s = X->desc()->name();\
		ExTrace( _T("%s::~%s()"), s, s );

#else
	#define TRACE_CTOR(X)
	#define TRACE_DTOR(X)
#endif

#pragma pack (pop)

//-----------------------------------------------------------------------------
#ifdef USE_DEBUG_WORKAROUND
	#ifdef _DEBUG_WAS_DEFINED
	#define _DEBUG
	#undef _DEBUG_WAS_DEFINED
	#endif
#endif

#include <tchar.h>
#include "TraceWnd.h"

#pragma comment(lib, "Imagehlp.lib")
