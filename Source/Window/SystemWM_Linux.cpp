#if __linux__
#include "SystemWM.h"
#include "SystemWM_Linux.h"

KlemmUI::SystemWM::SysWindow* KlemmUI::SystemWM::NewWindow(Window* Parent, Vector2ui Size, Vector2ui Pos, std::string Title, bool Borderless, bool Resizable, bool Popup)
{
	SysWindow* OutWindow = new SysWindow();
	OutWindow->X11 = X11Window();
	OutWindow->X11.Parent = Parent;
	OutWindow->X11.Create(Parent, Size, Pos, Title, Borderless, Resizable, Popup);
	return OutWindow;
}

void KlemmUI::SystemWM::DestroyWindow(SysWindow* Target)
{
}

void KlemmUI::SystemWM::SwapWindow(SysWindow* Target)
{
	Target->X11.Swap();
}

void KlemmUI::SystemWM::ActivateContext(SysWindow* Target)
{
}

Vector2ui KlemmUI::SystemWM::GetWindowSize(SysWindow* Target)
{
	return Target->X11.WindowSize;
}

void KlemmUI::SystemWM::UpdateWindow(SysWindow* Target)
{
	Target->X11.UpdateWindow();
}

bool KlemmUI::SystemWM::WindowHasFocus(SysWindow* Target)
{
	return true;
}

Vector2ui KlemmUI::SystemWM::GetCursorPosition(SysWindow* Target)
{
	return Target->X11.CursorPosition;
}

Vector2ui KlemmUI::SystemWM::GetScreenSize()
{
	return Vector2ui(1920, 1080);
}

std::string KlemmUI::SystemWM::GetTextInput(SysWindow* Target)
{
	return "";
}

uint32_t KlemmUI::SystemWM::GetDesiredRefreshRate(SysWindow* From)
{
	return 60;
}

void KlemmUI::SystemWM::SetWindowCursor(SysWindow* Target, Window::Cursor NewCursor)
{
}

float KlemmUI::SystemWM::GetDPIScale(SysWindow* Target)
{
	return 1;
}

void KlemmUI::SystemWM::SetClipboardText(std::string NewText)
{
}

std::string KlemmUI::SystemWM::GetClipboardText()
{
	return "";
}

bool KlemmUI::SystemWM::IsLMBDown()
{
	return false;
}

bool KlemmUI::SystemWM::IsRMBDown()
{
	return false;
}


#endif