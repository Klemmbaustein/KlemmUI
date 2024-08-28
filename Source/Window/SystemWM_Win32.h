#if _WIN32
#pragma once
#include <KlemmUI/Window.h>
#define NOMINMAX
#include <Windows.h>

namespace KlemmUI::SystemWM
{
	class SysWindow
	{
	public:
		HWND WindowHandle = nullptr;
		HGLRC GLContext = nullptr;
		HDC DeviceContext = nullptr;
		Window* Parent = nullptr;

		bool Borderless = false;
		bool Resizable = false;
		bool Popup = false;
		int ActiveCursor = -1;

		Vector2ui Size;
		Vector2ui MinSize = 1;
		Vector2ui MaxSize = INT32_MAX;

		void SetSize(Vector2ui NewSize) const;

		std::string TextInput;

		void MakeContextActive() const;
	};
}
#endif