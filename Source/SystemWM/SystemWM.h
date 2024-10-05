#pragma once
#include <kui/Window.h>

namespace kui::systemWM
{
	class SysWindow;

	SysWindow* NewWindow(Window* Parent, Vec2ui Size, Vec2ui Pos, std::string Title, Window::WindowFlag Flags);
	void DestroyWindow(SysWindow* Target);

	void SwapWindow(SysWindow* Target);
	void ActivateContext(SysWindow* Target);

	Vec2ui GetWindowSize(SysWindow* Target);
	void SetWindowSize(SysWindow* Target, Vec2ui Size);
	void SetWindowPosition(SysWindow* Target, Vec2ui NewPosition);

	void SetWindowMinSize(SysWindow* Target, Vec2ui MinSize);
	void SetWindowMaxSize(SysWindow* Target, Vec2ui MaxSize);
	void WaitFrame(SysWindow* Target, float RemainingTime);

	void UpdateWindow(SysWindow* Target);
	bool WindowHasFocus(SysWindow* Target);

	Vec2i GetCursorPosition(SysWindow* Target);
	Vec2ui GetScreenSize();

	std::string GetTextInput(SysWindow* Target);

	uint32_t GetDesiredRefreshRate(SysWindow* From);

	void SetWindowCursor(SysWindow* Target, Window::Cursor NewCursor);

	float GetDPIScale(SysWindow* Target);

	void UpdateWindowFlags(SysWindow* Parent, Window::WindowFlag NewFlags);

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