#if __linux__
#include "SystemWM.h"
#include "SystemWM_Linux.h"
#include <iostream>

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


void KlemmUI::SystemWM::SetWindowSize(SysWindow* Target, Vector2ui Size)
{
}

void KlemmUI::SystemWM::SetWindowPosition(SysWindow* Target, Vector2ui NewPosition)
{
}

void KlemmUI::SystemWM::SetTitle(SysWindow* Target, std::string Text)
{
}

bool KlemmUI::SystemWM::IsWindowFullScreen(SysWindow* Target)
{
	return false;
}

void KlemmUI::SystemWM::SetWindowMinSize(SysWindow* Target, Vector2ui MinSize)
{
}

void KlemmUI::SystemWM::SetWindowMaxSize(SysWindow* Target, Vector2ui MaxSize)
{
}

void KlemmUI::SystemWM::RestoreWindow(SysWindow* Target)
{
}

void KlemmUI::SystemWM::MinimizeWindow(SysWindow* Target)
{
}

void KlemmUI::SystemWM::MaximizeWindow(SysWindow* Target)
{
}

static bool CommandExists(std::string Command)
{
	return system(("command -v " + Command + " > /dev/null").c_str()) == 0;
}

void KlemmUI::SystemWM::MessageBox(std::string Text, std::string Title, int Type)
{
	if (CommandExists("kdialog"))
	{
		std::string ErrorType;
		switch (Type)
		{
		case 0:
			ErrorType = "msgbox";
			break;
		case 1:
			ErrorType = "sorry";
			break;
		case 2:
			ErrorType = "error";
			break;
		default:
			break;
		}

		system(("/usr/bin/env kdialog --title " + Title + " --" + ErrorType + " \"" + Text + "\"").c_str());
		return;
	}

	if (CommandExists("zenity"))
	{
		std::string ErrorType;
		switch (Type)
		{
		case 0:
			ErrorType = "info";
			break;
		case 1:
			ErrorType = "warming";
			break;
		case 2:
			ErrorType = "error";
			break;
		default:
			break;
		}

		system(("/usr/bin/env zenity --title " + Title + " --" + ErrorType + " --text \"" + Text + "\"").c_str());
		return;
	}

	// If kdialog and zenity don't exist, there's no good way of creating a message box.
	// Making GUI apps for Linux is great!
	std::cout << Title << ": " << Text << std::endl;
}

#endif