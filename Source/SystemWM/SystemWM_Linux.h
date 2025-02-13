#if __linux__ && !KLEMMUI_CUSTOM_SYSTEMWM && !KLEMMUI_WITH_SDL
#pragma once
#include "SystemWM.h"
#include "SystemWM_X11.h"

namespace kui::systemWM
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