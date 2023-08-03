#pragma once
#define SDL_MAIN_HANDLED
#include "Math/Vector2.h"
#include <SDL.h>

namespace Input
{
	bool IsKeyDown(int Key);
	void SetKeyDown(int Key, bool KeyDown);

	extern Vector2f MouseMovement;
	extern bool IsLMBDown;
	extern bool IsRMBDown;
	extern bool CursorVisible;
	extern Vector2f MouseLocation;
}

namespace TextInput
{
	extern bool PollForText;
	extern std::string Text;
	extern int TextIndex;
	extern int TextRow;
	extern int ReturnPresses;
	extern int BackspacePresses;
	extern int DeletePresses;
	extern int NumPastes;
}