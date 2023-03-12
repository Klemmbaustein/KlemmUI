#include "Input.h"

namespace Input
{
	Vector2f MouseLocation = Vector2(-2);
	bool CursorVisible = false;
	bool Keys[351];
}

bool Input::IsKeyDown(int Key)
{
	if (!(Key < 128))
	{
		Key -= 1073741755;
	}
	if (Key < 351 && Key >= 0)
		return Input::Keys[Key];
	return false;
}

void Input::SetKeyDown(int Key, bool KeyDown)
{
	if (!(Key < 128))
	{
		Key -= 1073741755;
	}
	if (Key < 351 && Key >= 0)
		Input::Keys[Key] = KeyDown;
}

namespace Input
{
	bool IsLMBDown = false;
	bool IsRMBDown = false;
	Vector2f Input::MouseMovement;
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