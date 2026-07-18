#pragma once

#include "acly.h"
#include "LayerFilterManager.h"
#include "LayerFilterDocData.h"
#include "DynamicLayerFilter.h"
#include "NestedSelectionLayerFilter.h"

class SelectionLayerFilter :	public DynamicLayerFilter
{
public:

	ACRX_DECLARE_MEMBERS( SelectionLayerFilter );

	SelectionLayerFilter() // : nestedFilter(NULL)
	{
		setName( SELECTED_LAYER_FILTER_NAME );
		//AcArray<AcLyLayerFilter*> nested = this->getNestedFilters();
		//if( nested.length() == 0 )
		//	this->Add( new NestedSelectionLayerFilter() );
		// TRACE_CTOR(this)
		generateNested();
	}

	inline virtual ~SelectionLayerFilter()
	{
		//if( AcadIsQuitting() )
		//{
		//	AcArray<AcLyLayerFilter*> nested;
		//	nested.append( DynamicLayerFilter::getNestedFilters() );
		//	int len = nested.length();
		//	for( int i = 0; i < len; i++ )
		//		this->removeNested( nested[i] );
		//}

		//if( ! AcadIsQuitting() )
		//{
		//	for( int i = 0; i < len; i++ )
		//	{
		//		AcLyLayerFilter* filter = nested[i];
		//		if( filter != NULL )
		//			delete filter;
		//	}
		//}


		//if( nestedFilter != NULL && ! AcadIsQuitting() )
		//{
		//	delete nestedFilter;
		//	nestedFilter = NULL;
		//	nestedFilters.setLogicalLength(0);
		//}
		//TRACE_DTOR(this)
	}

	inline virtual bool filter( AcDbLayerTableRecord* layer ) const
	{
		return DynamicLayerFilter::filter( layer ) || LayerFilterManager::FilterSelectionLayer( layer );
	}

	inline virtual Acad::ErrorStatus generateNested()
	{
		if( this->getNestedFilters().length() == 0 )
		{
			this->addNested( new NestedSelectionLayerFilter() );
		}
		return Acad::eOk;
	}

	//inline virtual Acad::ErrorStatus generateNested()
	//{
	//	addNested( new NestedSelectionLayerFilter() );

	//	//if( nestedFilter == NULL )
	//	//{
	//	//	nestedFilter = new NestedSelectionLayerFilter();
	//	//	((NestedSelectionLayerFilter*) nestedFilter)->setParent( this );
	//	//	nestedFilters.setLogicalLength(0);
	//	//	nestedFilters.append(nestedFilter);
	//	//}
	//	return Acad::eOk;
	//}

	//inline virtual const AcArray<AcLyLayerFilter*>& getNestedFilters() const
	//{
	//	if( AcadIsQuitting() )
	//		return nestedFilters;
	//	else
	//		return DynamicLayerFilter::getNestedFilters();
	//}
	
	static SelectionLayerFilter* Find( const AcLyLayerFilter* parent )
	{
		return SelectionLayerFilter::cast( DynamicLayerFilter::Find( desc(), parent, true ) );
	}

	static bool IsActive( AcApDocument* pDoc = NULL )
	{
		return DynamicLayerFilter::IsActive( desc(), pDoc );
	}

	static bool SetActive( AcApDocument* pDoc = NULL )
	{
		return DynamicLayerFilter::SetActive( desc(), pDoc );
	}

private:
	
	//AcLyLayerFilter* nestedFilter;
	// AcArray<AcLyLayerFilter*> nestedFilters;

};

