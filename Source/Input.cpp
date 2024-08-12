#include <KlemmUI/Input.h>
#include <KlemmUI/Window.h>
#include <iostream>
#include <KlemmUI/Rendering/ScrollObject.h>
#include "Window/SystemWM.h"
using namespace KlemmUI;

Window* KlemmUI::InputManager::GetWindowByPtr(void* Ptr)
{
	std::vector<Window*> ActiveWindows = Window::GetActiveWindows();
	for (Window* i : ActiveWindows)
	{
		if (i->GetSysWindow() == Ptr)
		{
			return i;
		}
	}
	return nullptr;
}

void KlemmUI::InputManager::MoveTextIndex(int Amount, bool RespectShiftPress)
{
	TextIndex = std::max(std::min(TextIndex + Amount, (int)Text.size()), 0);
	if ((!IsKeyDown(Key::LSHIFT) && !IsKeyDown(Key::RSHIFT)) || !RespectShiftPress)
	{
		TextSelectionStart = TextIndex;
	}
}

KlemmUI::InputManager::InputManager(Window* Parent)
{
	ParentWindow = Parent;
	
	// Text input callbacks
	RegisterOnKeyDownCallback(Key::LEFT, [](Window* Win) {
		Win->Input.MoveTextIndex(-1);
		if (Win->Input.Text[Win->Input.TextIndex] & 0b10000000)
		{
			Win->Input.MoveTextIndex(-1);
		}
		});

	RegisterOnKeyDownCallback(Key::RIGHT, [](Window* Win) {
		Win->Input.MoveTextIndex(1);
		if (Win->Input.Text[Win->Input.TextIndex] & 0b10000000)
		{
			Win->Input.MoveTextIndex(1);
		}
		});

	RegisterOnKeyDownCallback(Key::DELETE, [](Window* Win) {
		InputManager& In = Win->Input;
		
		if (In.PollForText && In.TextIndex < In.Text.size() && In.TextIndex >= 0)
		{
			if (In.TextSelectionStart == In.TextIndex)
			{
				do
				{
					In.Text.erase(In.TextIndex, 1);
				} while (In.Text.size() && (In.Text[In.TextIndex] & char(0xC0)) == char(0x80));
			}
			else
			{
				In.DeleteTextSelection();
			}
			In.SetTextIndex(std::max(std::min(In.TextIndex, (int)In.Text.size()), 0), true);
		}
		});

	RegisterOnKeyDownCallback(Key::BACKSPACE, [](Window* Win) {
		InputManager& In = Win->Input;
		if (In.PollForText && In.Text.size() > 0)
		{
			if (In.TextIndex > 0 || In.TextSelectionStart > 0)
			{
				if (In.TextSelectionStart == In.TextIndex)
				{
					char c = 0;
					do
					{
						In.TextIndex--;
						In.Text.erase(In.TextIndex, 1);
						if (In.TextIndex == 0)
						{
							break;
						}

						c = In.Text[In.TextIndex - 1];
					} while (In.Text.size() && (c & char(0xC0)) != char(0x80) && c <= 0);
				}
				else
				{
					In.DeleteTextSelection();
				}
			}
			In.SetTextIndex(std::max(std::min(In.TextIndex, (int)In.Text.size()), 0), true);
		}
		});

	RegisterOnKeyDownCallback(Key::ESCAPE, [](Window* Win) {
		Win->Input.PollForText = false;
		});

	RegisterOnKeyDownCallback(Key::RETURN, [](Window* Win) {
		Win->Input.PollForText = false;
		});

	RegisterOnKeyDownCallback(Key::c, [](Window* Win) {
		if (Win->Input.IsKeyDown(Key::LCTRL) || Win->Input.IsKeyDown(Key::RCTRL))
		{
			SystemWM::SetClipboardText(Win->Input.GetSelectedTextString());
		}
		});

	RegisterOnKeyDownCallback(Key::x, [](Window* Win) {
		if (Win->Input.IsKeyDown(Key::LCTRL) || Win->Input.IsKeyDown(Key::RCTRL))
		{
			SystemWM::SetClipboardText(Win->Input.GetSelectedTextString());
			Win->Input.DeleteTextSelection();
		}
		});

	RegisterOnKeyDownCallback(Key::v, [](Window* Win) {
		if (Win->Input.IsKeyDown(Key::LCTRL) || Win->Input.IsKeyDown(Key::RCTRL))
			Win->Input.AddTextInput(SystemWM::GetClipboardText());
		});

}

void KlemmUI::InputManager::UpdateCursorPosition()
{
	SystemWM::SysWindow* SysWindow = static_cast<SystemWM::SysWindow*>(ParentWindow->GetSysWindow());

	if (!ParentWindow->HasFocus())
	{
		ParentWindow->Input.MousePosition = 99;
		return;
	}

	Vector2ui Size = ParentWindow->GetSize();
	Vector2ui Pos = SystemWM::GetCursorPosition(SysWindow);

	ParentWindow->Input.MousePosition = Vector2(((float)Pos.X / (float)Size.X - 0.5f) * 2.0f, 1.0f - ((float)Pos.Y / (float)Size.Y * 2.0f));
}

void InputManager::Poll()
{
	MoveMouseWheel(ScrollAmount);
	ScrollAmount = 0;
	IsLMBClicked = false;
	IsRMBClicked = false;

	SystemWM::SysWindow* SysWindow = static_cast<SystemWM::SysWindow*>(ParentWindow->GetSysWindow());

	bool NewLMBDown = SystemWM::IsLMBDown();
	bool NewRMBDown = SystemWM::IsRMBDown();
	if (!IsLMBDown && NewLMBDown)
	{
		IsLMBClicked = true;
	}
	if (!IsRMBDown && NewRMBDown)
	{
		IsRMBClicked = true;
	}

	IsLMBDown = NewLMBDown;
	IsRMBDown = NewRMBDown;

	AddTextInput(SystemWM::GetTextInput(SysWindow));
}

void KlemmUI::InputManager::MoveMouseWheel(int Amount)
{
	while (Amount)
	{
		for (ScrollObject* s : ScrollObject::GetAllScrollObjects())
		{
			if (Amount < 0)
				s->ScrollUp();
			else
				s->ScrollDown();
		}
		if (Amount < 0)
			Amount++;
		else
			Amount--;
	}

}

void KlemmUI::InputManager::AddTextInput(std::string Str)
{
	if (Str.empty())
	{
		return;
	}
	if (PollForText)
	{
		if (Text.size() < TextIndex)
		{
			TextIndex = (int)Text.size();
		}
		DeleteTextSelection();
		Text.insert(TextIndex, Str);
		MoveTextIndex((int)Str.size(), false);
	}
}

void KlemmUI::InputManager::DeleteTextSelection()
{
	int Difference = std::abs(TextSelectionStart - TextIndex);
	Text.erase(std::min(TextIndex, TextSelectionStart), Difference);
	SetTextIndex(std::min(TextIndex, TextSelectionStart), true);
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
			Function(ParentWindow);
		}
	}
}

Vector2ui KlemmUI::InputManager::GetMouseScreenPosition()
{
	return 0;
}

void InputManager::RegisterOnKeyDownCallback(Key PressedKey, void(*Callback)(Window*))
{
	if (!ButtonPressedCallbacks.contains(PressedKey))
	{
		ButtonPressedCallbacks.insert(std::pair<Key, std::vector<void(*)(Window*)>>(PressedKey, { Callback }));
	}
	else
	{
		ButtonPressedCallbacks[PressedKey].push_back(Callback);
	}
}

void KlemmUI::InputManager::RemoveOnKeyDownCallback(Key PressedKey, void(*Callback)(Window*))
{
	if (ButtonPressedCallbacks.contains(PressedKey))
	{
		auto& Keys = ButtonPressedCallbacks[PressedKey];

		for (size_t i = 0; i < Keys.size(); i++)
		{
			if (Keys[i] == Callback)
			{
				Keys.erase(Keys.begin() + i);
				return;
			}
		}
	}
}

std::string KlemmUI::InputManager::GetSelectedTextString() const
{
	int Start = std::min(TextIndex, TextSelectionStart), End = std::max(TextIndex, TextSelectionStart);
	return Text.substr(Start, End - Start);
}

void KlemmUI::InputManager::SetTextIndex(int NewIndex, bool ClearSelection)
{
	TextIndex = NewIndex;
	if (ClearSelection)
	{
		TextSelectionStart = TextIndex;
	}
}
