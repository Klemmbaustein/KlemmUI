#if __linux__
#pragma once
#include "SystemWM.h"
#include "SystemWM_X11.h"

namespace kui::SystemWM
{
	class SysWindow
	{
	public:
		SysWindow() {}
		~SysWindow() {}
		union
		{
			X11Window X11 = {};
		};
	};
}
#endif