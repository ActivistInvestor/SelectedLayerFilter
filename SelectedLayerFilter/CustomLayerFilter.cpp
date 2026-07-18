#include "StdAfx.h"
#include "CustomLayerFilter.h"

ACRX_CONS_DEFINE_MEMBERS( CustomLayerFilter, AcLyLayerFilter, AcDb::kDHL_CURRENT )

ACRX_CONS_DEFINE_MEMBERS( ChildFilter, AcLyLayerFilter, AcDb::kDHL_CURRENT )


Acad::ErrorStatus CustomLayerFilter::generateNested()
{
	//if( ! m_bNestedCreated )
	//{
	//	ChildFilter* pChild = new ChildFilter();
	//	pChild->setParent(this);
	//	m_nested.append( pChild );

	//	m_bNestedCreated = true;
	//}
	if( ! m_bNestedCreated )
	{
		ChildFilter* pChild = new ChildFilter();
		// pChild->setName(_T("ChildFilter"));
		m_bNestedCreated = true;
		return this->addNested( pChild );
	}
	return Acad::eOk;
}
