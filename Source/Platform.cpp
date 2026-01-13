#include <kui/Platform.h>
#include "SystemWM/SystemWM_Win32.h"

#if __linux__
#if KLEMMUI_WITH_WAYLAND
static bool UseWayland = getenv("WAYLAND_DISPLAY");
#endif
bool kui::platform::linux::GetUseWayland()
{
#if KLEMMUI_WITH_WAYLAND
	return UseWayland;
#endif
	return false;
}

void kui::platform::linux::AlwaysUseX11()
{
#if KLEMMUI_WITH_WAYLAND
	UseWayland = false;
#endif
}

void kui::platform::linux::AlwaysUseWayland()
{
#if KLEMMUI_WITH_WAYLAND
	UseWayland = true;
#endif
}
#endif

static std::string AppIdString = "generic-kui-app";

void kui::platform::SetAppId(std::string NewAppId)
{
	AppIdString = NewAppId;
}

std::string kui::platform::GetAppId()
{
	return AppIdString;
}

#if defined(_WIN32) && !defined(KLEMMUI_CUSTOM_SYSTEMWM)
#include <dwmapi.h>

void kui::platform::win32::SetWindowTitleBarColor(Window* TargetWindow, Vec3f Color, std::optional<Vec3f> TextColor)
{
	using namespace systemWM;

	SysWindow* WindowData = reinterpret_cast<SysWindow*>(TargetWindow->GetSysWindow());
	HWND hwnd = WindowData->WindowHandle;

	auto CaptionRgb = RGB(Color.X * 255, Color.Y * 255, Color.Z * 255);

	DwmSetWindowAttribute(
		hwnd, DWMWINDOWATTRIBUTE::DWMWA_CAPTION_COLOR,
		&CaptionRgb, sizeof(CaptionRgb));

	if (TextColor)
	{
		auto TextRgb = RGB(TextColor->X * 255, TextColor->Y * 255, TextColor->Z * 255);

		DwmSetWindowAttribute(
			hwnd, DWMWINDOWATTRIBUTE::DWMWA_TEXT_COLOR,
			&TextRgb, sizeof(TextRgb));
	}
}

void kui::platform::win32::SetWindowBorderRounding(Window* TargetWindow, WindowRoundingMode Rounding)
{
	using namespace systemWM;

	SysWindow* WindowData = reinterpret_cast<SysWindow*>(TargetWindow->GetSysWindow());
	HWND hwnd = WindowData->WindowHandle;

	DWM_WINDOW_CORNER_PREFERENCE CornerPref;
	switch (Rounding)
	{
	default:
	case WindowRoundingMode::Default:
	{
		CornerPref = DWMWCP_DEFAULT;
		break;
	}
	case WindowRoundingMode::Small:
	{
		CornerPref = DWMWCP_ROUNDSMALL;
		break;
	}
	case WindowRoundingMode::Large:
	{
		CornerPref = DWMWCP_ROUND;
		break;
	}
	case WindowRoundingMode::None:
	{
		CornerPref = DWMWCP_DONOTROUND;
		break;
	}
	}

	DwmSetWindowAttribute(
		hwnd, DWMWINDOWATTRIBUTE::DWMWA_WINDOW_CORNER_PREFERENCE,
		&CornerPref, sizeof(CornerPref));
}

#endif
