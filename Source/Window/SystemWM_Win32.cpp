#if _WIN32
#include "SystemWM.h"
#include "SystemWM_Win32.h"
#include <KlemmUI/Application.h>
#include <Windows.h>
#include <windowsx.h>
#include "../Internal.h"
#include <unordered_map>
#include <GL/glew.h>
#include <GL/wglew.h>
#include <dwmapi.h>
#include <iostream>

#undef IsMaximized

#pragma comment(lib, "Dwmapi.lib")

bool IsMaximized(HWND hWnd)
{
	WINDOWPLACEMENT placement;
	ZeroMemory(&placement, sizeof(placement));
	if (!::GetWindowPlacement(hWnd, &placement)) {
		return false;
	}

	return placement.showCmd == SW_MAXIMIZE;
}

namespace KlemmUI::SystemWM::Borderless
{
	// Based on: https://github.com/melak47/BorderlessWindow/blob/main/src/BorderlessWindow.cpp

	bool IsCompositionEnabled()
	{
		BOOL composition_enabled = FALSE;
		bool success = ::DwmIsCompositionEnabled(&composition_enabled) == S_OK;
		return composition_enabled && success;
	}

	void SetShadow(HWND handle, bool enabled)
	{
		if (IsCompositionEnabled()) {
			static const MARGINS shadow_state[2]{ { 0,0,0,0 },{ 1,1,1,1 } };
			::DwmExtendFrameIntoClientArea(handle, &shadow_state[enabled]);
		}
	}
	/* Adjust client rect to not spill over monitor edges when maximized.
	 * rect(in/out): in: proposed window rect, out: calculated client rect
	 * Does nothing if the window is not maximized.
	 */
	void AdjustFullScreen(HWND window, RECT& rect)
	{
		if (!IsMaximized(window))
		{
			return;
		}

		auto monitor = ::MonitorFromWindow(window, MONITOR_DEFAULTTONULL);
		if (!monitor)
		{
			return;
		}

		MONITORINFO monitor_info{};
		monitor_info.cbSize = sizeof(monitor_info);
		if (!::GetMonitorInfo(monitor, &monitor_info))
		{
			return;
		}

		// when maximized, make the client area fill just the monitor (without task bar) rect,
		// not the whole window rect which extends beyond the monitor.
		rect = monitor_info.rcWork;
	}

	LRESULT HitTest(POINT cursor, KlemmUI::SystemWM::SysWindow* Window)
	{
		bool borderless_resize = true;
		bool borderless_drag = true;
		// identify borders and corners to allow resizing the window.
		// Note: On Windows 10, windows behave differently and
		// allow resizing outside the visible window frame.
		// This implementation does not replicate that behavior.
		const POINT border
		{
			::GetSystemMetrics(SM_CXFRAME) + ::GetSystemMetrics(SM_CXPADDEDBORDER),
			::GetSystemMetrics(SM_CYFRAME) + ::GetSystemMetrics(SM_CXPADDEDBORDER)
		};
		RECT window;
		if (!::GetWindowRect(Window->WindowHandle, &window))
		{
			return HTNOWHERE;
		}

		const auto drag = Window->Parent->IsAreaGrabbableCallback && Window->Parent->IsAreaGrabbableCallback(Window->Parent) ? HTCAPTION : HTCLIENT;

		enum region_mask
		{
			client = 0b0000,
			left   = 0b0001,
			right  = 0b0010,
			top    = 0b0100,
			bottom = 0b1000,
		};

		const auto result =
			left * (cursor.x < (window.left + border.x)) |
			right * (cursor.x >= (window.right - border.x)) |
			top * (cursor.y < (window.top + border.y)) |
			bottom * (cursor.y >= (window.bottom - border.y));

		switch (result)
		{
		case left: return borderless_resize ? HTLEFT : drag;
		case right: return borderless_resize ? HTRIGHT : drag;
		case top: return borderless_resize ? HTTOP : drag;
		case bottom: return borderless_resize ? HTBOTTOM : drag;
		case top | left: return borderless_resize ? HTTOPLEFT : drag;
		case top | right: return borderless_resize ? HTTOPRIGHT : drag;
		case bottom | left: return borderless_resize ? HTBOTTOMLEFT : drag;
		case bottom | right: return borderless_resize ? HTBOTTOMRIGHT : drag;
		case client: return drag;
		default: return HTNOWHERE;
		}
	}
}

static HCURSOR WindowCursors[3] =
{
	LoadCursor(NULL, IDC_ARROW),
	LoadCursor(NULL, IDC_HAND),
	LoadCursor(NULL, IDC_IBEAM),
};

static int ActiveCursor = -1;

static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	using namespace KlemmUI;

	// some vk codes do not have a #define, but why?
	static std::map<int, KlemmUI::Key> Keys =
	{
		{VK_ESCAPE, Key::ESCAPE},
		{VK_BACK, Key::BACKSPACE},
		{VK_TAB, Key::TAB},
		{VK_SPACE, Key::SPACE},
		{VK_OEM_PLUS, Key::PLUS},
		{VK_OEM_COMMA, Key::COMMA},
		{VK_OEM_PERIOD, Key::PERIOD},
		{VK_OEM_2, Key::SLASH},
		{0x30, Key::k0},
		{0x31, Key::k1},
		{0x32, Key::k2},
		{0x33, Key::k3},
		{0x34, Key::k4},
		{0x35, Key::k5},
		{0x36, Key::k6},
		{0x37, Key::k7},
		{0x38, Key::k8},
		{0x39, Key::k9},
		{VK_OEM_1, Key::SEMICOLON},
		{VK_OEM_102, Key::LESS},
		{VK_RETURN, Key::RETURN},
		{VK_OEM_4, Key::LEFTBRACKET},
		{VK_OEM_6, Key::RIGHTBRACKET},
		{VK_RIGHT, Key::RIGHT},
		{VK_LEFT, Key::LEFT},
		{VK_UP, Key::UP},
		{VK_DOWN, Key::DOWN},
		{VK_SHIFT, Key::LSHIFT},
		{VK_CONTROL, Key::LCTRL},
		{VK_MENU, Key::LALT},
		{0x41, Key::a},
		{0x42, Key::b},
		{0x43, Key::c},
		{0x44, Key::d},
		{0x45, Key::e},
		{0x46, Key::f},
		{0x47, Key::g},
		{0x48, Key::h},
		{0x49, Key::i},
		{0x4a, Key::j},
		{0x4b, Key::k},
		{0x4c, Key::l},
		{0x4d, Key::m},
		{0x4e, Key::n},
		{0x4f, Key::o},
		{0x50, Key::p},
		{0x51, Key::q},
		{0x52, Key::r},
		{0x53, Key::s},
		{0x54, Key::t},
		{0x55, Key::u},
		{0x56, Key::v},
		{0x57, Key::w},
		{0x58, Key::x},
		{0x59, Key::y},
		{0x5a, Key::z},
	};

	SystemWM::SysWindow* SysWindow = reinterpret_cast<SystemWM::SysWindow*>(
		::GetWindowLongPtr(hWnd, GWLP_USERDATA));

	if (!SysWindow && uMsg != WM_NCCREATE)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	switch (uMsg)
	{
	case WM_NCCREATE:
	{
		const auto caller = reinterpret_cast<SystemWM::SysWindow*>(
			reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);

		// Change the user data of the window for subsequent messages.
		::SetWindowLongPtr(hWnd, GWLP_USERDATA,
			reinterpret_cast<LONG_PTR>(caller));

		break;
	}
	case WM_ACTIVATE:
	{
		return 0;
	}

	case WM_SETCURSOR:
	{
		static bool LoadedCursor = false;
		if (LOWORD(lParam) == 1 && !LoadedCursor)
		{
			return 0;
		}
		LoadedCursor = true;
		break;
	}

	case WM_NCCALCSIZE:
	{
		if (wParam == TRUE && SysWindow->Borderless)
		{
			NCCALCSIZE_PARAMS& Parameters = *reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
			SystemWM::Borderless::AdjustFullScreen(hWnd, Parameters.rgrc[0]);
			return 0;
		}
		break;
	}
	case WM_CHAR:
	case WM_SYSCHAR:
	{
		if (wParam < '\t')
		{
			break;
		}
		if (wParam <= 0x7F)
		{
			// Plain ASCII
			SysWindow->TextInput.push_back((char)wParam);
		}
		else if (wParam <= 0x07FF)
		{
			// 2-byte unicode
			SysWindow->TextInput.push_back((char)(((wParam >> 6) & 0x1F) | 0xC0));
			SysWindow->TextInput.push_back((char)(((wParam >> 0) & 0x3F) | 0x80));
		}
		break;
	}
	case WM_NCHITTEST:
	{
		if (SysWindow->Borderless)
		{
			return SystemWM::Borderless::HitTest(POINT{
				GET_X_LPARAM(lParam),
				GET_Y_LPARAM(lParam)
			}, SysWindow);
		}
		break;
	}

	case WM_MOUSEWHEEL:
	{
		int Amount = int16_t(HIWORD(wParam)) / WHEEL_DELTA;
		SysWindow->Parent->Input.MoveMouseWheel(Amount);
		break;
	}

	case WM_CLOSE:
	case WM_QUIT:
	{
		SysWindow->Parent->Close();
		return 0;
	}

	case WM_SYSCOMMAND:
	{
		switch (wParam)
		{
		case SC_SCREENSAVE:
		case SC_MONITORPOWER:
			return 0;
		}
		break;
	}
	case WM_KEYDOWN:
	{
		auto k = Keys.find(int(wParam));
		if (k != Keys.end())
		{
			SysWindow->Parent->Input.SetKeyDown(k->second, true);
		}
		return 0;
	}

	case WM_KEYUP:
	{
		auto k = Keys.find(int(wParam));
		if (k != Keys.end())
		{
			SysWindow->Parent->Input.SetKeyDown(k->second, false);
		}
		return 0;
	}

	case WM_SIZE:
	{
		SysWindow->Size = Vector2ui(LOWORD(lParam), HIWORD(lParam));
		SysWindow->Parent->OnResized();
		SysWindow->Parent->RedrawInternal();
		return 0;
	}
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

KlemmUI::SystemWM::SysWindow* KlemmUI::SystemWM::NewWindow(Window* Parent, Vector2ui Size, Vector2ui Pos, std::string Title, bool Borderless, bool Resizable, bool Popup)
{
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	SysWindow* OutWindow = new SysWindow();
	OutWindow->Parent = Parent;
	OutWindow->Size = Size;
	OutWindow->Borderless = Borderless;
	OutWindow->Resizable = Resizable;

	HINSTANCE Instance = GetModuleHandle(NULL);
	static bool WindowClassExists = false;

	WNDCLASS NewWindowClass;
	if (!WindowClassExists)
	{
		NewWindowClass =
		{
			.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
			.lpfnWndProc = &WndProc,
			.hInstance = Instance,
			.hIcon = LoadIcon(NULL, IDI_APPLICATION),
			.hCursor = WindowCursors[0],
			.hbrBackground = NULL,
			.lpszMenuName = NULL,
			.lpszClassName = TEXT("KlemmUIWindow"),
		};
		WindowClassExists = true;

		if (!RegisterClass(&NewWindowClass))
		{
			delete OutWindow;
			Application::Error::Error("Failed to register Win32 window class.", true);
			return nullptr;
		}
	}
	DWORD ExStyle;
	DWORD Style;

	ExStyle = WS_EX_APPWINDOW;
	Style = WS_POPUP | WS_CAPTION | WS_SYSMENU;

	if (Resizable)
	{
		// Maximize button. Only a resizable window should have this.
		Style |= WS_MAXIMIZEBOX;
		// WS_THICKFRAME enables window resizing.
		Style |= WS_THICKFRAME;
	}

	if (!Popup)
	{
		Style |= WS_MINIMIZEBOX;
	}

	OutWindow->WindowHandle = CreateWindowEx(
		ExStyle,
		TEXT("KlemmUIWindow"),
		TEXT(Title.c_str()),
		Style,
		Pos.X,
		Pos.Y,
		Size.X,
		Size.Y,
		NULL,
		NULL,
		Instance,
		OutWindow
	);

	if (!OutWindow->WindowHandle)
	{
		delete OutWindow;
		Application::Error::Error("Failed to create window", true);
		return nullptr;
	}

	static PIXELFORMATDESCRIPTOR PixelFormatDescr =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		0,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	OutWindow->DeviceContext = GetDC(OutWindow->WindowHandle);

	int PixelFormat = 0;

	if (!(PixelFormat = ChoosePixelFormat(OutWindow->DeviceContext, &PixelFormatDescr)))
	{
		delete OutWindow;
		Application::Error::Error("Can't find suitable pixel format.", true);
		return nullptr;
	}

	if (!SetPixelFormat(OutWindow->DeviceContext, PixelFormat, &PixelFormatDescr))
	{
		delete OutWindow;
		Application::Error::Error("Can't set pixel format.", true);
		return nullptr;
	}

	OutWindow->GLContext = wglCreateContext(OutWindow->DeviceContext);

	if (!OutWindow->GLContext)
	{
		delete OutWindow;
		Application::Error::Error("Can't create wgl context.", true);
		return nullptr;
	}

	if (Borderless)
	{
		Borderless::SetShadow(OutWindow->WindowHandle, true);
		SetWindowPos(OutWindow->WindowHandle, NULL, Pos.X, Pos.Y, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
	}

	SystemWM::SetWindowCursor(OutWindow, Window::Cursor::Default);
	ShowWindow(OutWindow->WindowHandle, SW_SHOW);

	OutWindow->MakeContextActive();

	return OutWindow;
}

void KlemmUI::SystemWM::DestroyWindow(SysWindow* Target)
{
	wglDeleteContext(Target->GLContext);
	DestroyWindow(Target->WindowHandle);
}

void KlemmUI::SystemWM::SwapWindow(SysWindow* Target)
{
	wglSwapIntervalEXT(0);
	SwapBuffers(Target->DeviceContext);
}

void KlemmUI::SystemWM::ActivateContext(SysWindow* Target)
{
	Target->MakeContextActive();
}

Vector2ui KlemmUI::SystemWM::GetWindowSize(SysWindow* Target)
{
	return Target->Size;
}

void KlemmUI::SystemWM::UpdateWindow(SysWindow*)
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

bool KlemmUI::SystemWM::WindowHasFocus(SysWindow* Target)
{
	return GetFocus() == Target->WindowHandle;
}

Vector2ui KlemmUI::SystemWM::GetCursorPosition(SysWindow* Target)
{
	POINT p;
	if (GetCursorPos(&p))
	{
		if (ScreenToClient(Target->WindowHandle, &p))
		{
			return Vector2ui(p.x, p.y);
		}
	}
	return 0;
}

Vector2ui KlemmUI::SystemWM::GetScreenSize()
{
	HWND Desktop = GetDesktopWindow();
	RECT Size;
	GetWindowRect(Desktop, &Size);
	return Vector2ui(Size.left + Size.right, Size.top + Size.bottom);
}

std::string KlemmUI::SystemWM::GetTextInput(SysWindow* Target)
{
	std::string Out = Target->TextInput;
	Target->TextInput.clear();
	return Out;
}

uint32_t KlemmUI::SystemWM::GetDesiredRefreshRate(SysWindow* From)
{
	HMONITOR Monitor = MonitorFromWindow(From->WindowHandle, MONITOR_DEFAULTTONEAREST);

	MONITORINFOEX Info;
	ZeroMemory(&Info, sizeof(Info));
	Info.cbSize = sizeof(Info);
	GetMonitorInfo(Monitor, &Info);

	DEVMODE DeviceMode;
	ZeroMemory(&DeviceMode, sizeof(DeviceMode));
	DeviceMode.dmSize = sizeof(DeviceMode);
	EnumDisplaySettings(Info.szDevice, ENUM_CURRENT_SETTINGS, &DeviceMode);

	return DeviceMode.dmDisplayFrequency;
}

void KlemmUI::SystemWM::SetWindowCursor(SysWindow* Target, Window::Cursor NewCursor)
{
	if (ActiveCursor != int(NewCursor))
	{
		ActiveCursor = int(NewCursor);
		SetCursor(WindowCursors[int(NewCursor)]);
	}
}

float KlemmUI::SystemWM::GetDPIScale(SysWindow* Target)
{
	return (float)GetDpiForWindow(Target->WindowHandle) / 96.0f;
}

void KlemmUI::SystemWM::SetClipboardText(std::string NewText)
{
	if (!OpenClipboard(nullptr))
	{
		return;
	}

	HGLOBAL ClipBuffer;
	char* buffer;
	EmptyClipboard();
	ClipBuffer = GlobalAlloc(GMEM_DDESHARE, NewText.size() + 1);
	if (!ClipBuffer)
	{
		return;
	}
	buffer = (char*)GlobalLock(ClipBuffer);
	if (!buffer)
	{
		return;
	}

	strcpy(buffer, NewText.c_str());
	GlobalUnlock(ClipBuffer);
	SetClipboardData(CF_TEXT, ClipBuffer);
	CloseClipboard();
}

std::string KlemmUI::SystemWM::GetClipboardText()
{
	if (!OpenClipboard(nullptr))
	{
		return "";
	}

	HANDLE ClipboardDataHandle = GetClipboardData(CF_TEXT);
	char* pszText = static_cast<char*>(GlobalLock(ClipboardDataHandle));
	if (pszText == nullptr)
	{
		return "";
	}
	std::string Text = pszText;
	GlobalUnlock(ClipboardDataHandle);
	CloseClipboard();
	return Text;
}

bool KlemmUI::SystemWM::IsLMBDown()
{
	return GetKeyState(VK_LBUTTON) & 0x8000;
}

bool KlemmUI::SystemWM::IsRMBDown()
{
	return GetKeyState(VK_RBUTTON) & 0x8000;
}

void KlemmUI::SystemWM::SysWindow::MakeContextActive() const
{
	if (!wglMakeCurrent(DeviceContext, GLContext))
	{
		Application::Error::Error("Failed to make context current.", true);
	}
}
#endif
