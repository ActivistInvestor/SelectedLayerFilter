#pragma once

#include "acly.h"
#include "LayerFilterManager.h"
#include "LayerFilterDocData.h"
#include "MgdStackWalker.h"

#define DEBUG_REFCOUNT

class DynamicLayerFilter :	public AcLyLayerFilter
{
public:

	ACRX_DECLARE_MEMBERS( DynamicLayerFilter );

	inline DynamicLayerFilter() 
		: m_bClientIsLayerDropDownControl( false ), m_bFilterCalled(false), m_bAllowNested(false), m_pParent(NULL), m_nestedFiltersLoaded(false)
	{

#ifdef DEBUG_REFCOUNT

		LayerFilterManager* pMgr = LayerFilterManager::GetInstance( false );
		if( pMgr != NULL )
		{
			pMgr->filters.append( this );
		}
#ifdef EXTRACE
		else
			ExTrace(_T("DynamicLayerFilter::DynamicLayerFilter(): Failed to get LayerFilterManager"));
#endif

#endif

	}

	inline virtual ~DynamicLayerFilter()
	{
		deleteNested();

#ifdef DEBUG_REFCOUNT

		LayerFilterManager* pMgr = LayerFilterManager::GetInstance( false );
		if( pMgr != NULL )
		{
			pMgr->filters.remove( this );
		}
#ifdef EXTRACE
		else
			ExTrace(_T("DynamicLayerFilter::~DynamicLayerFilter(): Failed to get LayerFilterManager"));
#endif

#endif

	}

	inline void deleteNested()
	{
		AcArray<AcLyLayerFilter*> nested;
		nested.append( getNestedFilters() );
		int len = nested.length();
		for( int i = 0; i < len; i++ )
		{
			AcLyLayerFilter* filter = nested[i];
			if( filter != NULL )
				delete filter;
		}
	}

	inline virtual bool isProxy() const
	{
		return false;
	}

	inline virtual bool isDynamicallyGenerated()
	{
		return true;
	}

	inline virtual bool allowRename() const
	{
		return false;
	}
	
	inline virtual bool allowNested() const
	{
		return m_bAllowNested;
	}

	inline virtual bool allowDelete() const
	{
		return false;
	}

	//virtual bool isDynamicallyGenerated() const
	//{
	//	return true;   // testing
	//}

	inline virtual DialogResult showEditor()
	{
		return kCancel;
	}

	// returns true if the floating layer palette was not active
	// when this instance was created and the modal layer dialog
	// is not active, to prevent filter from being applied to the 
	// layer dropdown control

	// derived types should return 'DynamicLayerFilter::filter( layer ) || <derived_type>::filter( layer )'

	inline virtual bool filter( AcDbLayerTableRecord* layer ) const
	{
		if( ! m_bFilterCalled )
		{
			gcroot<MgdStackWalker^> walker = gcnew MgdStackWalker();
			DynamicLayerFilter* pThis = const_cast<DynamicLayerFilter*>(this);
			pThis->m_bClientIsLayerDropDownControl = walker->FrameCount < 4;
			pThis->m_bFilterCalled = true;
		}

		//if( ! m_bFilterCalled )
		//{
		//	const_cast<DynamicLayerFilter*>(this)->m_bFilterCalled = true;
		//	ExTrace( _T("%s::filter()"), this->isA()->name() );
		//}
		// derived types should return this || their result

		return this->m_bClientIsLayerDropDownControl;
	}

	//virtual Acad::ErrorStatus getImages(HIMAGELIST& imageList, Adesk::UInt32& normalImage, Adesk::UInt32& selected) const
	//{
	//	if( parent() != NULL )
	//		return parent()->getImages(imageList, normalImage, selected);
	//	else
	//		return AcLyLayerFilter::getImages(imageList, normalImage, selected);
	//}

	static bool IsActive( AcRxClass* pFilterClass, AcApDocument* pDoc = NULL )
	{
		bool result = false;
		pDoc = pDoc ? pDoc : acDocManager->mdiActiveDocument();
		if( pDoc != NULL && LayerFilterManager::IsLayerPropertiesManagerActive() )
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
						{
							result = pCurrent->isKindOf( pFilterClass );
							// ExTrace(_T("Current filter: %s "), pCurrent->name());
						}
						delete pRoot;
					}
				}
			}
		}
		return result;
	}

	// document modification
	static bool SetActive( AcRxClass* pFilterClass, AcApDocument* pDoc = NULL )
	{
		bool result = false;
		pDoc = pDoc ? pDoc : acDocManager->mdiActiveDocument();
		if( pDoc != NULL ) // && LayerFilterManager::IsLayerPropertiesManagerActive() )
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
						AcLyLayerFilter* pFilter = Find( pFilterClass, pRoot);
						if( pFilter != NULL && pFilter != pCurrent )
							result = pMgr->setFilters( pRoot, pFilter ) == Acad::eOk;
						delete pRoot;
					}
				}
			}
		}
		return result;
	}

	static AcLyLayerFilter* Find( AcRxClass* pClass, const AcLyLayerFilter* parent, bool recursive = false )
	{
		AcLyLayerFilter* result = NULL;
		const AcArray<AcLyLayerFilter*>& filters = parent->getNestedFilters();
		int len = filters.length();
		for( int i = 0; i < len; i++ )
		{
			if( filters[i] != NULL )
			{
				if( filters[i]->isKindOf( pClass ) )
				{
					return filters[i];
				}
				else if( recursive )
				{
					AcLyLayerFilter* f = Find( pClass, filters[i], recursive );
					if( f != NULL )
						return f;
				}
			}
		}
		return NULL;
	}

	//virtual Acad::ErrorStatus Add( AcLyLayerFilter* pFilter )
	//{
	//	if( ! m_nested.find( pFilter ) )
	//		m_nested.append( pFilter );
	//	return Acad::eOk;
	//}

/// Nested filter protocol

	//inline virtual Acad::ErrorStatus addNested( AcLyLayerFilter* pFilter )
	//{
	//	if( pFilter == this )
	//		return Acad::eOk;
	//	int len = m_nested.length();
	//	if( ! m_nested.contains( pFilter ) )
	//	{
	//		m_nested.append( pFilter );
	//		DynamicLayerFilter* dlf = DynamicLayerFilter::cast( pFilter );
	//		if( dlf != NULL )
	//			dlf->setParent( this );
	//	}
	//	return Acad::eOk;
	//}

	//inline virtual Acad::ErrorStatus removeNested(AcLyLayerFilter* filter)
	//{
	//	m_nested.remove( filter );
	//	return Acad::eOk;
	//}

	//inline virtual const AcArray<AcLyLayerFilter*>& getNestedFilters() const
	//{
	//	DynamicLayerFilter* pThis = const_cast<DynamicLayerFilter*>( this );
	//	if( ! m_nestedFiltersLoaded )
	//	{
	//		pThis->m_nestedFiltersLoaded = true;
	//		const AcArray<AcLyLayerFilter*> nested = AcLyLayerFilter::getNestedFilters();
	//		pThis->m_nested.append( nested );
	//		ExTrace(_T("%s Nested filters found: %d"), isA()->name(), nested.length());
	//	}
	//	ExTrace(_T("%s::getNestedFilters(): count = %d"), isA()->name(), m_nested.length());
	//	return m_nested;
	//}

	//inline virtual Acad::ErrorStatus generateNested()
	//{
	//	m_nested.append( AcLyLayerFilter::getNestedFilters() );
	//}

protected:

	inline void setParent( AcLyLayerFilter* pParent )
	{
		m_pParent = pParent;
	}

	bool Debug( AcDbLayerTableRecord* layer, bool result ) const
	{

#ifdef DEBUG_FILTER_METHOD

		const ACHAR* name;
		layer->getName( name );
		ExTrace(_T("%s::filter( %s ) == %s"),
			this->isA()->name(), nme, result ? _T("true") : _T("false"));

#endif

		return result;
	}

	// true if the layer palette was active when the instance was created.
	// if this is false, it means that the layer filter was created for
	// filtering the layer dropdown control. If true, it could be for any
	// purpose.

	bool ClientIsLayerDropDownControl()
	{
		return m_bClientIsLayerDropDownControl;
	}

	virtual AcLyLayerFilter* parent() const
	{
		// return m_pParent != NULL ? m_pParent : AcLyLayerFilter::parent();
		return AcLyLayerFilter::parent();
	}

private:
	
	bool m_bClientIsLayerDropDownControl;
	bool m_bAllowNested;
	bool m_bFilterCalled;

	bool m_nestedFiltersLoaded;

	AcArray<AcLyLayerFilter*> m_nested;
	AcLyLayerFilter* m_pParent;

};

