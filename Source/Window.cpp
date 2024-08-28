#include <KlemmUI/Window.h>

#include "Internal.h"
#include "Window/SystemWM.h"
#include <KlemmUI/UI/UIBox.h>
#include <KlemmUI/Application.h>
#include <KlemmUI/UI/UIButton.h>
#include <KlemmUI/UI/UITextField.h>
#include <KlemmUI/Image.h>
#include <mutex>
#include <cstring>
#include <iostream>
#include <thread>

#define SDL_WINDOW_PTR(x) SystemWM::SysWindow* x = static_cast<SystemWM::SysWindow*>(this->SysWindowPtr)

static thread_local KlemmUI::Window* ActiveWindow = nullptr;
static thread_local bool HasMainWindow = false;
#if __linux__
// VSync is enabled with OpenGL by default, and you have to load OpenGL-Implementation-specific
// extensions to disable it.
// This is done on windows, but not on linux.
// This variable makes the window not sleep to match the framerate if the window has been redrawn this frame.
static thread_local bool RedrawnWindow = false;
#endif

const Vector2ui KlemmUI::Window::POSITION_CENTERED = Vector2ui(UINT64_MAX, UINT64_MAX);
const Vector2ui KlemmUI::Window::SIZE_DEFAULT = Vector2ui(UINT64_MAX, UINT64_MAX);
std::vector<KlemmUI::Window*> KlemmUI::Window::ActiveWindows;

void KlemmUI::Window::UpdateSize()
{
	SDL_WINDOW_PTR(SysWindow);
	WindowSize = SystemWM::GetWindowSize(SysWindow);
}

std::mutex WindowMutex;

KlemmUI::Window::Window(std::string Name, WindowFlag Flags, Vector2ui WindowPos, Vector2ui WindowSize)
{
	if (WindowPos == POSITION_CENTERED)
	{
		WindowPos = SystemWM::GetScreenSize() / 2 - WindowSize / 2;
	}

	const bool IsDefaultSize = WindowSize == SIZE_DEFAULT;

	SysWindowPtr = SystemWM::NewWindow(this,
		WindowSize,
		WindowPos,
		Name,
		(Flags & WindowFlag::Borderless) == WindowFlag::Borderless,
		(Flags & WindowFlag::Resizable) == WindowFlag::Resizable,
		(Flags & WindowFlag::Popup) == WindowFlag::Popup
	);
	CurrentWindowFlags = Flags;

	std::lock_guard Guard = std::lock_guard(Internal::WindowCreationMutex);

	UpdateSize();
	SetWindowActive();
	Internal::InitGLContext(this);
	UI.InitUI();

	ActiveWindows.push_back(this);
}

KlemmUI::Window::~Window()
{
	std::lock_guard Guard = std::lock_guard(Internal::WindowCreationMutex);

	SDL_WINDOW_PTR(SysWindow);

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

	SystemWM::DestroyWindow(SysWindow);
}

KlemmUI::Window* KlemmUI::Window::GetActiveWindow()
{
	return ActiveWindow;
}

void KlemmUI::Window::WaitFrame()
{
	SDL_WINDOW_PTR(SysWindow);

	uint32_t FPS = TargetFPS;

	if (FPS == 0)
	{
		FPS = SystemWM::GetDesiredRefreshRate(SysWindow);

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
		float DesiredDelta = 1.0f / (float)FPS;
		float TimeToWait = std::max(DesiredDelta - WindowDeltaTimer.Get(), 0.0f);
		std::this_thread::sleep_for(std::chrono::milliseconds((int)(TimeToWait * 1000.f)));
	}
#if __linux__
	RedrawnWindow = false;
#endif
}

void KlemmUI::Window::RedrawInternal()
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
		Internal::DrawWindow(this);
	}
}

void KlemmUI::Window::SetIconFile(std::string IconFilePath)
{
#if 0
	SDL_WINDOW_PTR(GLWindow);
	
	size_t Width, Height;
	uint8_t* IconBytes = Image::LoadImageBytes(IconFilePath, Width, Height, true);

	// Limitation of SDL2.
	if (Width > 64 || Height > 64)
	{
		Application::Error::Error("Window icon too large. Maximum is 64x64 pixels.");
		return;
	}

	SDL_Surface* s = SDL_CreateRGBSurfaceWithFormatFrom(IconBytes, 64, 64, 1, (int)Width * 4, SDL_PIXELFORMAT_RGBA8888);

	SDL_SetWindowIcon(GLWindow, s);

	SDL_FreeSurface(s);
	Image::FreeImageBytes(IconBytes);
#endif
}

void* KlemmUI::Window::GetSysWindow() const
{
	return SysWindowPtr;
}

float KlemmUI::Window::GetAspectRatio() const
{
	return (float)WindowSize.X / (float)WindowSize.Y;
}

bool KlemmUI::Window::HasFocus()
{
	SDL_WINDOW_PTR(SysWindow);
	return SystemWM::WindowHasFocus(SysWindow);
}

Vector2ui KlemmUI::Window::GetSize() const
{
	return WindowSize;
}

void KlemmUI::Window::SetSize(Vector2ui NewSize)
{
	SDL_WINDOW_PTR(SysWindow);
	SystemWM::SetWindowSize(SysWindow, NewSize);
}

void KlemmUI::Window::SetPosition(Vector2ui Pos)
{
	SDL_WINDOW_PTR(SysWindow);
	SystemWM::SetWindowPosition(SysWindow, Pos);
}

void KlemmUI::Window::SetWindowFlags(WindowFlag NewFlags)
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

KlemmUI::Window::WindowFlag KlemmUI::Window::GetWindowFlags() const
{
	return CurrentWindowFlags;
}

void KlemmUI::Window::MakeContextCurrent()
{
	SDL_WINDOW_PTR(SysWindow);

	SystemWM::ActivateContext(SysWindow);
}

void KlemmUI::Window::CancelClose()
{
	ShouldClose = false;
}

void KlemmUI::Window::SetTitle(std::string NewTitle)
{
	SDL_WINDOW_PTR(SysWindow);
	SystemWM::SetTitle(SysWindow, NewTitle);
}

float KlemmUI::Window::GetDPI() const
{
	return DPI;
}

void KlemmUI::Window::UpdateDPI()
{
	SDL_WINDOW_PTR(SysWindow);

	float hdpi = SystemWM::GetDPIScale(SysWindow);

	if (hdpi == 0)
	{
		hdpi = 1.0f;
	}

	hdpi *= DPIMultiplier;

	if (hdpi != DPI && UI.UIElements.size())
	{
		UI.ForceUpdateUI();
	}
	DPI = hdpi;
}

void KlemmUI::Window::HandleCursor()
{
	if (!HasFocus())
	{
		return;
	}
	SDL_WINDOW_PTR(SysWindow);

	SystemWM::SetWindowCursor(SysWindow, CurrentCursor);
	CurrentCursor = dynamic_cast<UIButton*>(UI.HoveredBox) ? Cursor::Hand : (dynamic_cast<UITextField*>(UI.HoveredBox) ? Cursor::Text : Cursor::Default);
}

int KlemmUI::Window::ToSDLWindowFlags(WindowFlag Flags)
{
#if 0
	int SDLFlags = 0;

	if ((Flags & WindowFlag::Borderless) == WindowFlag::Borderless)
	{
		SDLFlags |= SDL_WINDOW_BORDERLESS;
	}
	if ((Flags & WindowFlag::AlwaysOnTop) == WindowFlag::AlwaysOnTop)
	{
		SDLFlags |= SDL_WINDOW_ALWAYS_ON_TOP;
	}
	if ((Flags & WindowFlag::FullScreen) == WindowFlag::FullScreen)
	{
		SDLFlags |= SDL_WINDOW_MAXIMIZED;
	}
	if ((Flags & WindowFlag::Resizable) == WindowFlag::Resizable)
	{
		SDLFlags |= SDL_WINDOW_RESIZABLE;
	}
	if ((Flags & WindowFlag::Popup) == WindowFlag::Popup)
	{
		SDLFlags |= SDL_WINDOW_TOOLTIP | SDL_WINDOW_SKIP_TASKBAR;
	}
	return SDLFlags;
#endif
	return 0;
}

bool KlemmUI::Window::UpdateWindow()
{
	SDL_WINDOW_PTR(SysWindow);

	if (!HasMainWindow)
	{
		HasMainWindow = true;
		IsMainWindow = true;
	}

	SetWindowActive();
	RedrawInternal();
	SystemWM::UpdateWindow(SysWindow);
	UI.UpdateEvents();
	HandleCursor();
	UpdateDPI();

	if (IsMainWindow)
	{
		WaitFrame();
	}
	Input.UpdateCursorPosition();
	Input.Poll();

	FrameDelta = WindowDeltaTimer.Get();
	Time += FrameDelta;
	WindowDeltaTimer.Reset();

	return !ShouldClose;
}

void KlemmUI::Window::SetWindowActive()
{
	ActiveWindow = this;
	MakeContextCurrent();
}

void KlemmUI::Window::OnResized()
{
	ShouldUpdateSize = true;
}

void KlemmUI::Window::SetMinSize(Vector2ui MinimumSize)
{
	SDL_WINDOW_PTR(SysWindow);
	SystemWM::SetWindowMinSize(SysWindow, MinimumSize);
	MinSize = MinimumSize;
}

void KlemmUI::Window::SetMaxSize(Vector2ui MaximumSize)
{
	SDL_WINDOW_PTR(SysWindow);
	SystemWM::SetWindowMaxSize(SysWindow, MaximumSize);
	MaxSize = MaximumSize;
}

std::vector<KlemmUI::Window*> KlemmUI::Window::GetActiveWindows()
{
	std::lock_guard Guard = std::lock_guard(WindowMutex);
	return ActiveWindows;
}

void KlemmUI::Window::SetMaximized(bool NewIsFullScreen)
{
	SDL_WINDOW_PTR(SysWindow);

	if (NewIsFullScreen)
	{
		SystemWM::MaximizeWindow(SysWindow);
	}
	else
	{
		SystemWM::RestoreWindow(SysWindow);
	}
}

bool KlemmUI::Window::GetWindowFullScreen()
{
	SDL_WINDOW_PTR(SysWindow);
	return SystemWM::IsWindowFullScreen(SysWindow);
}

bool KlemmUI::Window::GetMinimized()
{
//	SDL_WINDOW_PTR(SDLWindow);
	return false;
}

void KlemmUI::Window::SetMinimized(bool NewIsMinimized)
{
	SDL_WINDOW_PTR(SysWindow);

	if (NewIsMinimized)
	{
		SystemWM::MinimizeWindow(SysWindow);
	}
	else
	{
		SystemWM::RestoreWindow(SysWindow);
	}
}

void KlemmUI::Window::Close()
{
	ShouldClose = true;
}

float KlemmUI::Window::GetDeltaTime() const
{
	return FrameDelta;
}

KlemmUI::Window::WindowFlag KlemmUI::operator|(Window::WindowFlag a, Window::WindowFlag b)
{
	return Window::WindowFlag((int)a | (int)b);
}

KlemmUI::Window::WindowFlag KlemmUI::operator&(Window::WindowFlag a, Window::WindowFlag b)
{
	return Window::WindowFlag((int)a & (int)b);
}