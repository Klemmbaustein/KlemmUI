#if __linux__ && !KLEMMUI_CUSTOM_SYSTEMWM
#pragma once
#include "SystemWM.h"
#include "SystemWM_X11.h"
#include "SystemWM_Wayland.h"

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
#ifdef KLEMMUI_WAYLAND
			WaylandWindow Wayland;
#endif
		};
	};
}
#endif