#if __linux__
#pragma once
#include "SystemWM.h"
#include "SystemWM_X11.h"

namespace KlemmUI::SystemWM
{

	class SysWindow
	{
	public:
		union
		{
			X11Window X11 = {};
		};

	};
}
#endif