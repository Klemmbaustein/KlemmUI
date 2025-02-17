#include <kui/Window.h>

#include "Internal/Internal.h"
#include "SystemWM/SystemWM.h"
#include <kui/UI/UIButton.h>
#include <kui/UI/UIScrollBox.h>
#include <kui/UI/UITextField.h>
#include <kui/Image.h>
#include <kui/Platform.h>
#include <mutex>
#include <cstring>
#include <iostream>

#define SYS_WINDOW_PTR(x) systemWM::SysWindow* x = static_cast<systemWM::SysWindow*>(this->SysWindowPtr)

static thread_local kui::Window* ActiveWindow = nullptr;
static thread_local bool HasMainWindow = false;
#if __linux__
// VSync is enabled with OpenGL by default, and you have to load OpenGL-Implementation-specific
// extensions to disable it.
// This is done on windows, but not on linux.
// This variable makes the window not sleep to match the framerate if the window has been redrawn this frame.
static thread_local bool RedrawnWindow = false;
#endif

const kui::Vec2ui kui::Window::POSITION_CENTERED = Vec2ui(UINT64_MAX, UINT64_MAX);
const kui::Vec2ui kui::Window::SIZE_DEFAULT = Vec2ui(UINT64_MAX, UINT64_MAX);
std::vector<kui::Window*> kui::Window::ActiveWindows;

void kui::Window::UpdateSize()
{
	SYS_WINDOW_PTR(SysWindow);
	WindowSize = systemWM::GetWindowSize(SysWindow);
}

std::mutex WindowMutex;

void* kui::Window::GetPlatformHandle() const
{
	SYS_WINDOW_PTR(SysWindow);
	return systemWM::GetPlatformHandle(SysWindow);
}

kui::Window::Window(std::string Name, WindowFlag Flags, Vec2ui WindowPos, Vec2ui WindowSize)
{
	if (WindowSize == SIZE_DEFAULT)
	{
#ifdef KLEMMUI_WEB_BUILD
		WindowSize = systemWM::GetScreenSize();
#else
		WindowSize = Vec2f(systemWM::GetScreenSize()) * 0.6f;
#endif
	}

	if (WindowPos == POSITION_CENTERED)
	{
		WindowPos = systemWM::GetScreenSize() / 2 - WindowSize / 2;
	}

	IgnoreDPI = (Flags & WindowFlag::IgnoreDPI) == WindowFlag::IgnoreDPI;
	std::unique_lock Guard = std::unique_lock(internal::WindowCreationMutex);

	SysWindowPtr = systemWM::NewWindow(this,
		WindowSize,
		WindowPos,
		Name,
		Flags);
	CurrentWindowFlags = Flags;
	DPI = IgnoreDPI ? 1.0f : systemWM::GetDPIScale(static_cast<systemWM::SysWindow*>(SysWindowPtr));


	UpdateSize();
	SetWindowActive();
	internal::InitGLContext(this);
	UI.InitUI();

	ActiveWindows.push_back(this);
}

kui::Window::~Window()
{
	std::unique_lock Guard = std::unique_lock(internal::WindowCreationMutex);

	SYS_WINDOW_PTR(SysWindow);
	systemWM::DestroyWindow(SysWindow);

	Markup.TranslationChangedCallbacks.clear();

	for (size_t i = 0; i < ActiveWindows.size(); i++)
	{
		if (ActiveWindows[i] == this)
		{
			ActiveWindows.erase(ActiveWindows.begin() + i);
			break;
		}
	}

	if (ActiveWindow == this)
	{
		ActiveWindow = nullptr;
	}

	if (IsMainWindow)
	{
		HasMainWindow = false;
	}

}

kui::Window* kui::Window::GetActiveWindow()
{
	return ActiveWindow;
}

void kui::Window::WaitFrame()
{
	SYS_WINDOW_PTR(SysWindow);

	uint32_t FPS = TargetFPS;

	if (FPS == 0)
	{
		FPS = systemWM::GetDesiredRefreshRate(SysWindow);

		// FPS might be 0 if the refresh rate is unknown.
		if (FPS == 0)
		{
			FPS = 60;
		}
	}

#if __linux__
	if (!RedrawnWindow && !platform::linux::GetUseWayland())
#endif
	{
#if __linux__
		float DesiredDelta = 0.5f / (float)FPS;
#else
		float DesiredDelta = 1.0f / (float)FPS;
#endif
		float TimeToWait = std::max(DesiredDelta - WindowDeltaTimer.Get(), 0.0f);
		systemWM::WaitFrame(SysWindow, TimeToWait);
	}
#if __linux__
	RedrawnWindow = false;
#endif
}

void kui::Window::RedrawInternal()
{
	if (!SysWindowPtr)
	{
		return;
	}

	if (ShouldUpdateSize)
	{
		UpdateSize();
		UI.ForceUpdateUI();
		ShouldUpdateSize = false;
		if (OnResizedCallback)
		{
			OnResizedCallback(this);
		}
	}

	if (UI.DrawElements())
	{
#if __linux__
		RedrawnWindow = true;
#endif
		internal::DrawWindow(this);
	}
}

void kui::Window::SetIconFile(std::string IconFilePath)
{
	SYS_WINDOW_PTR(SysWindow);

	size_t w, h;
	uint8_t* Image = image::LoadImageBytes(IconFilePath, w, h, true);
	systemWM::SetWindowIcon(SysWindow, Image, w, h);
	image::FreeImageBytes(Image);
}

void* kui::Window::GetSysWindow() const
{
	return SysWindowPtr;
}

float kui::Window::GetAspectRatio() const
{
	return (float)WindowSize.X / (float)WindowSize.Y;
}

bool kui::Window::HasFocus()
{
	SYS_WINDOW_PTR(SysWindow);
	return systemWM::WindowHasFocus(SysWindow);
}

kui::Vec2ui kui::Window::GetSize() const
{
	return WindowSize;
}

void kui::Window::SetSize(Vec2ui NewSize)
{
	SYS_WINDOW_PTR(SysWindow);
	systemWM::SetWindowSize(SysWindow, NewSize);
}

void kui::Window::SetPosition(Vec2ui Pos)
{
	SYS_WINDOW_PTR(SysWindow);
	systemWM::SetWindowPosition(SysWindow, Pos);
}

void kui::Window::SetWindowFlags(WindowFlag NewFlags)
{
	SYS_WINDOW_PTR(SysWindow);

	CurrentWindowFlags = NewFlags;

	systemWM::UpdateWindowFlags(SysWindow, NewFlags);
}

kui::Window::WindowFlag kui::Window::GetWindowFlags() const
{
	return CurrentWindowFlags;
}

void kui::Window::MakeContextCurrent()
{
	SYS_WINDOW_PTR(SysWindow);

	systemWM::ActivateContext(SysWindow);
}

void kui::Window::CancelClose()
{
	ShouldClose = false;
}

void kui::Window::SetTitle(std::string NewTitle)
{
	SYS_WINDOW_PTR(SysWindow);
	systemWM::SetTitle(SysWindow, NewTitle);
}

float kui::Window::GetDPI() const
{
	return DPI;
}

void kui::Window::UpdateDPI()
{
	SYS_WINDOW_PTR(SysWindow);

	float NewDPI = IgnoreDPI ? 0 : systemWM::GetDPIScale(SysWindow);

	if (NewDPI == 0)
	{
		NewDPI = 1.0f;
	}

	NewDPI *= DPIMultiplier;

	if (NewDPI != DPI && UI.UIElements.size())
	{
		// Resize the window so it still has the same visual scale.
		Window::SetSize(Vec2f(Window::GetSize()) * (NewDPI / DPI));
		UI.ForceUpdateUI();
		DPI = NewDPI;
	}
}

void kui::Window::HandleCursor()
{
	if (!HasFocus())
	{
		return;
	}
	SYS_WINDOW_PTR(SysWindow);

	systemWM::SetWindowCursor(SysWindow, CurrentCursor);

	if (UIScrollBox::IsDraggingScrollBox)
	{
		CurrentCursor = Cursor::Hand;
	}
	else
	{
		CurrentCursor = dynamic_cast<UIButton*>(UI.HoveredBox)
			? Cursor::Hand
			: (dynamic_cast<UITextField*>(UI.HoveredBox) ? Cursor::Text : Cursor::Default);
	}
}

bool kui::Window::UpdateWindow()
{
	SYS_WINDOW_PTR(SysWindow);

	if (!HasMainWindow)
	{
		HasMainWindow = true;
		IsMainWindow = true;
	}

	SetWindowActive();
	RedrawInternal();
	UI.UpdateEvents();
	HandleCursor();
	UpdateDPI();

	if (IsMainWindow)
	{
		WaitFrame();
	}
	FrameDelta = WindowDeltaTimer.Get();
	Time += FrameDelta;
	WindowDeltaTimer.Reset();

	Input.UpdateCursorPosition();
	systemWM::UpdateWindow(SysWindow);
	Input.Poll();

	return !ShouldClose;
}

void kui::Window::SetWindowActive()
{
	ActiveWindow = this;
	MakeContextCurrent();
}

void kui::Window::OnResized()
{
	ShouldUpdateSize = true;
}

void kui::Window::SetMinSize(Vec2ui MinimumSize)
{
	SYS_WINDOW_PTR(SysWindow);
	systemWM::SetWindowMinSize(SysWindow, MinimumSize);
	MinSize = MinimumSize;
}

void kui::Window::SetMaxSize(Vec2ui MaximumSize)
{
	SYS_WINDOW_PTR(SysWindow);
	systemWM::SetWindowMaxSize(SysWindow, MaximumSize);
	MaxSize = MaximumSize;
}

std::vector<kui::Window*> kui::Window::GetActiveWindows()
{
	std::unique_lock Guard = std::unique_lock(internal::WindowCreationMutex);
	return ActiveWindows;
}

void kui::Window::SetMaximized(bool NewIsFullScreen)
{
	SYS_WINDOW_PTR(SysWindow);

	if (NewIsFullScreen)
	{
		systemWM::MaximizeWindow(SysWindow);
	}
	else
	{
		systemWM::RestoreWindow(SysWindow);
	}
}

bool kui::Window::GetWindowFullScreen()
{
	SYS_WINDOW_PTR(SysWindow);
	return systemWM::IsWindowFullScreen(SysWindow);
}

bool kui::Window::GetMinimized()
{
	SYS_WINDOW_PTR(SysWindow);
	return systemWM::IsWindowMinimized(SysWindow);
}

void kui::Window::SetMinimized(bool NewIsMinimized)
{
	SYS_WINDOW_PTR(SysWindow);

	if (NewIsMinimized)
	{
		systemWM::MinimizeWindow(SysWindow);
	}
	else
	{
		systemWM::RestoreWindow(SysWindow);
	}
}

void kui::Window::Close()
{
	ShouldClose = true;
}

float kui::Window::GetDeltaTime() const
{
	return FrameDelta;
}

kui::Window::WindowFlag kui::operator|(Window::WindowFlag a, Window::WindowFlag b)
{
	return Window::WindowFlag((int)a | (int)b);
}

kui::Window::WindowFlag kui::operator&(Window::WindowFlag a, Window::WindowFlag b)
{
	return Window::WindowFlag((int)a & (int)b);
}