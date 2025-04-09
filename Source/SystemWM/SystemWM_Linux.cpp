#if __linux__ && !KLEMMUI_CUSTOM_SYSTEMWM
#include "SystemWM.h"
#include "SystemWM_Linux.h"
#include <kui/Platform.h>
#include <iostream>
#include <thread>
#include <kui/StringReplace.h>
using namespace kui::platform::linux;

#ifdef KLEMMUI_WITH_WAYLAND
#define WAYLAND_FN(x) x
#else
#define WAYLAND_FN(...) (void*)0
#endif

static bool CheckFlag(kui::Window::WindowFlag Flag, kui::Window::WindowFlag Value)
{
	return (Flag & Value) == Value;
}

kui::systemWM::SysWindow* kui::systemWM::NewWindow(Window* Parent, Vec2ui Size, Vec2ui Pos, std::string Title, Window::WindowFlag Flags)
{
	SysWindow* OutWindow = new SysWindow();
#ifdef KLEMMUI_WITH_WAYLAND
	if (GetUseWayland())
	{
		OutWindow->Wayland = new WaylandWindow();
		OutWindow->Wayland->Parent = Parent;
		OutWindow->Wayland->Create(Parent,
			Size, Pos,
			Title,
			CheckFlag(Flags, Window::WindowFlag::Borderless),
			CheckFlag(Flags, Window::WindowFlag::Resizable),
			CheckFlag(Flags, Window::WindowFlag::Popup));
	}
	else
#endif
	{
		OutWindow->X11 = new X11Window();
		OutWindow->X11->Parent = Parent;
		OutWindow->X11->Create(
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
	if (GetUseWayland())
	{
#if KLEMMUI_WITH_WAYLAND
		Target->Wayland->Destroy();
		delete Target->Wayland;
#endif
	}
	else
	{
		Target->X11->Destroy();
		delete Target->X11;
	}

	delete Target;
}

void kui::systemWM::SwapWindow(SysWindow* Target)
{
	if (GetUseWayland())
	{
		WAYLAND_FN(Target->Wayland->Swap());
	}
	else
	{
		Target->X11->Swap();
	}
}
void kui::systemWM::UpdateWindowFlags(SysWindow* Target, Window::WindowFlag NewFlags)
{
	if (GetUseWayland())
	{
#if KLEMMUI_WITH_WAYLAND
		Target->Wayland->SetBorderless(CheckFlag(NewFlags, Window::WindowFlag::Borderless));
		Target->Wayland->SetResizable(CheckFlag(NewFlags, Window::WindowFlag::Resizable));
#endif
	}
	else
	{
		Target->X11->SetBorderless(CheckFlag(NewFlags, Window::WindowFlag::Borderless));
		Target->X11->SetResizable(CheckFlag(NewFlags, Window::WindowFlag::Resizable));
		Target->X11->SetAlwaysOnTop(CheckFlag(NewFlags, Window::WindowFlag::AlwaysOnTop));
	}
}

void kui::systemWM::ActivateContext(SysWindow* Target)
{
	if (GetUseWayland())
		WAYLAND_FN(Target->Wayland->MakeContextCurrent());
	else
		Target->X11->MakeContextCurrent();
}

kui::Vec2ui kui::systemWM::GetWindowSize(SysWindow* Target)
{
	if (GetUseWayland())
		WAYLAND_FN(return Target->Wayland->ContentSize);
	return Target->X11->WindowSize;
}

void kui::systemWM::UpdateWindow(SysWindow* Target)
{
	if (GetUseWayland())
		WAYLAND_FN(Target->Wayland->UpdateWindow());
	else
		Target->X11->UpdateWindow();
}

bool kui::systemWM::WindowHasMouseFocus(SysWindow* Target)
{
	// Thanks, xlib
#undef PointerWindow
	if (GetUseWayland())
		WAYLAND_FN(return Target->Wayland == WaylandConnection::GetConnection()->PointerWindow);
	return Target->X11->HasFocus;
}

bool kui::systemWM::WindowHasFocus(SysWindow* Target)
{
	if (GetUseWayland())
		WAYLAND_FN(return Target->Wayland == WaylandConnection::GetConnection()->KeyboardWindow);
	return Target->X11->HasFocus;
}

kui::Vec2i kui::systemWM::GetCursorPosition(SysWindow* Target)
{
	if (GetUseWayland())
		WAYLAND_FN(return Target->Wayland->MousePosition);
	return Target->X11->CursorPosition;
}

kui::Vec2ui kui::systemWM::GetScreenSize()
{
	if (GetUseWayland())
		WAYLAND_FN(return WaylandConnection::GetConnection()->PrimarySize);
	return X11Window::GetMainScreenResolution();
}

std::string kui::systemWM::GetTextInput(SysWindow* Target)
{
	if (GetUseWayland())
	{
#ifdef KLEMMUI_WITH_WAYLAND
		std::string NewText = Target->Wayland->TextInput;
		Target->Wayland->TextInput.clear();
		return NewText;
#endif
	}
	std::string NewText = Target->X11->TextInput;
	Target->X11->TextInput.clear();
	return NewText;
}

uint32_t kui::systemWM::GetDesiredRefreshRate(SysWindow* From)
{
	if (GetUseWayland())
		return 120;
	return From->X11->GetMonitorRefreshRate();
}

void kui::systemWM::SetWindowCursor(SysWindow* Target, Window::Cursor NewCursor)
{
	if (GetUseWayland())
		WAYLAND_FN(Target->Wayland->SetCursor(NewCursor));
	else
		Target->X11->SetCursor(NewCursor);
}

float kui::systemWM::GetDPIScale(SysWindow* Target)
{
	if (GetUseWayland())
		return 1;
	return Target->X11->GetDPIScale();
}

void kui::systemWM::SetClipboardText(std::string NewText)
{
	if (GetUseWayland())
	{
		WAYLAND_FN(WaylandConnection::GetConnection()->Clipboard.SetSelectionString(NewText));
	}
}

std::string kui::systemWM::GetClipboardText()
{
	if (GetUseWayland())
	{
		WAYLAND_FN(return WaylandConnection::GetConnection()->Clipboard.GetSelectionString());
	}
	return X11Window::GetClipboard();
}

bool kui::systemWM::IsLMBDown()
{
	if (GetUseWayland())
		WAYLAND_FN(return WaylandConnection::GetConnection()->IsLMBDown);
	return X11Window::IsLMBDown();
}

bool kui::systemWM::IsRMBDown()
{
	if (GetUseWayland())
		WAYLAND_FN(return WaylandConnection::GetConnection()->IsRMBDown);
	return X11Window::IsRMBDown();
}

void kui::systemWM::SetWindowSize(SysWindow* Target, Vec2ui Size)
{
	if (GetUseWayland())
		return;
	Target->X11->SetSize(Size);
}

void kui::systemWM::SetWindowPosition(SysWindow* Target, Vec2ui NewPosition)
{
	if (GetUseWayland())
		return;
	Target->X11->SetPosition(NewPosition);
}

void kui::systemWM::SetTitle(SysWindow* Target, std::string Text)
{
	if (GetUseWayland())
		WAYLAND_FN(Target->Wayland->SetTitle(Text));
	else
		Target->X11->SetTitle(Text);
}

bool kui::systemWM::IsWindowFullScreen(SysWindow* Target)
{
	if (GetUseWayland())
		WAYLAND_FN(return Target->Wayland->Maximized);
	return Target->X11->IsMaximized();
}
bool kui::systemWM::IsWindowMinimized(SysWindow* Target)
{
	if (GetUseWayland())
		return false;
	return Target->X11->IsMinimized();
}

void kui::systemWM::SetWindowMinSize(SysWindow* Target, Vec2ui MinSize)
{
	if (GetUseWayland())
		WAYLAND_FN(Target->Wayland->SetMinSize(MinSize));
	else
		Target->X11->SetMinSize(MinSize);
}

void kui::systemWM::SetWindowMaxSize(SysWindow* Target, Vec2ui MaxSize)
{
	if (GetUseWayland())
		WAYLAND_FN(Target->Wayland->SetMaxSize(MaxSize));
	else
		Target->X11->SetMaxSize(MaxSize);
}

void kui::systemWM::RestoreWindow(SysWindow* Target)
{
	if (GetUseWayland())
		WAYLAND_FN(Target->Wayland->RestoreWindow());
	else
		Target->X11->Restore();
}

void kui::systemWM::MinimizeWindow(SysWindow* Target)
{
	if (GetUseWayland())
		WAYLAND_FN(Target->Wayland->Minimize());
	else
		Target->X11->Minimize();
}

void kui::systemWM::MaximizeWindow(SysWindow* Target)
{
	if (GetUseWayland())
		WAYLAND_FN(Target->Wayland->Maximize());
	else
		Target->X11->Maximize();
}

static bool CommandExists(std::string Command)
{
	return system(("command -v " + Command + " > /dev/null").c_str()) == 0;
}

void kui::systemWM::SetWindowIcon(SysWindow* Target, uint8_t* Bytes, size_t Width, size_t Height)
{
	if (GetUseWayland())
		return; // TODO: xdg-toplevel-icon-v1
	Target->X11->SetIcon(Bytes, Width, Height);
}

void* kui::systemWM::GetPlatformHandle(SysWindow* Target)
{
	return reinterpret_cast<void*>(Target->X11->XWindow);
}

void kui::systemWM::WaitFrame(SysWindow* Target, float RemainingTime)
{
	std::this_thread::sleep_for(std::chrono::microseconds(int(RemainingTime * 1'000'000.0f)));
}

static std::string& SanitizeString(std::string& In)
{
	kui::strReplace::ReplaceChar(In, '\"', "\\\"");
	kui::strReplace::ReplaceChar(In, '\n', "\\n");
	kui::strReplace::ReplaceChar(In, '\\', "\\\\");
	return In;
}

void kui::systemWM::MessageBox(std::string Text, std::string Title, int Type)
{
	if (Type < 0 || Type > 2)
	{
		return;
	}

	SanitizeString(Text);
	SanitizeString(Title);

	if (CommandExists("kdialog"))
	{
		std::array<const char*, 3> Types = { "msgbox", "sorry", "error" };

		if (system(("/usr/bin/env kdialog --title \"" + Title + "\" --" + Types[Type] + " \"" + Text + "\"").c_str()) == 0)
			return;
	}

	if (CommandExists("zenity"))
	{
		std::array<const char*, 3> Types = { "info", "warning", "error" };

		if (system(("/usr/bin/env zenity --title \"" + Title + "\" --" + Types[Type] + " --text \"" + Text + "\"").c_str()) == 0)
			return;
	}

	// If kdialog and zenity don't exist, there's no good way of creating a message box.
	// TODO: Maybe create a KlemmUI window containing the message?
	// Making GUI apps for Linux is great!
	std::cerr << Title << ": " << Text << std::endl;
}

bool kui::systemWM::YesNoBox(std::string Text, std::string Title)
{
	SanitizeString(Text);
	SanitizeString(Title);

	if (CommandExists("kdialog"))
	{
		return system(("/usr/bin/env kdialog --title \"" + Title + "\" --yesno \"" + Text + "\"").c_str()) == 0;
	}

	if (CommandExists("zenity"))
	{
		return system(("/usr/bin/env zenity --title \"" + Title + "\" --question --text \"" + Text + "\"").c_str()) == 0;
	}

	return false;
}

std::string kui::systemWM::SelectFileDialog(bool PickFolders)
{
	std::string Command;

	if (CommandExists("kdialog"))
	{
		Command = PickFolders ? "kdialog --getexistingdirectory" : "kdialog --getopenfilename";
	}
	else
	{
		Command = "zenity --file-selection";

		if (PickFolders)
		{
			Command.append(" --directory");
		}
	}

	char FileName[4096];
	FILE* f = popen(Command.c_str(), "r");
	char* buf = fgets(FileName, 4096, f);
	if (!buf)
	{
		return "";
	}
	pclose(f);
	std::string Out = FileName;
	strReplace::ReplaceChar(Out, '\n', "");
	return Out;
}

#endif