#if __linux__ && KLEMMUI_WAYLAND && !KLEMMUI_CUSTOM_SYSTEMWM
#include "SystemWM_Wayland.h"


void kui::systemWM::WaylandWindow::Create(Window* Parent, Vec2ui Size, Vec2ui Pos, std::string Title, bool Borderless, bool Resizable, bool AlwaysOnTop)
{
}

#endif