#include <KlemmUI/Window.h>
#include <SDL.h>

#include "Internal.h"
#include <KlemmUI/UI/UIBox.h>
#include <iostream>

#define SDL_WINDOW_PTR(x) SDL_Window* x = static_cast<SDL_Window*>(this->SDLWindowPtr)

static thread_local KlemmUI::Window* ActiveWindow = nullptr;

const Vector2ui KlemmUI::Window::POSITION_CENTERED = Vector2ui(UINT64_MAX, UINT64_MAX);
std::vector<KlemmUI::Window*> KlemmUI::Window::ActiveWindows;

void KlemmUI::Window::UpdateSize()
{
	SDL_Window* SDLWindow = static_cast<SDL_Window*>(this->GetSDLWindowPtr());

	int w = 0, h = 0;

	SDL_GetWindowSize(SDLWindow, &w, &h);

	WindowSize = Vector2ui(w, h);
}

KlemmUI::Window::Window(std::string Name, Vector2ui WindowPos, Vector2ui WindowSize)
{
	if (WindowPos == POSITION_CENTERED)
	{
		WindowPos = Vector2ui(SDL_WINDOWPOS_CENTERED);
	}

	Internal::InitSDL();

	SDL_Window* SDLWindow = SDL_CreateWindow(Name.c_str(), WindowPos.X, WindowPos.Y, WindowSize.X, WindowSize.Y, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	SDLWindowPtr = SDLWindow;

	UpdateSize();
	SetAsActiveWindow();

	Internal::InitGLContext(this);

	UIBox::InitUI();

	ActiveWindows.push_back(this);
}

KlemmUI::Window::~Window()
{
	if (ActiveWindow == this)
	{
		ClearActiveWindow();
	}
}

KlemmUI::Window* KlemmUI::Window::GetActiveWindow()
{
	return ActiveWindow;
}

void KlemmUI::Window::SetAsActiveWindow()
{
	ActiveWindow = this;
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

void KlemmUI::Window::ClearActiveWindow()
{
	ActiveWindow = nullptr;
}

void* KlemmUI::Window::GetSDLWindowPtr() const
{
	return SDLWindowPtr;
}

float KlemmUI::Window::GetAspectRatio() const
{
	return (float)WindowSize.X / (float)WindowSize.Y;
}

Vector2ui KlemmUI::Window::GetSize() const
{
	return WindowSize;
}

bool KlemmUI::Window::UpdateWindow()
{
	Input.Poll();

	if (ShouldUpdate)
	{
		UpdateSize();
		UIBox::ForceUpdateUI();
	}

	bool RedrawWindow = UIBox::DrawAllUIElements();
	if (RedrawWindow)
	{
		Internal::DrawWindow(this);
	}

	WaitFrame();

	FrameDelta = WindowDeltaTimer.Get();
	Time += FrameDelta;
	WindowDeltaTimer.Reset();

	bool ReturnShouldClose = ShouldClose;
	ShouldClose = false;
	return !ReturnShouldClose;
}

void KlemmUI::Window::OnResized()
{
	ShouldUpdate = true;
}

const std::vector<KlemmUI::Window*>& KlemmUI::Window::GetActiveWindows()
{
	return ActiveWindows;
}

void KlemmUI::Window::Close()
{
	ShouldClose = true;
}

float KlemmUI::Window::GetDeltaTime() const
{
	return FrameDelta;
}
