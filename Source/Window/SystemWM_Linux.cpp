#if __linux__
#include "SystemWM.h"
#include "SystemWM_Linux.h"
#include <iostream>

static bool CheckFlag(KlemmUI::Window::WindowFlag Flag, KlemmUI::Window::WindowFlag Value)
{
	return (Flag & Value) == Value;
}

KlemmUI::SystemWM::SysWindow* KlemmUI::SystemWM::NewWindow(Window* Parent, Vector2ui Size, Vector2ui Pos, std::string Title, Window::WindowFlag Flags)
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

void KlemmUI::SystemWM::DestroyWindow(SysWindow* Target)
{
	Target->X11.Destroy();
	delete Target;
}

void KlemmUI::SystemWM::SwapWindow(SysWindow* Target)
{
	Target->X11.Swap();
}

void KlemmUI::SystemWM::ActivateContext(SysWindow* Target)
{
	Target->X11.MakeContextCurrent();
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
	return Target->X11.HasFocus;
}

Vector2i KlemmUI::SystemWM::GetCursorPosition(SysWindow* Target)
{
	return Target->X11.CursorPosition;
}

Vector2ui KlemmUI::SystemWM::GetScreenSize()
{
	return X11Window::GetMainScreenResolution();
}

std::string KlemmUI::SystemWM::GetTextInput(SysWindow* Target)
{
	std::string NewText = Target->X11.TextInput;
	Target->X11.TextInput.clear();
	return NewText;
}

uint32_t KlemmUI::SystemWM::GetDesiredRefreshRate(SysWindow* From)
{
	return From->X11.GetMonitorRefreshRate();
}

void KlemmUI::SystemWM::SetWindowCursor(SysWindow* Target, Window::Cursor NewCursor)
{
	Target->X11.SetCursor(NewCursor);
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
	return X11Window::GetClipboard();
}

bool KlemmUI::SystemWM::IsLMBDown()
{
	return X11Window::IsLMBDown();
}

bool KlemmUI::SystemWM::IsRMBDown()
{
	return X11Window::IsRMBDown();
}

void KlemmUI::SystemWM::SetWindowSize(SysWindow* Target, Vector2ui Size)
{
	Target->X11.SetSize(Size);
}

void KlemmUI::SystemWM::SetWindowPosition(SysWindow* Target, Vector2ui NewPosition)
{
	Target->X11.SetPosition(NewPosition);
}

void KlemmUI::SystemWM::SetTitle(SysWindow* Target, std::string Text)
{
	Target->X11.SetTitle(Text.c_str());
}

bool KlemmUI::SystemWM::IsWindowFullScreen(SysWindow* Target)
{
	return Target->X11.IsMaximized();
}
bool KlemmUI::SystemWM::IsWindowMinimized(SysWindow* Target)
{
	return Target->X11.IsMinimized();
}

void KlemmUI::SystemWM::SetWindowMinSize(SysWindow* Target, Vector2ui MinSize)
{
	Target->X11.SetMinSize(MinSize);
}

void KlemmUI::SystemWM::SetWindowMaxSize(SysWindow* Target, Vector2ui MaxSize)
{
	Target->X11.SetMaxSize(MaxSize);
}

void KlemmUI::SystemWM::RestoreWindow(SysWindow* Target)
{
	Target->X11.Restore();
}

void KlemmUI::SystemWM::MinimizeWindow(SysWindow* Target)
{
	Target->X11.Minimize();
}

void KlemmUI::SystemWM::MaximizeWindow(SysWindow* Target)
{
	Target->X11.Maximize();
}

static bool CommandExists(std::string Command)
{
	return system(("command -v " + Command + " > /dev/null").c_str()) == 0;
}

void KlemmUI::SystemWM::MessageBox(std::string Text, std::string Title, int Type)
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