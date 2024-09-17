#pragma once
#include <KlemmUI/Window.h>

namespace KlemmUI::SystemWM
{
	class SysWindow;

	SysWindow* NewWindow(Window* Parent, Vector2ui Size, Vector2ui Pos, std::string Title, Window::WindowFlag Flags);
	void DestroyWindow(SysWindow* Target);

	void SwapWindow(SysWindow* Target);
	void ActivateContext(SysWindow* Target);

	Vector2ui GetWindowSize(SysWindow* Target);
	void SetWindowSize(SysWindow* Target, Vector2ui Size);
	void SetWindowPosition(SysWindow* Target, Vector2ui NewPosition);

	void SetWindowMinSize(SysWindow* Target, Vector2ui MinSize);
	void SetWindowMaxSize(SysWindow* Target, Vector2ui MaxSize);

	void UpdateWindow(SysWindow* Target);
	bool WindowHasFocus(SysWindow* Target);

	Vector2i GetCursorPosition(SysWindow* Target);
	Vector2ui GetScreenSize();

	std::string GetTextInput(SysWindow* Target);

	uint32_t GetDesiredRefreshRate(SysWindow* From);

	void SetWindowCursor(SysWindow* Target, Window::Cursor NewCursor);

	float GetDPIScale(SysWindow* Target);

	void SetTitle(SysWindow* Target, std::string Text);

	void SetClipboardText(std::string NewText);
	std::string GetClipboardText();

	bool IsLMBDown();
	bool IsRMBDown();
	bool IsWindowFullScreen(SysWindow* Target);
	bool IsWindowMinimized(SysWindow* Target);

	void MaximizeWindow(SysWindow* Target);
	void MinimizeWindow(SysWindow* Target);
	void HideWindow(SysWindow* Target);
	void RestoreWindow(SysWindow* Target);

	void MessageBox(std::string Text, std::string Title, int Type);
}