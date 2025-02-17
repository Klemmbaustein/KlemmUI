#include <kui/Platform.h>

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