#if KLEMMUI_WITH_WAYLAND && !KLEMMUI_CUSTOM_SYSTEMWM
#pragma once
#include "SystemWM.h"
#include "SystemWM_WaylandClipboard.h"
#include <cstdint>
#include <EGL/egl.h>
#include <wayland-client.h>
#include <wayland-egl.h>
#include <libdecor-0/libdecor.h>
#include <wayland-cursor.h>
#include <xkbcommon/xkbcommon.h>
#include <kui/Timer.h>
#include <string>

namespace kui::systemWM
{
	class WaylandWindow;
	struct WaylandConnection;

	struct WaylandKeyboardInfo
	{
		xkb_context* KeyboardContext = nullptr;
		xkb_state* KeyboardState = nullptr;
		xkb_keymap* KeyboardMap = nullptr;
		wl_keyboard* WaylandKeyboard = nullptr;
		Timer RepeatTimer;
		uint32_t RepeatDelay = 500;
		uint32_t RepeatRate = 25;
		bool RepeatStarted = false;
		int RepeatSymbol = 0;
		int RepeatCode = 0;
		std::string RepeatedString;

		bool InitialDelayDone = false;

		std::string TextInput;

		void SetRepeated(std::string RepeatedString, int Code, int Symbol);
		void ClearRepeated();
		std::string UpdateRepeated(WaylandConnection* With);
	};

	struct WaylandCursorInfo
	{
		std::map<Window::Cursor, wl_cursor*> LoadedCursors;
		wl_cursor* CurrentCursorAnimation;
		wl_cursor_image* CurrentCursorImage;
		uint32_t AnimationFrame = 0;
		Timer AnimationTimer;
		wl_surface* CursorSurface = nullptr;
		wl_cursor_theme* CursorTheme = nullptr;
		wl_pointer* Pointer = nullptr;
	};

	struct WaylandConnection
	{
		static WaylandConnection* Current;

		Vec2ui PrimarySize = Vec2ui(1920, 1080);
		bool HasSize = false;

		wl_compositor* WaylandCompositor = nullptr;
		wl_display* WaylandDisplay = nullptr;
		wl_registry* WaylandRegistry = nullptr;
		wl_seat* WaylandSeat = nullptr;
		wl_shell* WaylandShell = nullptr;
		wl_surface* PointerFocus = nullptr;
		WaylandWindow* PointerWindow = nullptr;
		WaylandWindow* KeyboardWindow = nullptr;
		wl_shm* SharedMemory = nullptr;

		WaylandKeyboardInfo Keyboard;
		WaylandCursorInfo Cursor;
		WaylandClipboard Clipboard;

		Window::Cursor CurrentCursor = Window::Cursor::End;

		wl_cursor* CreateCursor(Window::Cursor New);

		uint32_t Serial = 0;
		uint32_t PointerScale = -1;
		int Scrolled = 0;
		bool IsLMBDown = false;
		bool IsRMBDown = false;

		WaylandConnection();

		void UpdateCursor();
		void NextCursorFrame();
		void UpdateCursorGraphics() const;
		void SetCursor(Window::Cursor New);

		static WaylandConnection* GetConnection();

		static std::string GetIniFileValue(std::string FilePath, std::string Category, std::string Key);
		static std::string GetDefaultCursor();

		static std::string ParseThemeFile(std::string ThemePath);

		enum class ResizeCursor
		{
			ResizeLeft = int(Window::Cursor::End) + 1,
			ResizeRight,
			ResizeUp,
			ResizeDown,
			ResizeTopLeft,
			ResizeTopRight,
			ResizeBottomLeft,
			ResizeBottomRight,
		};
	};

	class WaylandWindow
	{
	public:
		Window* Parent;
		void Create(Window* Parent, Vec2ui Size, Vec2ui Pos, std::string Title, bool Borderless, bool Resizable, bool AlwaysOnTop);

		void MakeContextCurrent() const;

		void UpdateWindow();
		void Swap() const;
		void Destroy();
		void SetTitle(std::string NewTitle) const;

		void Minimize() const;
		void RestoreWindow();
		void Maximize();
		void InitEGL();

		void SetCursor(Window::Cursor NewCursor);

		void SetMinSize(Vec2ui NewMinSize);
		void SetMaxSize(Vec2ui NewMaxSize);

		void SetSize(Vec2ui NewSize);

		void HandleKey(int Symbol, bool NewValue);
		void SetBorderless(bool NewBorderless);
		void SetResizable(bool NewResizable) const;

		WaylandConnection* Connection;
		EGLSurface GLSurface;
		EGLContext GLContext;
		EGLDisplay GLDisplay = nullptr;
		EGLConfig GLConfig = nullptr;

		wl_surface* WaylandSurface;
		wl_egl_window* WaylandGLWindow;

		libdecor* DecorContext;
		libdecor_frame* DecorFrame;

		Vec2ui FloatingSize;
		Vec2ui ContentSize;

		Vec2ui MinSize = 0;
		Vec2ui MaxSize = 999999;

		std::string TextInput;

		Window::Cursor ActiveCursor = Window::Cursor::Default;

		Vec2ui MousePosition;
		bool Configured = false;
		bool Resizable = false;
		bool Borderless = false;
		bool Maximized = false;
		bool Minimized = false;

		WaylandConnection::ResizeCursor WindowResizeCursor = WaylandConnection::ResizeCursor(0);

		WaylandConnection::ResizeCursor HandleHitTest(uint32_t Serial, bool Move);

		static WaylandWindow* GetFromSurface(wl_surface* s);
	};
}
#endif