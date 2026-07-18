#pragma once

#include "acly.h"
#include "LayerFilterManager.h"
#include "DynamicLayerFilter.h"


class RecentLayerFilter :	public DynamicLayerFilter
{
public:

	ACRX_DECLARE_MEMBERS( RecentLayerFilter );

	RecentLayerFilter() 
	{
		setName( RECENT_LAYER_FILTER_NAME );
//#ifdef EXTRACE
//		TRACE_CTOR(this)
//#endif		
	}

	virtual ~RecentLayerFilter()
	{
//#ifdef EXTRACE
//		TRACE_DTOR(this)
//#endif
	}

	virtual bool filter( AcDbLayerTableRecord* layer ) const
	{
		return DynamicLayerFilter::filter( layer ) || LayerFilterManager::FilterRecentLayer( layer );
	}

	static RecentLayerFilter* Find( const AcLyLayerFilter* parent )
	{
		return RecentLayerFilter::cast( DynamicLayerFilter::Find( RecentLayerFilter::desc(), parent, true ) );
	}

	static bool IsActive( AcApDocument* pDoc = NULL )
	{
		return DynamicLayerFilter::IsActive( RecentLayerFilter::desc(), pDoc );
	}

private:


};

