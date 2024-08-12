#pragma once
#include <KlemmUI/Window.h>

namespace KlemmUI::SystemWM
{
	class SysWindow;

	SysWindow* NewWindow(Window* Parent, Vector2ui Size, Vector2ui Pos, std::string Title, bool Borderless, bool Resizable, bool Popup);
	void DestroyWindow(SysWindow* Target);

	void SwapWindow(SysWindow* Target);
	void ActivateContext(SysWindow* Target);

	Vector2ui GetWindowSize(SysWindow* Target);

	void UpdateWindow(SysWindow* Target);
	bool WindowHasFocus(SysWindow* Target);

	Vector2ui GetCursorPosition(SysWindow* Target);
	Vector2ui GetScreenSize();

	std::string GetTextInput(SysWindow* Target);

	uint32_t GetDesiredRefreshRate(SysWindow* From);

	void SetWindowCursor(SysWindow* Target, Window::Cursor NewCursor);

	float GetDPIScale(SysWindow* Target);

	void SetClipboardText(std::string NewText);
	std::string GetClipboardText();

	bool IsLMBDown();
	bool IsRMBDown();
}