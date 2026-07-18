#include "StdAfx.h"
#include "MgdDynamicLayerFilter.h"

using namespace System;
using namespace Autodesk::AutoCAD::DatabaseServices;
using namespace Autodesk::AutoCAD::LayerManager;

namespace QLayer
{

MgdDynamicLayerFilter::MgdDynamicLayerFilter(void)
{
}

MgdDynamicLayerFilter::~MgdDynamicLayerFilter(void)
{
}

MgdDynamicLayerFilter::MgdDynamicLayerFilter( IntPtr unmanagedObject, bool autoDelete )
	: LayerFilter( unmanagedObject, autoDelete )
{
}

//void MgdDynamicLayerFilter::DeleteUnmanagedObject()
//{
//}

bool MgdDynamicLayerFilter::Filter( LayerTableRecord^ layer )
{
	// TODO: Check flag indicating if we are being consumed by
	//       the Layer dropdown control, or by another managed component.

	//AcDbLayerTableRecord* pLayer = static_cast<AcDbLayerTableRecord*>( (void*) layer->UnmanagedObject.ToPointer() );
	//if( pLayer != NULL )
	//{
	//	AcLyLayerFilter* imp = impObj();
	//	if( imp != NULL )
	//		return imp->filter( layer );
	//}

	return __super::Filter( layer );

	return true;
}

AcLyLayerFilter* MgdDynamicLayerFilter::impObj()
{
	IntPtr p = this->UnmanagedObject;
	if( p != IntPtr::Zero )
	{
		return static_cast<AcLyLayerFilter*>( (void*) p.ToPointer() );
	}
	return NULL;
}

} // Namespace 