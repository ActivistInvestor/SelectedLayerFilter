#pragma once

#include "acly.h"
#include "DynamicLayerFilter.h"
#include "BlockLayerFilter.h"
#include "LayerFilterManager.h"
#include "LayerFilterDocData.h"

class BlockLayerFilter : public DynamicLayerFilter
{
public:

	ACRX_DECLARE_MEMBERS( BlockLayerFilter );

	BlockLayerFilter() : m_pParent(NULL)
	{
	}

	BlockLayerFilter( AcDbObjectId blockId ) : m_pParent(NULL), m_blockId( blockId )
	{
		AcDbBlockTableRecordPointer pBtr( blockId, AcDb::kForRead );
		if( pBtr.openStatus() == Acad::eOk )
		{
			const ACHAR* name;
			pBtr->getName( name );
			setName( name );
		}
		else
			setName( _T( "Unknown" ) );

		TRACE_CTOR(this)
	}

	void setParent( const AcLyLayerFilter* parent )
	{
		m_pParent = const_cast<AcLyLayerFilter*>( parent );
	}

	bool Add( AcDbObjectId layerId )
	{
		return m_layers.Add(layerId);
	}

	int Count() const
	{
		return m_layers.Count();
	}

	virtual ~BlockLayerFilter()
	{
		TRACE_DTOR(this)
	}

   virtual bool isProxy() const
	{
		return false;
	}

	virtual AcLyLayerFilter* parent() const
	{
		return m_pParent;
	}

	virtual bool isDynamicallyGenerated() const
	{
		return true;
	}

	virtual bool filter( AcDbLayerTableRecord* layer ) const
	{
		// return Debug( layer, DynamicLayerFilter::filter( layer ) || LayerFilterManager::FilterNestedSelectionLayer( layer ) );
		// return DynamicLayerFilter::filter( layer ) || LayerFilterManager::FilterNestedSelectionLayer( layer );
		return m_layers.Contains( layer->objectId() );
	}

	//static BlockLayerFilter* Find( const AcLyLayerFilter* parent )
	//{
	//	return BlockLayerFilter::cast( DynamicLayerFilter::Find( desc(), parent ) );
	//}

	//static bool IsActive( AcApDocument* pDoc = NULL )
	//{
	//	bool result = DynamicLayerFilter::IsActive( desc(), pDoc );
	//	ExTrace(_T("%s::IsActive() returned %s"),
	//		desc()->name(), result ? _T("true") : _T("false"));
	//	return result;
	//}

	//static bool SetActive( AcApDocument* pDoc = NULL )
	//{
	//	return DynamicLayerFilter::SetActive( desc(), pDoc );
	//}

private:

	AcLyLayerFilter* m_pParent;
	AcDbObjectId m_blockId;
	AcDbObjectIdSet m_layers;

};

