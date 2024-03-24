#include "KlemmUI/Input.h"
#include <map>
#include <vector>

namespace Input
{
	Vector2f MouseLocation = Vector2(-2);
	bool CursorVisible = false;
	bool Keys[351];

	std::map<int, std::vector<void(*)()>> ButtonPressedCallbacks;
}

bool Input::IsKeyDown(Key key)
{
	int KeyVal = (int)key;
	if (!(KeyVal < 128))
	{
		KeyVal -= 1073741755;
	}
	return KeyVal < 351 && KeyVal >= 0 && Keys[KeyVal];
}

void Input::SetKeyDown(Key key, bool KeyDown)
{
	int KeyVal = (int)key;
	if (!(KeyVal < 128))
	{
		KeyVal -= 1073741755;
	}
	if (KeyVal < 351 && KeyVal >= 0)
		Keys[KeyVal] = KeyDown;

	if (ButtonPressedCallbacks.contains(KeyVal) && KeyDown)
	{
		for (auto Function : ButtonPressedCallbacks[KeyVal])
		{
			Function();
		}
	}
}

void Input::RegisterOnKeyDownCallback(Key key, void(*Callback)())
{
	int KeyVal = (int)key;
	if (!(KeyVal < 128))
	{
		KeyVal -= 1073741755;
	}
	if (!ButtonPressedCallbacks.contains(KeyVal))
	{
		ButtonPressedCallbacks.insert(std::pair<int, std::vector<void(*)()>>(KeyVal, { Callback }));
	}
	else
	{
		ButtonPressedCallbacks[KeyVal].push_back(Callback);
	}
}

namespace Input
{
	bool IsLMBDown = false;
	bool IsRMBDown = false;
	Vector2f MouseMovement;
}

namespace TextInput
{
	bool PollForText = false;
	std::string Text;
	int TextIndex = 0;
	int TextRow = 0;
	int ReturnPresses = 0;
	int BackspacePresses = 0;
	int DeletePresses = 0;
	int NumPastes = 0;
	int TextSelectionStart = 0;
	std::string GetSelectedTextString()
	{
		int Start = std::min(TextIndex, TextSelectionStart), End = std::max(TextIndex, TextSelectionStart);
		return Text.substr(Start, End - Start);
	}
	void SetTextIndex(int NewIndex, bool ClearSelection)
	{
		TextIndex = NewIndex;
		if (ClearSelection)
		{
			TextSelectionStart = TextIndex;
		}
	}
}