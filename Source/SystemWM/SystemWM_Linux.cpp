#if __linux__
#include "SystemWM.h"
#include "SystemWM_Linux.h"
#include <iostream>

static bool CheckFlag(kui::Window::WindowFlag Flag, kui::Window::WindowFlag Value)
{
	return (Flag & Value) == Value;
}

kui::SystemWM::SysWindow* kui::SystemWM::NewWindow(Window* Parent, Vec2ui Size, Vec2ui Pos, std::string Title, Window::WindowFlag Flags)
{
	SysWindow* OutWindow = new SysWindow();
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

	return OutWindow;
}

void kui::SystemWM::DestroyWindow(SysWindow* Target)
{
	Target->X11.Destroy();
	delete Target;
}

void kui::SystemWM::SwapWindow(SysWindow* Target)
{
	Target->X11.Swap();
}

void kui::SystemWM::ActivateContext(SysWindow* Target)
{
	Target->X11.MakeContextCurrent();
}

Vec2ui kui::SystemWM::GetWindowSize(SysWindow* Target)
{
	return Target->X11.WindowSize;
}

void kui::SystemWM::UpdateWindow(SysWindow* Target)
{
	Target->X11.UpdateWindow();
}

bool kui::SystemWM::WindowHasFocus(SysWindow* Target)
{
	return Target->X11.HasFocus;
}

Vec2i kui::SystemWM::GetCursorPosition(SysWindow* Target)
{
	return Target->X11.CursorPosition;
}

Vec2ui kui::SystemWM::GetScreenSize()
{
	return X11Window::GetMainScreenResolution();
}

std::string kui::SystemWM::GetTextInput(SysWindow* Target)
{
	std::string NewText = Target->X11.TextInput;
	Target->X11.TextInput.clear();
	return NewText;
}

uint32_t kui::SystemWM::GetDesiredRefreshRate(SysWindow* From)
{
	return From->X11.GetMonitorRefreshRate();
}

void kui::SystemWM::SetWindowCursor(SysWindow* Target, Window::Cursor NewCursor)
{
	Target->X11.SetCursor(NewCursor);
}

float kui::SystemWM::GetDPIScale(SysWindow* Target)
{
	return Target->X11.GetDPIScale();
}

void kui::SystemWM::SetClipboardText(std::string NewText)
{
}

std::string kui::SystemWM::GetClipboardText()
{
	return X11Window::GetClipboard();
}

bool kui::SystemWM::IsLMBDown()
{
	return X11Window::IsLMBDown();
}

bool kui::SystemWM::IsRMBDown()
{
	return X11Window::IsRMBDown();
}

void kui::SystemWM::SetWindowSize(SysWindow* Target, Vec2ui Size)
{
	Target->X11.SetSize(Size);
}

void kui::SystemWM::SetWindowPosition(SysWindow* Target, Vec2ui NewPosition)
{
	Target->X11.SetPosition(NewPosition);
}

void kui::SystemWM::SetTitle(SysWindow* Target, std::string Text)
{
	Target->X11.SetTitle(Text.c_str());
}

bool kui::SystemWM::IsWindowFullScreen(SysWindow* Target)
{
	return Target->X11.IsMaximized();
}
bool kui::SystemWM::IsWindowMinimized(SysWindow* Target)
{
	return Target->X11.IsMinimized();
}

void kui::SystemWM::SetWindowMinSize(SysWindow* Target, Vec2ui MinSize)
{
	Target->X11.SetMinSize(MinSize);
}

void kui::SystemWM::SetWindowMaxSize(SysWindow* Target, Vec2ui MaxSize)
{
	Target->X11.SetMaxSize(MaxSize);
}

void kui::SystemWM::RestoreWindow(SysWindow* Target)
{
	Target->X11.Restore();
}

void kui::SystemWM::MinimizeWindow(SysWindow* Target)
{
	Target->X11.Minimize();
}

void kui::SystemWM::MaximizeWindow(SysWindow* Target)
{
	Target->X11.Maximize();
}

static bool CommandExists(std::string Command)
{
	return system(("command -v " + Command + " > /dev/null").c_str()) == 0;
}

void kui::SystemWM::MessageBox(std::string Text, std::string Title, int Type)
{
	if (Type < 0 || Type > 2)
	{
		return;
	}
	if (CommandExists("kdialog"))
	{
		std::array<const char*, 3> Types = { "msgbox", "sorry", "error" };

		system(("/usr/bin/env kdialog --title " + Title + " --" + Types[Type] + " \"" + Text + "\"").c_str());
		return;
	}

	if (CommandExists("zenity"))
	{
		std::array<const char*, 3> Types = { "info", "warning", "error" };

		system(("/usr/bin/env zenity --title " + Title + " --" + Types[Type] + " --text \"" + Text + "\"").c_str());
		return;
	}

	// If kdialog and zenity don't exist, there's no good way of creating a message box.
	// TODO: Maybe create a KlemmUI window containing the message?
	// Making GUI apps for Linux is great!
	std::cout << Title << ": " << Text << std::endl;
}

#endif