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
#include <optional>

namespace kui::platform
{
	namespace win32
	{
		/**
		 * @brief
		 * Extended window flags for the windows platform.
		 */
		enum class WindowFlag : int
		{
			/// The window should have a dark title bar.
			DarkTitleBar = 1 << 16,
		};
	}

	void SetAppId(std::string NewAppId);
	std::string GetAppId();

	namespace linux
	{
#if __linux__

		bool GetUseWayland();

		void AlwaysUseWayland();
		void AlwaysUseX11();
#else
		constexpr bool GetUseWayland()
		{
			return false;
		}
		constexpr void AlwaysUseWayland() {}
		constexpr void AlwaysUseX11() {}
#endif
	}

	namespace win32
	{
		/**
		 * @brief
		 * Controls window rounding on Windows 11 or later.
		 *
		 * This should be left on "Default" in most cases but is nice to have as an option.
		 */
		enum class WindowRoundingMode
		{
			/// Let the OS decide. Equals to large on Windows 11.
			Default,
			/// Very small (approx. 3px) rounded corners.
			Small,
			/// Larger (approx. 8px) rounded corners.
			Large,
			/// No rounded corners, like with Windows 10. Can't be named `None` because XLib globally defines
			/// that identifier (Ty xlib)
			NoCorners
		};

#if _WIN32 && !defined(KLEMMUI_CUSTOM_SYSTEMWM)
		void SetWindowTitleBarColor(Window* TargetWindow, Vec3f Color, std::optional<Vec3f> TextColor);
		void SetWindowBorderRounding(Window* TargetWindow, WindowRoundingMode Rounding);
#else
		constexpr void SetWindowTitleBarColor(Window*, Vec3f, Vec3f, std::optional<Vec3f> TextColor) {}
		constexpr void SetWindowBorderRounding(Window* TargetWindow, WindowRoundingMode Rounding) {}
#endif
	}
}


inline kui::Window::WindowFlag operator|(kui::Window::WindowFlag a, kui::platform::win32::WindowFlag b)
{
	return kui::Window::WindowFlag(int(a) | int(b));
}
