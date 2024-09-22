#include <kui/Window.h>

#include "Internal/Internal.h"
#include "SystemWM/SystemWM.h"
#include <kui/UI/UIBox.h>
#include <kui/App.h>
#include <kui/UI/UIButton.h>
#include <kui/UI/UITextField.h>
#include <kui/Image.h>
#include <mutex>
#include <cstring>
#include <chrono>
#include <thread>

#define SDL_WINDOW_PTR(x) systemWM::SysWindow* x = static_cast<systemWM::SysWindow*>(this->SysWindowPtr)

static thread_local kui::Window* ActiveWindow = nullptr;
static thread_local bool HasMainWindow = false;
#if __linux__
// VSync is enabled with OpenGL by default, and you have to load OpenGL-Implementation-specific
// extensions to disable it.
// This is done on windows, but not on linux.
// This variable makes the window not sleep to match the framerate if the window has been redrawn this frame.
static thread_local bool RedrawnWindow = false;
#endif

const Vec2ui kui::Window::POSITION_CENTERED = Vec2ui(UINT64_MAX, UINT64_MAX);
const Vec2ui kui::Window::SIZE_DEFAULT = Vec2ui(UINT64_MAX, UINT64_MAX);
std::vector<kui::Window*> kui::Window::ActiveWindows;

void kui::Window::UpdateSize()
{
	SDL_WINDOW_PTR(SysWindow);
	WindowSize = systemWM::GetWindowSize(SysWindow);
}

std::mutex WindowMutex;

kui::Window::Window(std::string Name, WindowFlag Flags, Vec2ui WindowPos, Vec2ui WindowSize)
{
	if (WindowSize == SIZE_DEFAULT)
	{
		WindowSize = Vec2f(systemWM::GetScreenSize()) * 0.6f;
	}

	if (WindowPos == POSITION_CENTERED)
	{
		WindowPos = systemWM::GetScreenSize() / 2 - WindowSize / 2;
	}

	IgnoreDPI = (Flags & WindowFlag::IgnoreDPI) == WindowFlag::IgnoreDPI;

	SysWindowPtr = systemWM::NewWindow(this,
		WindowSize,
		WindowPos,
		Name,
		Flags);
	CurrentWindowFlags = Flags;
	DPI = systemWM::GetDPIScale(static_cast<systemWM::SysWindow*>(SysWindowPtr));

	std::lock_guard Guard = std::lock_guard(internal::WindowCreationMutex);

	UpdateSize();
	SetWindowActive();
	internal::InitGLContext(this);
	UI.InitUI();

	ActiveWindows.push_back(this);
}

kui::Window::~Window()
{
	std::lock_guard Guard = std::lock_guard(internal::WindowCreationMutex);

	SDL_WINDOW_PTR(SysWindow);
	systemWM::DestroyWindow(SysWindow);

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
	SDL_WINDOW_PTR(SysWindow);

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
	if (!RedrawnWindow)
#endif
	{
#if __linux__
		float DesiredDelta = 0.5f / (float)FPS;
#else
		float DesiredDelta = 1.0f / (float)FPS;
#endif
		float TimeToWait = std::max(DesiredDelta - WindowDeltaTimer.Get(), 0.0f);
		std::this_thread::sleep_for(std::chrono::milliseconds(int(TimeToWait * 1000.f)));
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
#if 0
	SDL_WINDOW_PTR(GLWindow);

	size_t Width, Height;
	uint8_t* IconBytes = image::LoadImageBytes(IconFilePath, Width, Height, true);

	// Limitation of SDL2.
	if (Width > 64 || Height > 64)
	{
		app::error::Error("Window icon too large. Maximum is 64x64 pixels.");
		return;
	}

	SDL_Surface* s = SDL_CreateRGBSurfaceWithFormatFrom(IconBytes, 64, 64, 1, (int)Width * 4, SDL_PIXELFORMAT_RGBA8888);

	SDL_SetWindowIcon(GLWindow, s);

	SDL_FreeSurface(s);
	image::FreeImageBytes(IconBytes);
#endif
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
	SDL_WINDOW_PTR(SysWindow);
	return systemWM::WindowHasFocus(SysWindow);
}

Vec2ui kui::Window::GetSize() const
{
	return WindowSize;
}

void kui::Window::SetSize(Vec2ui NewSize)
{
	SDL_WINDOW_PTR(SysWindow);
	systemWM::SetWindowSize(SysWindow, NewSize);
}

void kui::Window::SetPosition(Vec2ui Pos)
{
	SDL_WINDOW_PTR(SysWindow);
	systemWM::SetWindowPosition(SysWindow, Pos);
}

void kui::Window::SetWindowFlags(WindowFlag NewFlags)
{
	/*
	SDL_WINDOW_PTR(GLWindow);

	CurrentWindowFlags = NewFlags;

	SDL_SetWindowBordered(GLWindow, SDL_bool((NewFlags & WindowFlag::Borderless) != WindowFlag::Borderless));
	if ((NewFlags & WindowFlag::Borderless) == WindowFlag::Borderless)
	{
		SDL_SetWindowHitTest(GLWindow, WindowHitTest, this);
	}
	else
	{
		SDL_SetWindowHitTest(GLWindow, nullptr, this);
	}

	SDL_SetWindowAlwaysOnTop(GLWindow, SDL_bool((NewFlags & WindowFlag::AlwaysOnTop) == WindowFlag::AlwaysOnTop));
	SDL_SetWindowResizable(GLWindow, SDL_bool((NewFlags & WindowFlag::Resizable) == WindowFlag::Resizable));
	*/
}

kui::Window::WindowFlag kui::Window::GetWindowFlags() const
{
	return CurrentWindowFlags;
}

void kui::Window::MakeContextCurrent()
{
	SDL_WINDOW_PTR(SysWindow);

	systemWM::ActivateContext(SysWindow);
}

void kui::Window::CancelClose()
{
	ShouldClose = false;
}

void kui::Window::SetTitle(std::string NewTitle)
{
	SDL_WINDOW_PTR(SysWindow);
	systemWM::SetTitle(SysWindow, NewTitle);
}

float kui::Window::GetDPI() const
{
	return DPI;
}

void kui::Window::UpdateDPI()
{
	SDL_WINDOW_PTR(SysWindow);

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
	SDL_WINDOW_PTR(SysWindow);

	systemWM::SetWindowCursor(SysWindow, CurrentCursor);
	CurrentCursor = dynamic_cast<UIButton*>(UI.HoveredBox)
		? Cursor::Hand
		: (dynamic_cast<UITextField*>(UI.HoveredBox) ? Cursor::Text : Cursor::Default);
}

bool kui::Window::UpdateWindow()
{
	SDL_WINDOW_PTR(SysWindow);

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
	Input.UpdateCursorPosition();
	systemWM::UpdateWindow(SysWindow);
	Input.Poll();

	FrameDelta = WindowDeltaTimer.Get();
	Time += FrameDelta;
	WindowDeltaTimer.Reset();

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
	SDL_WINDOW_PTR(SysWindow);
	systemWM::SetWindowMinSize(SysWindow, MinimumSize);
	MinSize = MinimumSize;
}

void kui::Window::SetMaxSize(Vec2ui MaximumSize)
{
	SDL_WINDOW_PTR(SysWindow);
	systemWM::SetWindowMaxSize(SysWindow, MaximumSize);
	MaxSize = MaximumSize;
}

std::vector<kui::Window*> kui::Window::GetActiveWindows()
{
	std::lock_guard Guard = std::lock_guard(WindowMutex);
	return ActiveWindows;
}

void kui::Window::SetMaximized(bool NewIsFullScreen)
{
	SDL_WINDOW_PTR(SysWindow);

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
	SDL_WINDOW_PTR(SysWindow);
	return systemWM::IsWindowFullScreen(SysWindow);
}

bool kui::Window::GetMinimized()
{
	SDL_WINDOW_PTR(SysWindow);
	return systemWM::IsWindowMinimized(SysWindow);
}

void kui::Window::SetMinimized(bool NewIsMinimized)
{
	SDL_WINDOW_PTR(SysWindow);

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