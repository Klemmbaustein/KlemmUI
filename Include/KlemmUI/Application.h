#pragma once
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include <vector>

class UIBox;

namespace Application
{
	extern float AspectRatio;
	extern bool Quit;
	extern float DeltaTime;
	extern float Time;
	void SetApplicationTitle(std::string NewTitle);
	std::string GetApplicationTitle();
	Vector2ui GetWindowResolution();
	void SetWindowResolution(Vector2ui NewResolution);

	void SetShaderPath(std::string NewPath);
	const std::string& GetShaderPath();

	void SetErrorMessageCallback(void(*Callback)(std::string));
	void SetOnWindowResizedCallback(void(*Callback)());

	void Error(std::string Message);
	
	void UpdateWindow();

	bool GetWindowHasFocus();

	void SetFullScreen(bool NewFullScreen);
	bool GetFullScreen();

	void SetWindowMovableCallback(bool(*NewFunction)());

	void SetBorderlessWindowOutlineColor(Vector3f32 NewColor);

	void Minimize();

	void SetMinWindowSize(Vector2ui NewSize);

	void SetClipboard(std::string NewClipboardText);

	const std::string& GetNewTypedText();

	struct Timer
	{
		Timer();
		float TimeSinceCreation();
	private:
		uint64_t Time = 0;
	};

	struct ButtonEvent
	{
		ButtonEvent(void(*Function)(),
			void(*FunctionIndex)(int),
			UIBox* Btn,
			int Index = 0)
		{
			this->Function = Function;
			this->FunctionIndex = FunctionIndex;
			this->Btn = Btn;
			this->Index = Index;
		}
		void(*Function)() = nullptr;
		void(*FunctionIndex)(int) = nullptr;
		UIBox* Btn = nullptr;
		int Index = 0;
	};
	extern std::vector<ButtonEvent> ButtonEvents;

	enum MouseCursorType
	{
		CURSOR_NORMAL		= 0,
		CURSOR_HAND			= 1,
		CURSOR_CROSSHAIR	= 2,
		CURSOR_TEXT_HOVER	= 3
	};
	void SetActiveMouseCursor(MouseCursorType NewType);

	MouseCursorType GetMouseCursorFromHoveredButtons();

	enum InitializationFlags
	{
		MAXIMIZED_BIT		= 0b00000001,
		NO_RESIZE_BIT		= 0b00000010,
		BORDERLESS_BIT		= 0b00000100,
		ALWAYS_TOP_BIT		= 0b00001000,
	};

	// Initializes the graphics library. If DefaultResolution = {0, 0}, it will use 0.75x of the Desktop resolution.
	void Initialize(std::string WindowName, int Flags, Vector2ui DefaultResolution = Vector2ui());
	void SetWindowFlags(int Flags);
}