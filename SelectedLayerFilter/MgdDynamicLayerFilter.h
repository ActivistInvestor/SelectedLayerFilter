#pragma once

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace Autodesk::AutoCAD::DatabaseServices;
using namespace Autodesk::AutoCAD::LayerManager;

namespace QLayer
{

public ref class MgdDynamicLayerFilter : public LayerFilter
{
public:
	MgdDynamicLayerFilter(void);
	virtual ~MgdDynamicLayerFilter(void);

	// [return: MarshalAs( UnmanagedType::U1 )]
	virtual bool Filter( LayerTableRecord^ layer ) override;

protected:
	MgdDynamicLayerFilter( IntPtr unmanagedObject, bool autoDelete );
	// virtual void DeleteUnmanagedObject() override;

private:
	AcLyLayerFilter* impObj();

};

}