#ifdef KLEMMUI_WAYLAND
#pragma once
#include "SystemWM.h"

namespace kui::systemWM
{
	class WaylandWindow
	{
	public:
		WaylandWindow() {};
		~WaylandWindow() {};

		Window* Parent;

		void Create(Window* Parent, Vec2ui Size, Vec2ui Pos, std::string Title, bool Borderless, bool Resizable, bool AlwaysOnTop);
	};
}

#endif