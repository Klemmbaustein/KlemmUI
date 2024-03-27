#include <KlemmUI/Input.h>
#include <KlemmUI/Window.h>
#include <SDL.h>
#include <iostream>
using namespace KlemmUI;

Window* KlemmUI::InputManager::GetWindowBySDLID(uint32_t ID)
{
	std::vector<Window*> ActiveWindows = Window::GetActiveWindows();
	for (Window* i : ActiveWindows)
	{
		SDL_Window* SDLWindow = static_cast<SDL_Window*>(i->GetSDLWindowPtr());
		if (SDL_GetWindowID(SDLWindow) == ID)
		{
			return i;
		}
	}
	return nullptr;
}

void InputManager::Poll()
{
	SDL_Event Event;

	while (SDL_PollEvent(&Event))
	{
		switch (Event.type)
		{
		case SDL_KEYDOWN:
			SetKeyDown(static_cast<KlemmUI::Key>(Event.key.keysym.sym), true);
			break;
		case SDL_KEYUP:
			SetKeyDown(static_cast<KlemmUI::Key>(Event.key.keysym.sym), false);
			break;
		case SDL_WINDOWEVENT:
			switch (Event.window.event)
			{
			case SDL_WINDOWEVENT_CLOSE:
				GetWindowBySDLID(Event.window.windowID)->Close();
				break;
			case SDL_WINDOWEVENT_RESIZED:
				GetWindowBySDLID(Event.window.windowID)->OnResized();
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}

	return;
}

bool InputManager::IsKeyDown(Key PressedKey)
{
	auto Key = PressedKeys.find(PressedKey);

	if (Key == PressedKeys.end())
	{
		return false;
	}
	return Key->second;
}

void InputManager::SetKeyDown(Key PressedKey, bool KeyDown)
{
	auto Key = PressedKeys.find(PressedKey);

	if (Key == PressedKeys.end())
	{
		PressedKeys.insert(std::pair(PressedKey, KeyDown));
	}
	else
	{
		Key->second = KeyDown;
	}
}

void InputManager::RegisterOnKeyDownCallback(Key PressedKey, void(*Callback)())
{
}

std::string TextInput::GetSelectedTextString()
{
	return std::string();
}

void KlemmUI::TextInput::SetTextIndex(int NewIndex, bool ClearSelection)
{
}
