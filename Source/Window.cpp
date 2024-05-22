#include <KlemmUI/Window.h>
#include <SDL.h>

#include "Internal.h"
#include <KlemmUI/UI/UIBox.h>
#include <KlemmUI/Application.h>
#include <KlemmUI/UI/UIButton.h>
#include <KlemmUI/UI/UITextField.h>
#include <mutex>
#include <cstring>

#define SDL_WINDOW_PTR(x) SDL_Window* x = static_cast<SDL_Window*>(this->SDLWindowPtr)

static thread_local KlemmUI::Window* ActiveWindow = nullptr;
static thread_local bool HasMainWindow = false;

const Vector2ui KlemmUI::Window::POSITION_CENTERED = Vector2ui(UINT64_MAX, UINT64_MAX);
const Vector2ui KlemmUI::Window::SIZE_DEFAULT = Vector2ui(UINT64_MAX, UINT64_MAX);
std::vector<KlemmUI::Window*> KlemmUI::Window::ActiveWindows;

static const int MOUSE_GRAB_PADDING = 8;

static SDL_HitTestResult WindowHitTest(SDL_Window* SDLWindow, const SDL_Point* Area, void* Data)
{
	int Width, Height;
	SDL_GetWindowSize(SDLWindow, &Width, &Height);

	KlemmUI::Window* HitTestWindow = static_cast<KlemmUI::Window*>(Data);

	HitTestWindow->Input.MousePosition = Vector2(((float)Area->x / (float)Width - 0.5f) * 2.0f, 1.0f - ((float)Area->y / (float)Height * 2.0f));

	if (bool(HitTestWindow->GetWindowFlags() & KlemmUI::Window::WindowFlag::Resizable)
		&& bool(HitTestWindow->GetWindowFlags() & KlemmUI::Window::WindowFlag::Borderless))
	{
		if (Area->y < MOUSE_GRAB_PADDING)
		{
			if (Area->x < MOUSE_GRAB_PADDING)
			{
				return SDL_HITTEST_RESIZE_TOPLEFT;
			}
			else if (Area->x > Width - MOUSE_GRAB_PADDING)
			{
				return SDL_HITTEST_RESIZE_TOPRIGHT;
			}
			else
			{
				return SDL_HITTEST_RESIZE_TOP;
			}
		}
		else if (Area->y > Height - MOUSE_GRAB_PADDING)
		{
			if (Area->x < MOUSE_GRAB_PADDING)
			{
				return SDL_HITTEST_RESIZE_BOTTOMLEFT;
			}
			else if (Area->x > Width - MOUSE_GRAB_PADDING)
			{
				return SDL_HITTEST_RESIZE_BOTTOMRIGHT;
			}
			else
			{
				return SDL_HITTEST_RESIZE_BOTTOM;
			}
		}
		else if (Area->x < MOUSE_GRAB_PADDING)
		{
			return SDL_HITTEST_RESIZE_LEFT;
		}
		else if (Area->x > Width - MOUSE_GRAB_PADDING)
		{
			return SDL_HITTEST_RESIZE_RIGHT;
		}
	}
	if (!HitTestWindow->UI.HoveredBox && HitTestWindow->IsAreaGrabbableCallback && HitTestWindow->IsAreaGrabbableCallback(HitTestWindow))
	{
		return SDL_HITTEST_DRAGGABLE;
	}
	return SDL_HITTEST_NORMAL;
}

void KlemmUI::Window::UpdateSize()
{
	SDL_WINDOW_PTR(SDLWindow);

	int w = 0, h = 0;

	SDL_GetWindowSize(SDLWindow, &w, &h);

	WindowSize = Vector2ui(w, h);
}

std::mutex WindowMutex;

KlemmUI::Window::Window(std::string Name, WindowFlag Flags, Vector2ui WindowPos, Vector2ui WindowSize)
{
	std::lock_guard Guard = std::lock_guard(WindowMutex);
	if (WindowPos == POSITION_CENTERED)
	{
		WindowPos = Vector2ui(SDL_WINDOWPOS_CENTERED);
	}

	const bool IsDefaultSize = WindowSize == SIZE_DEFAULT;

	if (IsDefaultSize)
	{
		SDL_DisplayMode Mode;
		SDL_GetDisplayMode(0, 0, &Mode);
		WindowSize = Vector2ui(uint64_t((float)Mode.w * 0.6f), uint64_t((float)Mode.h * 0.6f));
	}

	SDL_SetHint("SDL_BORDERLESS_WINDOWED_STYLE", "1");
	SDL_SetHint("SDL_BORDERLESS_RESIZABLE_STYLE", "1");
	CurrentWindowFlags = Flags;
	SDL_Window* SDLWindow = SDL_CreateWindow(Name.c_str(),
		WindowPos.X,
		WindowPos.Y,
		WindowSize.X,
		WindowSize.Y,
		ToSDLWindowFlags(Flags) | SDL_WINDOW_OPENGL);

	if ((Flags & WindowFlag::Borderless) == WindowFlag::Borderless)
	{
		SDL_SetWindowHitTest(SDLWindow, WindowHitTest, this);
	}
	else
	{
		SDL_SetWindowHitTest(SDLWindow, nullptr, this);
	}

	if (!SDLWindow)
	{
		KlemmUI::Application::Error::Error(SDL_GetError(), true);
	}
	SDLWindowPtr = SDLWindow;

	void* NewCursors[(int)Cursor::Cursor_End] =
	{
		SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW),
		SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND),
		SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM),
	};

	memcpy(Cursors, NewCursors, sizeof(NewCursors));

	UpdateDPI();
	if (!IsDefaultSize)
	{
		SDL_SetWindowSize(SDLWindow, int((float)WindowSize.X * GetDPI()), int((float)WindowSize.Y * GetDPI()));
	}

	UpdateSize();
	SetWindowActive();
	GLContext = Internal::InitGLContext(this);
	UI.InitUI();

	ActiveWindows.push_back(this);
}

KlemmUI::Window::~Window()
{
	std::lock_guard Guard = std::lock_guard(WindowMutex);
	SDL_WINDOW_PTR(SDLWindow);

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

	SDL_GL_DeleteContext(GLContext);
	SDL_DestroyWindow(SDLWindow);
}

KlemmUI::Window* KlemmUI::Window::GetActiveWindow()
{
	return ActiveWindow;
}

void KlemmUI::Window::WaitFrame()
{
	SDL_WINDOW_PTR(SDLWindow);

	uint32_t FPS = TargetFPS;

	if (FPS == 0)
	{
		SDL_DisplayMode Mode;
		SDL_GetWindowDisplayMode(SDLWindow, &Mode);
		FPS = Mode.refresh_rate;

		// Mode.refresh_rate might be 0 if the refresh rate is unknown.
		if (FPS == 0)
		{
			FPS = 60;
		}
	}

	float DesiredDelta = 1.0f / (float)FPS;
	float TimeToWait = std::max(DesiredDelta - WindowDeltaTimer.Get(), 0.0f);
	SDL_Delay((int)(TimeToWait * 1000.f));
}

void* KlemmUI::Window::GetSDLWindowPtr() const
{
	return SDLWindowPtr;
}

float KlemmUI::Window::GetAspectRatio() const
{
	return (float)WindowSize.X / (float)WindowSize.Y;
}

bool KlemmUI::Window::HasFocus()
{
	return SDL_GetMouseFocus() == SDLWindowPtr;
}

Vector2ui KlemmUI::Window::GetSize() const
{
	return WindowSize;
}

void KlemmUI::Window::SetPosition(Vector2ui Pos)
{
	SDL_WINDOW_PTR(SDLWindow);
	SDL_SetWindowPosition(SDLWindow, Pos.X, Pos.Y);
}

void KlemmUI::Window::SetWindowFlags(WindowFlag NewFlags)
{
	SDL_WINDOW_PTR(SDLWindow);

	CurrentWindowFlags = NewFlags;

	SDL_SetWindowBordered(SDLWindow, SDL_bool((NewFlags & WindowFlag::Borderless) != WindowFlag::Borderless));
	if ((NewFlags & WindowFlag::Borderless) == WindowFlag::Borderless)
	{
		SDL_SetWindowHitTest(SDLWindow, WindowHitTest, this);
	}
	else
	{
		SDL_SetWindowHitTest(SDLWindow, nullptr, this);
	}

	SDL_SetWindowAlwaysOnTop(SDLWindow, SDL_bool((NewFlags & WindowFlag::AlwaysOnTop) == WindowFlag::AlwaysOnTop));
	SDL_SetWindowResizable(SDLWindow, SDL_bool((NewFlags & WindowFlag::Resizable) == WindowFlag::Resizable));
}

KlemmUI::Window::WindowFlag KlemmUI::Window::GetWindowFlags() const
{
	return CurrentWindowFlags;
}

void KlemmUI::Window::MakeContextCurrent()
{
	SDL_WINDOW_PTR(SDLWindow);

	if (!GLContext)
	{
		return;
	}

	if (SDL_GL_MakeCurrent(SDLWindow, GLContext))
	{
		Application::Error::Error(SDL_GetError());
	}
}

void KlemmUI::Window::CancelClose()
{
	ShouldClose = false;
}

void KlemmUI::Window::SetTitle(std::string NewTitle)
{
	SDL_WINDOW_PTR(SDLWindow);

	SDL_SetWindowTitle(SDLWindow, NewTitle.c_str());
}

float KlemmUI::Window::GetDPI() const
{
	return DPI;
}

void KlemmUI::Window::UpdateDPI()
{
	SDL_WINDOW_PTR(SDLWindow);

	float ddpi, hdpi, vdpi;

	SDL_GetDisplayDPI(SDL_GetWindowDisplayIndex(SDLWindow), &ddpi, &hdpi, &vdpi);

	if (hdpi == 0)
	{
		hdpi = 96.0f;
	}

	hdpi /= 96;
	hdpi *= DPIMultiplier;

	if (hdpi != DPI && UI.UIElements.size())
	{
		UI.ForceUpdateUI();
	}
	DPI = hdpi;
}

void KlemmUI::Window::HandleCursor()
{
	SDL_SetCursor(static_cast<SDL_Cursor*>(Cursors[(int)CurrentCursor]));
	CurrentCursor = dynamic_cast<UIButton*>(UI.HoveredBox) ? Cursor::Hand : (dynamic_cast<UITextField*>(UI.HoveredBox) ? Cursor::Text : Cursor::Default);
}

int KlemmUI::Window::ToSDLWindowFlags(WindowFlag Flags)
{
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
	if ((Flags & WindowFlag::Tooltip) == WindowFlag::Tooltip)
	{
		SDLFlags |= SDL_WINDOW_TOOLTIP | SDL_WINDOW_SKIP_TASKBAR;
	}
	return SDLFlags;
}

bool KlemmUI::Window::UpdateWindow()
{
	if (!HasMainWindow)
	{
		HasMainWindow = true;
		IsMainWindow = true;
	}

	SetWindowActive();
	MakeContextCurrent();
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
		Internal::DrawWindow(this);
	}
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
	SDL_WINDOW_PTR(SDLWindow);
	SDL_SetWindowMinimumSize(SDLWindow, int((float)MinimumSize.X * GetDPI()), int((float)MinimumSize.Y * GetDPI()));
}

void KlemmUI::Window::SetMaxSize(Vector2ui MaximumSize)
{
	SDL_WINDOW_PTR(SDLWindow);
	SDL_SetWindowMaximumSize(SDLWindow, MaximumSize.X, MaximumSize.Y);
}

std::vector<KlemmUI::Window*> KlemmUI::Window::GetActiveWindows()
{
	std::lock_guard Guard = std::lock_guard(WindowMutex);
	return ActiveWindows;
}

void KlemmUI::Window::SetWindowFullScreen(bool NewIsFullScreen)
{
	SDL_WINDOW_PTR(SDLWindow);

	if (NewIsFullScreen)
	{
		SDL_MaximizeWindow(SDLWindow);
	}
	else
	{
		SDL_RestoreWindow(SDLWindow);
	}
}

bool KlemmUI::Window::GetWindowFullScreen()
{
	SDL_WINDOW_PTR(SDLWindow);

	Uint32 Flags = SDL_GetWindowFlags(SDLWindow);

	return Flags & SDL_WINDOW_MAXIMIZED;
}

bool KlemmUI::Window::GetMinimized()
{
	SDL_WINDOW_PTR(SDLWindow);
	return SDL_GetWindowFlags(SDLWindow) & SDL_WINDOW_MINIMIZED;
}

void KlemmUI::Window::SetMinimized(bool NewIsMinimized)
{
	SDL_WINDOW_PTR(SDLWindow);

	if (NewIsMinimized)
	{
		SDL_MinimizeWindow(SDLWindow);
	}
	else
	{
		SDL_RestoreWindow(SDLWindow);
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