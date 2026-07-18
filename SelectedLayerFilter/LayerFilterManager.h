#pragma once

#include "SingletonReactorTarget.h"
#include "LayerFilterDocData.h"
#include "AcString.h"

class LayerFilterManager :	
	public DocumentReactorTargetSingleton<LayerFilterManager, LayerFilterDocData>, 
	AcEditorReactor2
{
public:

	friend class DocumentReactorTargetSingleton<LayerFilterManager, LayerFilterDocData>;

	virtual void OnEndInitialize()
	{
		CDocumentReactorTarget<LayerFilterDocData>::OnEndInitialize();
		acedEditor->addReactor( this );
	}

	virtual ~LayerFilterManager()
	{
		try
		{
			acedEditor->removeReactor( this );
#ifdef EXTRACE
			ExTrace(_T("~LayerFilterManager(): filter count = %d"), filters.length());
#endif
			// CleanupDanglingFilters();
		}
		catch(...)
		{
		}
	}

	void CleanupDanglingFilters()
	{
		int cnt = filters.length();
		int deleted = 0;
		for( int i = 0; i < cnt; i++ )
		{
			AcLyLayerFilter* filter = filters[i];
			if( filter != NULL )
			{
				delete filter;
				++deleted;
			}
		}
#ifdef EXTRACE
		ExTrace(_T("CleanupDanglingFilters() deleted %d filters"), deleted);
#endif
		filters.setLogicalLength(0);
	}

	static bool FilterSelectionLayer( AcDbLayerTableRecord* layer )
	{
		LayerFilterManager* pManager = GetInstance();
		if( pManager != NULL )
		{
			AcApDocument* pDoc = acDocManager->document( layer->database() );
			if( pDoc != NULL )
			{
				return pManager->docData( pDoc ).filterSelectedLayer( layer );
			}
		}
		return false;
	}

	static bool FilterNestedSelectionLayer( AcDbLayerTableRecord* layer )
	{
		LayerFilterManager* pManager = GetInstance();
		if( pManager != NULL )
		{
			AcApDocument* pDoc = acDocManager->document( layer->database() );
			if( pDoc != NULL )
			{
				return pManager->docData( pDoc ).filterNestedLayer( layer );
			}
		}
		return false;
	}

	static bool FilterRecentLayer( AcDbLayerTableRecord* layer )
	{
		LayerFilterManager* pManager = GetInstance();
		if( pManager != NULL )
		{
			AcApDocument* pDoc = acDocManager->document( layer->database() );
			if( pDoc != NULL )
			{
				return pManager->docData( pDoc ).filterRecentLayer( layer );
			}
		}
		return false;
	}

	// Modeless only
	static bool IsLayerPropertiesManagerActive( bool checkIsStarting = true )
	{
		resbuf rb;
		if( acedGetVar(_T( "LAYERMANAGERSTATE" ), &rb ) == RTNORM )
		{
			if( rb.resval.rint != 0 )
				return true;

			if( checkIsStarting && IsCommandActive( _T("LAYER") ) )
			{
				if( acedGetVar( _T( "LAYERDLGMODE" ), &rb ) == RTNORM )
				{
					return rb.resval.rint != 0;
				}
			}
		}
		return false;
	}

	static bool IsLayerDialogActive( bool checkIsStarting = true )
	{
		return IsLayerPropertiesManagerActive( checkIsStarting ) || IsModalLayerCommand();
	}

	static bool IsModalLayerCommand()
	{
		AcApDocument* pDoc = acDocManager->mdiActiveDocument();
		if( pDoc != NULL )
		{
			TCHAR* name = NULL;
			if( acedGetCommandForDocument( pDoc, name ) == Acad::eOk && name != NULL )
			{
				bool result = IsModalLayerCommand( name );
				acutDelString( name );
				return result;
			}
		}
		return false;
	}

	static bool IsModalLayerCommand( const ACHAR* pGlobalCmdName )
	{
		if( _tcsicmp( _T("CLASSICLAYER"), pGlobalCmdName ) == 0 )
			return true;
		
		if( _tcsicmp( _T("QLAYER"), pGlobalCmdName ) == 0 )
			return true;

		if( _tcsicmp( _T("LAYER"), pGlobalCmdName ) == 0 )
		{
			resbuf rb;
			if( acedGetVar( _T( "LAYERDLGMODE" ), &rb ) == RTNORM )
			{
				return rb.resval.rint == 0;
			}
		}
		return false;
	}

	static bool IsCommandActive( const TCHAR* name, AcApDocument* pDoc = NULL )
	{
		bool result = false;
		pDoc = pDoc ? pDoc : acDocManager->mdiActiveDocument();
		if( pDoc != NULL )
		{
			TCHAR* cmd = NULL;
			acedGetCommandForDocument( pDoc, cmd );
			if( cmd != NULL )
			{
				result = acutWcMatchEx( cmd, name, true );
				acutDelString( cmd );
			}
		}
		return result;
	}

	// AcEditorReactor2 -------------------------------------------------------------------
	virtual void pickfirstModified()
	{
		AcApDocument* pDoc = acDocManager->mdiActiveDocument();
		if( pDoc != NULL )
		{
			docData( pDoc ).OnPickfirstModified();
		}
	}

   virtual void modelessOperationWillStart(const ACHAR* contextStr)
	{
		m_ModelessOperationContext = contextStr;
		m_bModelessOperationInProgress = true;
		AcApDocument* pDoc = acDocManager->mdiActiveDocument();
		if( pDoc != NULL )
		{
			docData( pDoc ).OnModelessOperationStarting( contextStr );
		}
	}

   virtual void modelessOperationEnded(const ACHAR* contextStr)
	{
		m_ModelessOperationContext.setEmpty();
		m_bModelessOperationInProgress = false;
		AcApDocument* pDoc = acDocManager->mdiActiveDocument();
		if( pDoc != NULL )
		{
			docData( pDoc ).OnModelessOperationEnded( contextStr );
		}
	}

	bool IsModeLessOperationInProgress() const
	{
		return m_bModelessOperationInProgress;
	}

	static AcString ActiveLayerFilter( AcApDocument* pDoc, bool rootDefault = true )
	{
		AcString result;
		result.setEmpty();
		pDoc = pDoc ? pDoc : acDocManager->mdiActiveDocument();
		if( pDoc != NULL )
		{
			AcLyLayerFilterManager* pMgr = aclyGetLayerFilterManager( pDoc->database() );
			if( pMgr != NULL )
			{
				AcLyLayerFilter* pRoot = NULL;
				AcLyLayerFilter* pCurrent = NULL;
				Acad::ErrorStatus es = pMgr->getFilters( pRoot, pCurrent );
				if( es == Acad::eOk )
				{
					if( pRoot != NULL )
					{
						if( pCurrent != NULL )
							result = pCurrent->name();
						else if( rootDefault )
							result = pRoot->name();
						delete pRoot;
					}
				}
			}
		}
		return result;
	}

	static bool SetActiveFilter( const TCHAR* name, AcApDocument* pDoc = NULL )
	{
		bool result = false;
		pDoc = pDoc ? pDoc : acDocManager->mdiActiveDocument();
		if( pDoc != NULL )
		{
			AcLyLayerFilterManager* pMgr = aclyGetLayerFilterManager( pDoc->database() );
			if( pMgr != NULL )
			{
				AcLyLayerFilter* pRoot = NULL;
				AcLyLayerFilter* pCurrent = NULL;
				Acad::ErrorStatus es = pMgr->getFilters( pRoot, pCurrent );
				if( es == Acad::eOk )
				{
					if( pRoot != NULL )
					{
						AcLyLayerFilter* pFilter = FindFilter( pRoot, name );
						if( pFilter != NULL && pFilter != pCurrent )
							result = pMgr->setFilters( pRoot, pFilter ) == Acad::eOk;
						delete pRoot;
					}
				}
			}
		}
		return result;
	}

	static AcLyLayerFilter* FindFilter( AcLyLayerFilter* root, const TCHAR* name, bool recursive = true )
	{
		AcArray<AcLyLayerFilter*> filters = root->getNestedFilters();
		int cnt = filters.length();
		for( int i = 0; i < cnt; i++ )
		{
			if( filters[i] != NULL && _tcsicmp( filters[i]->name(), name ) == 0 )
				return filters[i];
		}
		if( recursive )
		{
			for( int i = 0; i < cnt; i++ )
			{
				if( filters[i] != NULL )
				{
					AcLyLayerFilter* found = FindFilter( filters[i], name );
					if( found != NULL )
						return found;
				}
			}
		}
		return NULL;
	}

	// Testing only
	AcArray<AcLyLayerFilter*> filters;

protected:

	LayerFilterManager() : m_bUpdating( false ), 
		m_bModalLayerCommandActive( false ),
		m_bModelessOperationInProgress( false ),
		m_ModelessOperationContext( _T("") )
	{
	}
	
private:

	bool m_bUpdating;
	bool m_bModelessOperationInProgress;
	AcString m_ModelessOperationContext;
	bool m_bModalLayerCommandActive;
};

