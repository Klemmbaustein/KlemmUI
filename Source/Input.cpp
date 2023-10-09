#include "Input.h"
#include <map>
#include <vector>

namespace Input
{
	Vector2f MouseLocation = Vector2(-2);
	bool CursorVisible = false;
	bool Keys[351];

	std::map<int, std::vector<void(*)()>> ButtonPressedCallbacks;
}

bool Input::IsKeyDown(int Key)
{
	if (!(Key < 128))
	{
		Key -= 1073741755;
	}
	if (Key < 351 && Key >= 0)
		return Keys[Key];
	return false;
}

void Input::SetKeyDown(int Key, bool KeyDown)
{
	if (!(Key < 128))
	{
		Key -= 1073741755;
	}
	if (Key < 351 && Key >= 0)
		Keys[Key] = KeyDown;

	if (ButtonPressedCallbacks.contains(Key) && KeyDown)
	{
		for (auto Function : ButtonPressedCallbacks[Key])
		{
			Function();
		}
	}
}

void Input::RegisterOnKeyDownCallback(int Key, void(*Callback)())
{
	if (!(Key < 128))
	{
		Key -= 1073741755;
	}
	if (!ButtonPressedCallbacks.contains(Key))
	{
		ButtonPressedCallbacks.insert(std::pair<int, std::vector<void(*)()>>(Key, { Callback }));
	}
	else
	{
		ButtonPressedCallbacks[Key].push_back(Callback);
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
}