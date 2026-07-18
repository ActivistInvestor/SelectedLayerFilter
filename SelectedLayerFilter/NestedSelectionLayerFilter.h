#pragma once

#include "acly.h"
#include "DynamicLayerFilter.h"
#include "NestedSelectionLayerFilter.h"
#include "LayerFilterManager.h"
#include "LayerFilterDocData.h"
#include "BlockLayerFilter.h"

class NestedSelectionLayerFilter : public DynamicLayerFilter
{
public:

	ACRX_DECLARE_MEMBERS( NestedSelectionLayerFilter );

	inline NestedSelectionLayerFilter() // : m_pParent(NULL)
	{
		setName( NESTED_SELECTED_LAYER_FILTER_NAME );
//#ifdef EXTRACE
//		TRACE_CTOR(this)
//#endif
	}

	//void setParent( AcLyLayerFilter* parent )
	//{
	//	m_pParent = parent;
	//}

	inline virtual ~NestedSelectionLayerFilter()
	{
//#ifdef EXTRACE
//		TRACE_DTOR(this)
//#endif
	}

 //  virtual bool isProxy() const
	//{
	//	return false;
	//}

	//virtual AcLyLayerFilter* parent() const
	//{
	//	return m_pParent;
	//}

	//void SetParent( AcLyLayerFilter* pParent )
	//{
	//}

	//virtual bool isDynamicallyGenerated() const
	//{
	//	return true;
	//}

	//virtual const AcArray<AcLyLayerFilter*>& getNestedFilters() const
	//{
	//	LayerFilterManager* pMgr = LayerFilterManager::GetInstance(false);
	//	const AcArray<AcLyLayerFilter*>& filters = pMgr->docData().getBlockFilters();
	//	int cnt = filters.length();
	//	if( cnt > 0 )
	//		ExTrace(_T("block filter count = %s"), cnt);
	//	for( int i = 0; i < cnt; i++ )
	//	{
	//		AcLyLayerFilter* lf = filters[i];
	//		AcString desc = lf->isA()->name();
	//		BlockLayerFilter* blf = BlockLayerFilter::cast( lf );
	//		if( blf == NULL )
	//			assert(0);
	//		blf->setParent( this );
	//	}
	//	return filters;
	//}

	inline virtual bool filter( AcDbLayerTableRecord* layer ) const
	{
		// return Debug( layer, DynamicLayerFilter::filter( layer ) || LayerFilterManager::FilterNestedSelectionLayer( layer ) );
		return DynamicLayerFilter::filter( layer ) || LayerFilterManager::FilterNestedSelectionLayer( layer );
	}

	static NestedSelectionLayerFilter* Find( const AcLyLayerFilter* parent )
	{
		return NestedSelectionLayerFilter::cast( DynamicLayerFilter::Find( desc(), parent ) );
	}

	static bool IsActive( AcApDocument* pDoc = NULL )
	{
		bool result = DynamicLayerFilter::IsActive( desc(), pDoc );
		//ExTrace(_T("%s::IsActive() returned %s"),
		//	desc()->name(), result ? _T("true") : _T("false"));
		return result;
	}

	static bool SetActive( AcApDocument* pDoc = NULL )
	{
		return DynamicLayerFilter::SetActive( desc(), pDoc );
	}

private:

	// AcLyLayerFilter* m_pParent;

};

