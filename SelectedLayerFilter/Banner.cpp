#pragma once

#include "StdAfx.h"
#include "banner.h"
#include "dbdate.h"
#include "imagehlp.h"


//const TCHAR appname[] = _T("AcadX ActiveX Extension For AutoCAD");
// bool bFingerPrintQueried;
DWORD dwCrc32;
bool g_bBannerShown = false;
bool bFingerPrintQueried = false;
bool bHasExpired = false;

//size_t GetModulePath(AcString& str)
//{
//	TCHAR s[_MAX_PATH];
//	HMODULE hMod = NULL;
//	::GetModuleHandle(
//	::GetModuleFileName(NULL, s, _MAX_PATH);
//	str = s;
//	return str.length();
//}

//void MsgBox(const TCHAR * msg, UINT flags)
//{
//	CMDIFrameWnd* pAcadFrame = acedGetAcadFrame();
//	if(pAcadFrame)
//		pAcadFrame->MessageBox(msg, appname, flags);
//	else
//		::MessageBox(adsw_acadMainWnd(), msg, appname, flags);
//}


// #define TIME_LIMIT 1         // undefine to disable expiration

// #define RTC 1

#define EXPIRE_YEAR 2009
#define EXPIRE_MONTH 2
#define EXPIRED_MESSAGE _T("\n\n\n  ****  This preview version of QLayer.dll has expired\n  ****  Please contact tony.tanzillo@caddzone.com for assistance\n\n")

#ifdef TIME_LIMIT

	bool arx(const short y, const short m)
	{
		return (y * 12) + m > (EXPIRE_YEAR * 12) + EXPIRE_MONTH + 2;
	}

	bool expired()
	{
		if( bHasExpired )
		{
			ExTrace(_T("bHasExpired = true"));
			acutPrintf(EXPIRED_MESSAGE);
			return true;
		}
		AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
		if( pDb == NULL )
			return false;
		if( pDb->originalFileName() != NULL )
		{
			AcString filename( pDb->originalFileName() );
			if( filename.matchNoCase( _T("*.dwt") ) )
				return false;
			AcDbDate tdupdate = pDb->tdupdate();
			if( arx(tdupdate.year(), tdupdate.month()) )
			{
				ExTrace(_T("expired() == true"));
				acutPrintf( EXPIRED_MESSAGE );
				bHasExpired = true;
				return true;
			}
		}
      return false;
	}

#else

	bool GetArxExceptionHandler()
	{
		return false;
	}

#endif // TIME_LIMIT

void ShowExpiredMessage()
{
	acutPrintf( EXPIRED_MESSAGE );
}

bool ShowBanner()
{
	bFingerPrintQueried = false;
	dwCrc32 = 0;

	AcString BannerTemp( _T("\nAutoCAD PowerToys - QLayer (PREVIEW)   Copyright (c)2008  www.caddzone.com"));
//		"\nFor information on licensing AcadX, email tony.tanzillo@worldnet.att.net"
//		"\nThis copy of AcadX is for in-house use only, and may not be copied"
//		"\nThis copy of AcadX is licensed to Patella Woodworking."
		

	// This is a beta so we want to time-bomb it.

	if( Expired() ) return false;
/*
#ifdef LICENSE_CHECK
	if( ! bLicensed ) return false;
#endif
*/

	if( SHOWBANNER && !g_bBannerShown)
	{
		const TCHAR *s = BannerTemp;
		AcString banner(BannerTemp);
		acutPrintf(banner);
		AcString license;

#ifndef BANNER_CHECK
		g_bBannerShown = true;
		return true;
#endif

		g_bBannerShown = false;
		AcString ModuleName = acedGetAppName();
		bool fresult = false;
//#ifndef _DEBUG
		DWORD checksum;
		DWORD hdrsum;
		DWORD rslt;
		TCHAR* name = const_cast<TCHAR*>( (const TCHAR* ) ModuleName );
		rslt = ::MapFileAndCheckSum(name, &hdrsum, &checksum);
		// ExTrace("Checksum OK");
		// ExTrace("hdrsum = %u checksum = %u", hdrsum, checksum);
		if( rslt != CHECKSUM_SUCCESS )
		{
			ExTrace(_T("checksum failure"));
			acutPrintf(_T("Error: This copy of %s is corrupt\n\nPlease contact tony.tanzillo@caddzone.com for further assistence."), ModuleName );
			return false;
		}
		if( hdrsum == checksum )
		{
			dwCrc32 = hdrsum;
			fresult = true;
			g_bBannerShown = true;
			ExTrace(_T("checksum validation succeeded"));
			return fresult;
		}
//#else
//		fresult = true;
//		g_bBannerShown = true;
//		return fresult;
//#endif
		/*
		if ((sum + sum) == 22723 * 2)
		{
			g_bBannerShown = true;
			return true;
		}
		*/
		// Else somebody has altered the file to remove the copyright banner
		acutPrintf(_T("\nError loading %s\n"), ModuleName );
		acutPrintf(_T("\n*** Error: This copy of %s is corrupt\n\nPlease contact tony.tanzillo@caddzone.com for further assistence."), ModuleName	);
		return fresult;
	}
	return true;
}

