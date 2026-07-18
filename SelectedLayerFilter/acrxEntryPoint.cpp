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
//----- acrxEntryPoint.h
//-----------------------------------------------------------------------------
#pragma once

#include "StdAfx.h"
#include "resource.h"
#include "LayerFilterManager.h"
#include "SelectionLayerFilter.h"
#include "NestedSelectionLayerFilter.h"
#include "RecentLayerFilter.h"
#include "CustomLayerFilter.h"
// #include "BlockLayerFilter.h"
// #include "Banner.h"
// #include "DynamicLayerFilterObjectFactory.h"

//-----------------------------------------------------------------------------
#define szRDS _RXST("")

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CQLayerApp : public AcRxArxApp {

public:

	CQLayerApp () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) 
	{
		// TODO: Load dependencies here
		acrxDynamicLinker->lockApplication( pkt );

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;
		
		// TODO: Add your initialization code here

#ifdef EXTRACE
		ExTrace(_T("\n\n\n\n\n\n\n\nQLayer Loaded.....\n"));
#endif

		if( true ) //  ShowBanner() )
		{
			DynamicLayerFilter::rxInit();
			SelectionLayerFilter::rxInit();
			NestedSelectionLayerFilter::rxInit();
			RecentLayerFilter::rxInit();

			CustomLayerFilter::rxInit();
			ChildFilter::rxInit();
			// BlockLayerFilter::rxInit();
			acrxBuildClassHierarchy();

			//DynamicLayerFilterObjectFactory* pFactory = NULL;

			//factories.append( new DynamicLayerFilterObjectFactory( SelectionLayerFilter::desc() ) );
			//factories.append( new DynamicLayerFilterObjectFactory( NestedSelectionLayerFilter::desc() ) );
			//factories.append( new DynamicLayerFilterObjectFactory( RecentLayerFilter::desc() ) );


			LayerFilterManager::InitializeInstance();

			RegisterArxApp();
		}
		else
		{
			UnregisterArxApp();
			acutPrintf(_T("\n ***  QLayer.dll has been unregistered.  *** "));
		}

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) 
	{
		// TODO: Add your code here

		if( LayerFilterManager::InstanceInitialized() )
		{

			LayerFilterManager::Terminate();
			deleteAcRxClass( SelectionLayerFilter::desc() );
			deleteAcRxClass( NestedSelectionLayerFilter::desc() );
			deleteAcRxClass( RecentLayerFilter::desc() );
			// deleteAcRxClass( BlockLayerFilter::desc() );
			deleteAcRxClass( DynamicLayerFilter::desc() );

			deleteAcRxClass( ChildFilter::desc() );
			deleteAcRxClass( CustomLayerFilter::desc() );
		}

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;

		// TODO: Unload dependencies here

		return retCode;
	}

	// AcArray<DynamicLayerFilterObjectFactory*> factories;

	void RegisterArxApp()
	{
		AcadAppInfo appInfo;
		appInfo.setAppName(_T("QLayer")); // Application name
		appInfo.setModuleName(acedGetAppName());// Module path
		appInfo.setAppDesc(_T("QLayer ARX Application")); // Description
		appInfo.setLoadReason(// Specify when we want these to load
			AcadApp::LoadReasons(
			AcadApp::kOnAutoCADStartup |
			AcadApp::kOnLoadRequest));
		appInfo.setManaged(true);
		appInfo.writeToRegistry(false,true);// Write the appInfo to the registry.
	}

	void UnregisterArxApp()
	{
		AcadAppInfo appInfo;
		appInfo.setAppName(_T("QLayer")); // Application name
		appInfo.setModuleName(acedGetAppName());// Module path
		appInfo.setAppDesc(_T("QLayer ARX Application")); // Description
		appInfo.setLoadReason(// Specify when we want these to load
			AcadApp::LoadReasons(
			AcadApp::kOnAutoCADStartup |
			AcadApp::kOnLoadRequest));
		appInfo.setManaged(true);
		appInfo.delFromRegistry(false);
	}

	virtual void RegisterServerComponents() 
	{
		// RegisterArxApp();
	}


	// - SelectedLayerFilter.QLAYER command (do not rename)

	// Need to set current filter to selectted layer filter
	// Not filtering because layer palette is hidden

	static bool GetSelection( ads_name& ss, long& len, bool pickfirstOnly = false )
	{
		bool result = false;
		ss[0] = 0;
		ss[1] = 0;
		resbuf* unused = NULL;
		resbuf* rb = NULL;
		if( acedSSGetFirst( &unused, &rb ) == RTNORM && rb != NULL )
		{
			ss[0] = rb->resval.rlname[0];
			ss[1] = rb->resval.rlname[1];
			result = acedSSLength( ss, &len ) == RTNORM && len > 0;
			acutRelRb( rb );
			if( len > 0 || pickfirstOnly )
				return result;
		}
		ads_name temp;
		temp[0] = 0;
		temp[1] = 0;
		if( acedSSGet( NULL, NULL, NULL, NULL, temp ) == RTNORM )
		{
			if( acedSSLength( temp, &len ) == RTNORM && len > 0 )
			{
				ss[0] = temp[0];
				ss[1] = temp[1];
			}
		}
		return false;
	}

	static void QLayerCommandsPQLAYER(void)
	{
		//if( expired() )
		//{
		//	ShowExpiredMessage();
		//	return;
		//}
		if( LayerFilterManager::IsLayerPropertiesManagerActive( false ) )
		{
			acutPrintf(_T("\nLayer Properties Manager already active."));
			return;
		}
		LayerFilterManager* pMgr = LayerFilterManager::GetInstance();
		if( pMgr != NULL )
		{
			LayerFilterDocData& data = pMgr->docData();
			ads_name ss;
			ss[0] = 0;
			ss[1] = 0;
			long count = 0L;
			resbuf rb;
			bool pickFirst = GetSelection( ss, count );
			bool selected = ss[0] != 0 && ss[1] != 0;

			resbuf sysVar;
			acedGetVar(_T("CMDECHO"), &sysVar );
			sysVar.resval.rint = CMDECHO;
			acedSetVar(_T("CMDECHO"), &sysVar );
			AcString curFilter;
			curFilter.setEmpty();

			rb.restype = RTNONE;
			if( count > 0 )
			{
				curFilter = data.ActiveLayerFilter();
				rb.restype = RTPICKS;
				rb.resval.rlname[0] = ss[0];
				rb.resval.rlname[1] = ss[1];
			}

			bool filterChanged = count > 0 && curFilter != SELECTED_LAYER_FILTER_NAME;

			if( filterChanged )
			{
				acedCommand(RTSTR, _T("._UNDO"), RTSTR, _T("_Begin"), RTNONE );
				if( filterChanged )
					SelectionLayerFilter::SetActive();
				data.BeginModal( &rb );
			}

			acedCommand( RTSTR, _T("._CLASSICLAYER"), RTNONE );

			if( filterChanged )
			{
				data.EndModal();
				if( curFilter.isEmpty() )
					LayerFilterManager::SetActiveFilter( (const TCHAR*) curFilter );
				acedCommand( RTSTR, _T("._UNDO"), RTSTR, _T("_End"), RTNONE );
			}

			acedSetVar( _T("CMDECHO"), &sysVar );

			if( count > 0 )
			{
				if( pickFirst )
					acedSSSetFirst( ss, ss );
			}
			if( selected )
				acedSSFree( ss );
		}
	}



	// - SelectedLayerFilter.QLTEST command (do not rename)
	static void SelectedLayerFilterQLTEST(void)
	{
		AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
		AcLyLayerFilterManager *pMgr = aclyGetLayerFilterManager( pDb );
		AcLyLayerFilter* pRoot = NULL;
		AcLyLayerFilter* pCur = NULL;
		if( pMgr->getFilters( pRoot, pCur ) == Acad::eOk && pRoot != NULL )
		{
			SelectionLayerFilter* pSlf = SelectionLayerFilter::Find( pRoot );
			if( pSlf != NULL )
			{
				AcDbLayerTablePointer pLt(pDb, AcDb::kForRead);
				if( pLt.openStatus() == Acad::eOk )
				{
					AcDbLayerTableIterator* pIter = NULL;
					Acad::ErrorStatus es = pLt->newIterator( pIter );
					if( es == Acad::eOk && pIter != NULL )
					{
						for(; ! pIter->done(); pIter->step() )
						{
							AcDbObjectId id;
							if( pIter->getRecordId( id ) == Acad::eOk )
							{
								AcDbLayerTableRecordPointer pLayer( id, AcDb::kForRead );
								if( pLayer.openStatus() == Acad::eOk )
								{
									const TCHAR* name;
									pLayer->getName( name );
									bool result = pSlf->filter( pLayer.object() );
#ifdef EXTRACE
									ExTrace(_T("filter(%s) = %s"), name, result ? _T("true"):_T("false"));
#endif
								}
							}
						}
						if( pIter != NULL )
							delete pIter;
					}
				}
			}
			delete pRoot;
		}
	}



	// ----- ads_qlayer symbol (do not rename)
	static int ads_qlayer(void)
	{
		QLayerCommandsPQLAYER();
		acedRetVoid();
		return (RSRSLT);
	}
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CQLayerApp)

// ACED_ARXCOMMAND_ENTRY_AUTO(CQLayerApp, QLayerCommands, PQLAYER, PQLAYER, ACRX_CMD_MODAL | ACRX_CMD_NOMULTIPLE | ACRX_CMD_USEPICKSET | ACRX_CMD_REDRAW, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CQLayerApp, SelectedLayerFilter, QLTEST, QLTEST, ACRX_CMD_MODAL, NULL)
ACED_ADSCOMMAND_ENTRY_AUTO(CQLayerApp, qlayer, true)
