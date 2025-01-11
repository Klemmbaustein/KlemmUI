#pragma once
#if _WIN32
#define KLEMMUI_WINDOWS
#elif __linux__
#define KLEMMUI_LINUX
#elif KLEMMUI_WEB_BUILD
#define KLEMMUI_WEB
#else
#error Unknown Platform
#endif
#include "Window.h"

namespace kui::platform
{
	namespace win32
	{
		enum class WindowFlag : int
		{
			DarkTitleBar = 1 << 16,
		};
	}

	namespace linux
	{
#ifdef KLEMMUI_WAYLAND
		bool GetUseWayland();

		void SetUseWayland();
#endif
	}
}


inline kui::Window::WindowFlag operator|(kui::Window::WindowFlag a, kui::platform::win32::WindowFlag b)
{
	return kui::Window::WindowFlag(int(a) | int(b));
}
