#pragma once

using namespace System;
using namespace System::Diagnostics;

ref class MgdStackWalker
{
public:

	MgdStackWalker(void)
	{
		StackTrace^ str = gcnew StackTrace(1);
		cli::array<StackFrame^>^ frames = str->GetFrames();
		if( frames != nullptr )
			this->FrameCount = frames->Length;
		else
			this->FrameCount = 0;
	}

	property int FrameCount;

	virtual ~MgdStackWalker(void)
	{
	}
};
