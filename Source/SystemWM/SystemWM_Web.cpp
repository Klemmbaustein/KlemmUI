#ifdef KLEMMUI_WEB_BUILD
#include "SystemWM.h"
#include "../Internal/OpenGL.h"
#include "SystemWM_Web.h"
#include <iostream>
#include <emscripten.h>
#include <map>

using namespace kui;

static bool LMB = false, RMB = false;

static bool CheckFlag(kui::Window::WindowFlag Flag, kui::Window::WindowFlag Value)
{
	return (Flag & Value) == Value;
}

static std::map<int, kui::Key> Keys =
{
	std::pair{GLFW_KEY_ESCAPE, Key::ESCAPE},
	std::pair{GLFW_KEY_BACKSPACE, Key::BACKSPACE},
	std::pair{GLFW_KEY_TAB, Key::TAB},
	std::pair{GLFW_KEY_SPACE, Key::SPACE},
	//{GLFW_KEY_, Key::PLUS},
	std::pair{GLFW_KEY_COMMA, Key::COMMA},
	std::pair{GLFW_KEY_PERIOD, Key::PERIOD},
	std::pair{GLFW_KEY_SLASH, Key::SLASH},
	std::pair{GLFW_KEY_0, Key::k0},
	std::pair{GLFW_KEY_1, Key::k1},
	std::pair{GLFW_KEY_2, Key::k2},
	std::pair{GLFW_KEY_3, Key::k3},
	std::pair{GLFW_KEY_4, Key::k4},
	std::pair{GLFW_KEY_5, Key::k5},
	std::pair{GLFW_KEY_6, Key::k6},
	std::pair{GLFW_KEY_7, Key::k7},
	std::pair{GLFW_KEY_8, Key::k8},
	std::pair{GLFW_KEY_9, Key::k9},
	std::pair{GLFW_KEY_SEMICOLON, Key::SEMICOLON},
	//{GLFW_KEY_, Key::LESS},
	std::pair{GLFW_KEY_ENTER, Key::RETURN},
	std::pair{GLFW_KEY_LEFT_BRACKET, Key::LEFTBRACKET},
	std::pair{GLFW_KEY_RIGHT_BRACKET, Key::RIGHTBRACKET},
	std::pair{GLFW_KEY_RIGHT, Key::RIGHT},
	std::pair{GLFW_KEY_LEFT, Key::LEFT},
	std::pair{GLFW_KEY_UP, Key::UP},
	std::pair{GLFW_KEY_DOWN, Key::DOWN},
	std::pair{GLFW_KEY_LEFT_SHIFT, Key::LSHIFT},
	std::pair{GLFW_KEY_RIGHT_SHIFT, Key::RSHIFT},
	std::pair{GLFW_KEY_LEFT_CONTROL, Key::LCTRL},
	std::pair{GLFW_KEY_RIGHT_CONTROL, Key::RCTRL},
	std::pair{GLFW_KEY_LEFT_ALT, Key::LALT},
	std::pair{GLFW_KEY_RIGHT_ALT, Key::RALT},
	std::pair{GLFW_KEY_DELETE, Key::DELETE},
	std::pair{GLFW_KEY_A, Key::a},
	std::pair{GLFW_KEY_B, Key::b},
	std::pair{GLFW_KEY_C, Key::c},
	std::pair{GLFW_KEY_D, Key::d},
	std::pair{GLFW_KEY_E, Key::e},
	std::pair{GLFW_KEY_F, Key::f},
	std::pair{GLFW_KEY_G, Key::g},
	std::pair{GLFW_KEY_H, Key::h},
	std::pair{GLFW_KEY_I, Key::i},
	std::pair{GLFW_KEY_J, Key::j},
	std::pair{GLFW_KEY_K, Key::k},
	std::pair{GLFW_KEY_L, Key::l},
	std::pair{GLFW_KEY_M, Key::m},
	std::pair{GLFW_KEY_N, Key::n},
	std::pair{GLFW_KEY_O, Key::o},
	std::pair{GLFW_KEY_P, Key::p},
	std::pair{GLFW_KEY_Q, Key::q},
	std::pair{GLFW_KEY_R, Key::r},
	std::pair{GLFW_KEY_S, Key::s},
	std::pair{GLFW_KEY_T, Key::t},
	std::pair{GLFW_KEY_U, Key::u},
	std::pair{GLFW_KEY_V, Key::v},
	std::pair{GLFW_KEY_W, Key::w},
	std::pair{GLFW_KEY_X, Key::x},
	std::pair{GLFW_KEY_Y, Key::y},
	std::pair{GLFW_KEY_Z, Key::z},
};

kui::systemWM::SysWindow* kui::systemWM::NewWindow(Window* Parent, Vec2ui Size, Vec2ui Pos, std::string Title, Window::WindowFlag Flags)
{
	SysWindow* OutWindow = new SysWindow();

	glfwInit();

	OutWindow->GLWindow = glfwCreateWindow(Size.X, Size.Y, Title.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(OutWindow->GLWindow, OutWindow);

	glfwSetKeyCallback(OutWindow->GLWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			SysWindow* Win = reinterpret_cast<SysWindow*>(glfwGetWindowUserPointer(window));

			if (Keys.contains(key))
				Win->Parent->Input.SetKeyDown(Keys[key], action != GLFW_RELEASE);
		});

	glfwSetCharCallback(OutWindow->GLWindow, [](GLFWwindow* window, unsigned int wParam)
		{
			SysWindow* Win = reinterpret_cast<SysWindow*>(glfwGetWindowUserPointer(window));

			if (wParam < ' ' && wParam != '\t')
			{
				return;
			}
			if (wParam <= 0x7F)
			{
				Win->TextInput.push_back((char)wParam);
			}
			else if (wParam <= 0x07FF)
			{
				Win->TextInput.push_back((char)(((wParam >> 6) & 0x1F) | 0xC0));
				Win->TextInput.push_back((char)(((wParam >> 0) & 0x3F) | 0x80));
			}
		});
	
	glfwSetWindowSizeCallback(OutWindow->GLWindow, [](GLFWwindow* win, int x, int y)
		{
			SysWindow* Win = reinterpret_cast<SysWindow*>(glfwGetWindowUserPointer(win));
			Win->Parent->OnResized();
		});

	auto& cur = OutWindow->LoadedCursors;
	cur[size_t(Window::Cursor::Default)] = nullptr;
	cur[size_t(Window::Cursor::Hand)] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
	cur[size_t(Window::Cursor::Text)] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
	cur[size_t(Window::Cursor::ResizeLeftRight)] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
	cur[size_t(Window::Cursor::ResizeUpDown)] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);

	ActivateContext(OutWindow);

	return OutWindow;
}

void kui::systemWM::DestroyWindow(SysWindow* Target)
{
	glfwDestroyWindow(Target->GLWindow);
	delete Target;
}

void kui::systemWM::SwapWindow(SysWindow* Target)
{
	glfwSwapBuffers(Target->GLWindow);
}

void kui::systemWM::UpdateWindowFlags(SysWindow* Target, Window::WindowFlag NewFlags)
{
}

void kui::systemWM::ActivateContext(SysWindow* Target)
{
	glfwMakeContextCurrent(Target->GLWindow);
}

kui::Vec2ui kui::systemWM::GetWindowSize(SysWindow* Target)
{
	int w, h;
	glfwGetWindowSize(Target->GLWindow, &w, &h);
	return Vec2ui(w, h);
}

void kui::systemWM::UpdateWindow(SysWindow* Target)
{
	glfwPollEvents();
	LMB = glfwGetMouseButton(Target->GLWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
	RMB = glfwGetMouseButton(Target->GLWindow, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
}

bool kui::systemWM::WindowHasFocus(SysWindow* Target)
{
	return true;
}

kui::Vec2i kui::systemWM::GetCursorPosition(SysWindow* Target)
{
	double X, Y;
	glfwGetCursorPos(Target->GLWindow, &X, &Y);
	return kui::Vec2i(int64_t(X), int64_t(Y));
}

kui::Vec2ui kui::systemWM::GetScreenSize()
{
	int x, y, w, h;
	glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), &x, &y, &w, &h);
	return Vec2ui(w, h);
}

std::string kui::systemWM::GetTextInput(SysWindow* Target)
{
	std::string Copy = Target->TextInput;
	Target->TextInput.clear();
	return Copy;
}

uint32_t kui::systemWM::GetDesiredRefreshRate(SysWindow* From)
{
	auto md = glfwGetVideoMode(glfwGetWindowMonitor(From->GLWindow));
	return md->refreshRate;
}

void kui::systemWM::SetWindowCursor(SysWindow* Target, Window::Cursor NewCursor)
{
	glfwSetCursor(Target->GLWindow, Target->LoadedCursors[size_t(NewCursor)]);
}

float kui::systemWM::GetDPIScale(SysWindow* Target)
{
	float x, y;
	glfwGetMonitorContentScale(glfwGetWindowMonitor(Target->GLWindow), &x, &y);
	return (x + y) / 2.0f;
}

void kui::systemWM::SetClipboardText(std::string NewText)
{
	glfwSetClipboardString(nullptr, NewText.c_str());
}

std::string kui::systemWM::GetClipboardText()
{
	return glfwGetClipboardString(nullptr);
}

bool kui::systemWM::IsLMBDown()
{
	return LMB;
}

bool kui::systemWM::IsRMBDown()
{
	return RMB;
}

void kui::systemWM::SetWindowSize(SysWindow* Target, Vec2ui Size)
{
	glfwSetWindowSize(Target->GLWindow, Size.X, Size.Y);
}

void kui::systemWM::SetWindowPosition(SysWindow* Target, Vec2ui NewPosition)
{
}

void kui::systemWM::SetTitle(SysWindow* Target, std::string Text)
{
	glfwSetWindowTitle(Target->GLWindow, Text.c_str());
}

bool kui::systemWM::IsWindowFullScreen(SysWindow* Target)
{
	return false;
}
bool kui::systemWM::IsWindowMinimized(SysWindow* Target)
{
	return false;
}

void kui::systemWM::SetWindowMinSize(SysWindow* Target, Vec2ui MinSize)
{
}

void kui::systemWM::SetWindowMaxSize(SysWindow* Target, Vec2ui MaxSize)
{
}

void kui::systemWM::RestoreWindow(SysWindow* Target)
{
}

void kui::systemWM::MinimizeWindow(SysWindow* Target)
{
}

void kui::systemWM::MaximizeWindow(SysWindow* Target)
{
}

void kui::systemWM::SetWindowIcon(SysWindow* Target, uint8_t* Bytes, size_t Width, size_t Height)
{
}

void* kui::systemWM::GetPlatformHandle(SysWindow* Target)
{
	return Target->GLWindow;
}

void kui::systemWM::MessageBox(std::string Text, std::string Title, int Type)
{
	// TODO: implemenent something better
	std::cout << Title << Text << std::endl;
}

void kui::systemWM::WaitFrame(SysWindow* Target, float RemainingTime)
{
	emscripten_sleep(unsigned(RemainingTime * 1000.0f));
}

#endif