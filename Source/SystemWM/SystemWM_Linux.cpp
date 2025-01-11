#if __linux__ && !KLEMMUI_CUSTOM_SYSTEMWM
#include "SystemWM.h"
#include "SystemWM_Linux.h"
#include <kui/Platform.h>
#include <iostream>
using namespace kui::platform::linux;

static bool CheckFlag(kui::Window::WindowFlag Flag, kui::Window::WindowFlag Value)
{
	return (Flag & Value) == Value;
}

kui::systemWM::SysWindow* kui::systemWM::NewWindow(Window* Parent, Vec2ui Size, Vec2ui Pos, std::string Title, Window::WindowFlag Flags)
{
	SysWindow* OutWindow = new SysWindow();
#ifdef KLEMMUI_WAYLAND
	if (GetUseWayland())
	{
		OutWindow->Wayland = WaylandWindow();
		OutWindow->Wayland.Parent = Parent;
		OutWindow->Wayland.Create(Parent,
			Size, Pos,
			Title,
			CheckFlag(Flags, Window::WindowFlag::Borderless),
			CheckFlag(Flags, Window::WindowFlag::Resizable),
			CheckFlag(Flags, Window::WindowFlag::Popup));
	}
	else
#endif
	{
		OutWindow->X11 = X11Window();
		OutWindow->X11.Parent = Parent;
		OutWindow->X11.Create(
			Parent,
			Size, Pos,
			Title,
			CheckFlag(Flags, Window::WindowFlag::Borderless),
			CheckFlag(Flags, Window::WindowFlag::Resizable),
			CheckFlag(Flags, Window::WindowFlag::Popup)
		);
	}

	return OutWindow;
}

void kui::systemWM::DestroyWindow(SysWindow* Target)
{
	Target->X11.Destroy();
	delete Target;
}

void kui::systemWM::SwapWindow(SysWindow* Target)
{
	Target->X11.Swap();
}
void kui::systemWM::UpdateWindowFlags(SysWindow* Target, Window::WindowFlag NewFlags)
{
	Target->X11.SetBorderless(CheckFlag(NewFlags, Window::WindowFlag::Borderless));
	Target->X11.SetResizable(CheckFlag(NewFlags, Window::WindowFlag::Resizable));
	Target->X11.SetAlwaysOnTop(CheckFlag(NewFlags, Window::WindowFlag::AlwaysOnTop));
}

void kui::systemWM::ActivateContext(SysWindow* Target)
{
	Target->X11.MakeContextCurrent();
}

kui::Vec2ui kui::systemWM::GetWindowSize(SysWindow* Target)
{
	return Target->X11.WindowSize;
}

void kui::systemWM::UpdateWindow(SysWindow* Target)
{
	Target->X11.UpdateWindow();
}

bool kui::systemWM::WindowHasFocus(SysWindow* Target)
{
	return Target->X11.HasFocus;
}

kui::Vec2i kui::systemWM::GetCursorPosition(SysWindow* Target)
{
	return Target->X11.CursorPosition;
}

kui::Vec2ui kui::systemWM::GetScreenSize()
{
	return X11Window::GetMainScreenResolution();
}

std::string kui::systemWM::GetTextInput(SysWindow* Target)
{
	std::string NewText = Target->X11.TextInput;
	Target->X11.TextInput.clear();
	return NewText;
}

uint32_t kui::systemWM::GetDesiredRefreshRate(SysWindow* From)
{
	return From->X11.GetMonitorRefreshRate();
}

void kui::systemWM::SetWindowCursor(SysWindow* Target, Window::Cursor NewCursor)
{
	Target->X11.SetCursor(NewCursor);
}

float kui::systemWM::GetDPIScale(SysWindow* Target)
{
	return Target->X11.GetDPIScale();
}

void kui::systemWM::SetClipboardText(std::string NewText)
{
}

std::string kui::systemWM::GetClipboardText()
{
	return X11Window::GetClipboard();
}

bool kui::systemWM::IsLMBDown()
{
	return X11Window::IsLMBDown();
}

bool kui::systemWM::IsRMBDown()
{
	return X11Window::IsRMBDown();
}

void kui::systemWM::SetWindowSize(SysWindow* Target, Vec2ui Size)
{
	Target->X11.SetSize(Size);
}

void kui::systemWM::SetWindowPosition(SysWindow* Target, Vec2ui NewPosition)
{
	Target->X11.SetPosition(NewPosition);
}

void kui::systemWM::SetTitle(SysWindow* Target, std::string Text)
{
	Target->X11.SetTitle(Text.c_str());
}

bool kui::systemWM::IsWindowFullScreen(SysWindow* Target)
{
	return Target->X11.IsMaximized();
}
bool kui::systemWM::IsWindowMinimized(SysWindow* Target)
{
	return Target->X11.IsMinimized();
}

void kui::systemWM::SetWindowMinSize(SysWindow* Target, Vec2ui MinSize)
{
	Target->X11.SetMinSize(MinSize);
}

void kui::systemWM::SetWindowMaxSize(SysWindow* Target, Vec2ui MaxSize)
{
	Target->X11.SetMaxSize(MaxSize);
}

void kui::systemWM::RestoreWindow(SysWindow* Target)
{
	Target->X11.Restore();
}

void kui::systemWM::MinimizeWindow(SysWindow* Target)
{
	Target->X11.Minimize();
}

void kui::systemWM::MaximizeWindow(SysWindow* Target)
{
	Target->X11.Maximize();
}

static bool CommandExists(std::string Command)
{
	return system(("command -v " + Command + " > /dev/null").c_str()) == 0;
}

void kui::systemWM::SetWindowIcon(SysWindow* Target, uint8_t* Bytes, size_t Width, size_t Height)
{
	Target->X11.SetIcon(Bytes, Width, Height);
}

void* kui::systemWM::GetPlatformHandle(SysWindow* Target)
{
	return reinterpret_cast<void*>(Target->X11.XWindow);
}

void kui::systemWM::WaitFrame(SysWindow* Target, float RemainingTime)
{
	std::this_thread::sleep_for(std::chrono::microseconds(int(RemainingTime * 1'000'000.0f)));
}

void kui::systemWM::MessageBox(std::string Text, std::string Title, int Type)
{
	if (Type < 0 || Type > 2)
	{
		return;
	}
	if (CommandExists("kdialog"))
	{
		std::array<const char*, 3> Types = { "msgbox", "sorry", "error" };

		system(("/usr/bin/env kdialog --title \"" + Title + "\" --" + Types[Type] + " \"" + Text + "\"").c_str());
		return;
	}

	if (CommandExists("zenity"))
	{
		std::array<const char*, 3> Types = { "info", "warning", "error" };

		system(("/usr/bin/env zenity --title \"" + Title + "\" --" + Types[Type] + " --text \"" + Text + "\"").c_str());
		return;
	}

	// If kdialog and zenity don't exist, there's no good way of creating a message box.
	// TODO: Maybe create a KlemmUI window containing the message?
	// Making GUI apps for Linux is great!
	std::cout << Title << ": " << Text << std::endl;
}

#endif