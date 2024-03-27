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


#define FOR_ALL_WINDOWS(Action) \
	for (auto& i : Window::GetActiveWindows()) \
	{ \
		i->Action; \
	} \

void InputManager::Poll()
{
	SDL_Event Event;

	while (SDL_PollEvent(&Event))
	{
		switch (Event.type)
		{
		case SDL_KEYDOWN:
			FOR_ALL_WINDOWS(Input.SetKeyDown(static_cast<KlemmUI::Key>(Event.key.keysym.sym), true));
			break;
		case SDL_KEYUP:
			FOR_ALL_WINDOWS(Input.SetKeyDown(static_cast<KlemmUI::Key>(Event.key.keysym.sym), false));
			break;
		case SDL_MOUSEBUTTONDOWN:
			switch (Event.button.button)
			{
			case SDL_BUTTON_LEFT:
				FOR_ALL_WINDOWS(Input.IsLMBDown = true);
				break;
			case SDL_BUTTON_RIGHT:
				FOR_ALL_WINDOWS(Input.IsRMBDown = true);
				break;
			default:
				break;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			switch (Event.button.button)
			{
			case SDL_BUTTON_LEFT:
				FOR_ALL_WINDOWS(Input.IsLMBDown = false);
				break;
			case SDL_BUTTON_RIGHT:
				FOR_ALL_WINDOWS(Input.IsRMBDown = false);
				break;
			default:
				break;
			}
			break;
		case SDL_WINDOWEVENT:
			switch (Event.window.event)
			{
			case SDL_WINDOWEVENT_CLOSE:
				Window::GetActiveWindow()->Close();
				break;
			case SDL_WINDOWEVENT_RESIZED:
				Window::GetActiveWindow()->OnResized();
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
	if (ButtonPressedCallbacks.contains(PressedKey) && KeyDown)
	{
		for (auto Function : ButtonPressedCallbacks[PressedKey])
		{
			Function();
		}
	}
}

void InputManager::RegisterOnKeyDownCallback(Key PressedKey, void(*Callback)())
{
	if (!ButtonPressedCallbacks.contains(PressedKey))
	{
		ButtonPressedCallbacks.insert(std::pair<Key, std::vector<void(*)()>>(PressedKey, { Callback }));
	}
	else
	{
		ButtonPressedCallbacks[PressedKey].push_back(Callback);
	}
}

std::string TextInput::GetSelectedTextString()
{
	return std::string();
}

void KlemmUI::TextInput::SetTextIndex(int NewIndex, bool ClearSelection)
{
}
