#include "StdAfx.h"
#include "DynamicLayerFilterObjectFactory.h"

using namespace QLayer;

DynamicLayerFilterObjectFactory::DynamicLayerFilterObjectFactory( AcRxClass* pConcreteClass )
	: m_pClass( pConcreteClass )
{
	if( m_pClass != NULL )
		m_pClass->addX(AcMgObjectFactoryBase::desc(), this);
}

DynamicLayerFilterObjectFactory::~DynamicLayerFilterObjectFactory(void)
{
	if( m_pClass != NULL )
		m_pClass->delX(AcMgObjectFactoryBase::desc());
}


gcroot<DisposableWrapper_GcPtr> DynamicLayerFilterObjectFactory::createRCW( AcRxObject* unmanagedPointer, bool autoDelete)
{
	MgdDynamicLayerFilter^ filter = gcnew MgdDynamicLayerFilter();
	Autodesk::AutoCAD::Runtime::Interop::AttachUnmanagedObject( filter, IntPtr((void*) unmanagedPointer), autoDelete );
	return filter;
}
