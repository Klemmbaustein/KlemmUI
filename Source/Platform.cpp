#include <kui/Platform.h>

#ifdef KLEMMUI_WAYLAND
static bool UseWayland = false;

bool kui::platform::linux::GetUseWayland()
{
	return UseWayland;
}

void kui::platform::linux::SetUseWayland()
{
	UseWayland = true;
}
#endif