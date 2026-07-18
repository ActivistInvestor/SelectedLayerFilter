#pragma once

#include "mgdinterop.h"
#include "DynamicLayerFilter.h"
#include "MgdDynamicLayerFilter.h"

using namespace QLayer;

class DynamicLayerFilterObjectFactory :	public AcMgObjectFactoryBase
{
public:
	DynamicLayerFilterObjectFactory( AcRxClass* pConcreteClass );
	virtual ~DynamicLayerFilterObjectFactory(void);

	virtual gcroot<DisposableWrapper_GcPtr> createRCW( AcRxObject* unmanagedPointer, bool autoDelete);

private:
	AcRxClass* m_pClass;
};
