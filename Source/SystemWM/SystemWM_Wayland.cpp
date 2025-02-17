#if KLEMMUI_WITH_WAYLAND && !KLEMMUI_CUSTOM_SYSTEMWM
#include "SystemWM_Wayland.h"
#include <wayland-cursor.h>
#include <kui/App.h>
#include <libdecor-0/libdecor.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include <mutex>
#include <sys/mman.h>
#include <cstring>
#include <kui/UI/UIButton.h>
#include <algorithm>

using namespace kui::systemWM;
using namespace kui;

/*
* Who thought any of this was a good idea? These listeners and many callbacks make the code unreadable.
*/

WaylandConnection* WaylandConnection::Current = nullptr;

static std::mutex WindowUpdateMutex;
static std::mutex WindowMutex;
static std::mutex ConnectionMutex;
std::vector<WaylandWindow*> ActiveWindows;

static void HandleConfigureFrame(libdecor_frame* frame, libdecor_configuration* configuration, void* user_data)
{
	WaylandWindow* SysWin = (WaylandWindow*)user_data;
	libdecor_state* DecorState;
	int width, height;

	if (!libdecor_configuration_get_content_size(configuration, frame, &width, &height))
	{
		width = SysWin->FloatingSize.X;
		height = SysWin->FloatingSize.Y;
	}

	if (!SysWin->WaylandGLWindow)
		return;

	libdecor_window_state st;
	if (libdecor_configuration_get_window_state(configuration, &st))
	{
		SysWin->Maximized = st & LIBDECOR_WINDOW_STATE_MAXIMIZED;
	}

	SysWin->ContentSize = Vec2ui(width, height);
	SysWin->Parent->OnResized();

	wl_egl_window_resize(SysWin->WaylandGLWindow, width, height, 0, 0);

	DecorState = libdecor_state_new(width, height);
	libdecor_frame_commit(frame, DecorState, configuration);
	libdecor_state_free(DecorState);

	if (libdecor_frame_is_floating(SysWin->DecorFrame))
	{
		SysWin->FloatingSize = Vec2ui(width, height);
	}

	SysWin->Configured = true;
}

static void HandleCloseFrame(libdecor_frame* frame, void* user_data)
{
	WaylandWindow* w = (WaylandWindow*)user_data;

	w->Parent->Close();
}

static void HandleCommitFrame(libdecor_frame* frame, void* user_data)
{
	WaylandWindow* w = (WaylandWindow*)user_data;
}

static libdecor_frame_interface DecorFrameInterface = {
	HandleConfigureFrame,
	HandleCloseFrame,
	HandleCommitFrame,
};

static void HandleDecorError(struct libdecor* context, libdecor_error error, const char* message)
{
	app::error::Error("libDecor error: " + std::to_string(errno) + ": " + std::string(message), true);
}

static libdecor_interface LibDecorInterface = {
	HandleDecorError,
};

static void HandlePointerEnter(void* data, wl_pointer* pointer, uint32_t serial, wl_surface* surface, wl_fixed_t sx, wl_fixed_t sy)
{
	WaylandConnection* c = (WaylandConnection*)data;

	std::unique_lock g{ WindowMutex };

	c->Serial = serial;
	c->PointerFocus = surface;
	c->PointerWindow = WaylandWindow::GetFromSurface(c->PointerFocus);

	if (c->PointerWindow)
	{
		c->UpdateCursorGraphics();
	}
}

static void HandlePointerLeave(void* data, wl_pointer* pointer, uint32_t serial, wl_surface* surface)
{
	std::unique_lock g{ WindowMutex };
	
	WaylandConnection* c = (WaylandConnection*)data;
	c->Serial = serial;

	if (c->PointerFocus == surface)
	{
		c->PointerFocus = nullptr;
		c->PointerWindow = nullptr;
	}
	wl_surface_commit(c->Cursor.CursorSurface);
	wl_pointer_set_cursor(pointer, serial, nullptr, 0, 0);
}

static void HandlePointerMotion(void* data, wl_pointer* pointer, uint32_t time, wl_fixed_t sx, wl_fixed_t sy)
{
	WaylandConnection* c = (WaylandConnection*)data;
	
	int x = wl_fixed_to_int(sx), y = wl_fixed_to_int(sy);

	std::unique_lock g{ WindowMutex };

	if (!c->PointerWindow)
		return;

	c->PointerWindow->MousePosition = Vec2ui(x, y);

	c->PointerWindow->WindowResizeCursor = c->PointerWindow->HandleHitTest(c->Serial, false);
	c->SetCursor(Window::Cursor(c->PointerWindow->WindowResizeCursor));
}

static void HandlePointerButton(void* data, wl_pointer* wl_pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
{
	WaylandConnection* c = (WaylandConnection*)data;

	c->Serial = serial;

	std::unique_lock g{ WindowMutex };

	if (!c->PointerWindow)
		return;

	switch (button)
	{
	case 272:
		if (state && c->PointerWindow && c->PointerWindow->Borderless && c->PointerWindow->Resizable)
			c->PointerWindow->HandleHitTest(serial, true);
		c->IsLMBDown = state == 1 ? true : false;
		break;
	case 273:
		c->IsRMBDown = state == 1 ? true : false;
		break;
	default:
		break;
	}
}

static void HandlePointerAxis(void* data, wl_pointer* wl_pointer, uint32_t time, uint32_t axis, wl_fixed_t value)
{
	WaylandConnection* c = (WaylandConnection*)data;
	if (axis == WL_POINTER_AXIS_VERTICAL_SCROLL)
	{
		std::unique_lock g{ WindowMutex };

		c->Scrolled -= int(round(wl_fixed_to_double(value) / 5.0));
	}
}

static const wl_pointer_listener PointerListener = {
	HandlePointerEnter,
	HandlePointerLeave,
	HandlePointerMotion,
	HandlePointerButton,
	HandlePointerAxis,
};

static void HandleKeyboardKeymap(void* data, wl_keyboard* wl_keyboard, uint32_t format, int32_t fd, uint32_t size)
{
	WaylandConnection* c = (WaylandConnection*)data;

	if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1)
	{
		close(fd);
		return;
	}

	char* MapString = (char*)(mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0));
	if (MapString == MAP_FAILED)
	{
		close(fd);
		fprintf(stderr, "keymap mmap failed: %s", strerror(errno));
		return;
	}

	xkb_keymap_unref(c->Keyboard.KeyboardMap);

	c->Keyboard.KeyboardMap = xkb_keymap_new_from_string(c->Keyboard.KeyboardContext, MapString, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
	munmap(MapString, size);
	close(fd);

	if (!c->Keyboard.KeyboardMap)
		return;

	c->Keyboard.KeyboardState = xkb_state_new(c->Keyboard.KeyboardMap);
}

static void HandleKeyboardOnEnter(void* data, wl_keyboard* wl_keyboard, uint32_t serial, wl_surface* surface, wl_array* keys)
{
	WaylandConnection* c = (WaylandConnection*)data;

	std::unique_lock g{ WindowMutex };

	c->Serial = serial;
	c->KeyboardWindow = WaylandWindow::GetFromSurface(surface);
}

static void HandleKeyboardLeave(void* data, wl_keyboard* wl_keyboard, uint32_t serial, wl_surface* surface)
{
	std::unique_lock g{ WindowMutex };

	WaylandConnection* c = (WaylandConnection*)data;
	c->Serial = serial;

	c->Keyboard.ClearRepeated();
	if (c->KeyboardWindow && c->KeyboardWindow->WaylandSurface == surface)
	{
		c->KeyboardWindow = nullptr;
	}
}

static void HandleKeyboardKey(void* data, wl_keyboard* wl_keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state)
{
	WaylandConnection* c = (WaylandConnection*)data;
	c->Serial = serial;
	xkb_keycode_t Code = key + 8;

	xkb_keysym_t Symbol = xkb_keysym_to_lower(xkb_state_key_get_one_sym(c->Keyboard.KeyboardState, Code));

	char Buffer[9];
	int Written = xkb_state_key_get_utf8(c->Keyboard.KeyboardState, Code, Buffer, sizeof(Buffer) - 1);
	Buffer[sizeof(Buffer) - 1] = 0;

	std::string TypedString = std::string(Buffer, Written);

	if (TypedString.size() && TypedString[0] < 32)
	{
		TypedString.clear();
	}

	bool IsBackspaceOrDelete = Symbol == XKB_KEY_Delete || Symbol == XKB_KEY_BackSpace;

	if (IsBackspaceOrDelete)
	{
		TypedString.clear();
	}

	std::unique_lock g{ WindowMutex };

	if (state == WL_KEYBOARD_KEY_STATE_PRESSED)
	{
		if (xkb_keymap_key_repeats(c->Keyboard.KeyboardMap, Code) || IsBackspaceOrDelete)
		{
			c->Keyboard.SetRepeated(TypedString, Code, Symbol);
		}
		else
			c->Keyboard.ClearRepeated();

		for (auto& i : ActiveWindows)
		{
			i->HandleKey(Symbol, true);
			i->TextInput += TypedString;
		}
	}
	else if (state == WL_KEYBOARD_KEY_STATE_RELEASED)
	{
		for (auto& i : ActiveWindows)
		{
			i->HandleKey(Symbol, false);
		}
		if (c->Keyboard.RepeatCode == Code)
		{
			c->Keyboard.ClearRepeated();
		}
	}
}

static void HandleKeyboardModifiers(void* data, wl_keyboard* wl_keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group)
{
	WaylandConnection* c = (WaylandConnection*)data;

	xkb_state_update_mask(c->Keyboard.KeyboardState, mods_depressed, mods_latched, mods_locked, 0, 0, group);
	c->Keyboard.ClearRepeated();
}

static void HandleKeyboardRepeatInfo(void* data, wl_keyboard* wl_keyboard, int32_t rate, int32_t delay)
{
	WaylandConnection* c = (WaylandConnection*)data;

	c->Keyboard.RepeatDelay = delay;
	c->Keyboard.RepeatRate = rate;
}

static const wl_keyboard_listener KeyboardListener = {
	HandleKeyboardKeymap,
	HandleKeyboardOnEnter,
	HandleKeyboardLeave,
	HandleKeyboardKey,
	HandleKeyboardModifiers,
	HandleKeyboardRepeatInfo,
};

static void HandleOutputGeometry(void* data, wl_output* output, int x, int y,
	int physical_width, int physical_height, int subpixel, const char* make,
	const char* model, int transform)
{
}

static void HandleOutputMode(void* data, wl_output* output, uint32_t flags, int width, int height, int refresh)
{
	WaylandConnection* c = (WaylandConnection*)data;

	if (!c->HasSize)
	{
		c->PrimarySize = Vec2ui(width, height);
	}
}

static void HandleOutputDone(void* data, wl_output* output)
{
}

// Why is this an int???
static void HandleOutputScale(void* data, wl_output* output, int32_t factor)
{
	// TODO: read scale factor from the surface directly.
}

static const struct wl_output_listener output_listener = {
	HandleOutputGeometry,
	HandleOutputMode,
	HandleOutputDone,
	HandleOutputScale,
};

static void HandleSeatCapabilities(void* data, wl_seat* seat, uint32_t caps)
{
	WaylandConnection* c = (WaylandConnection*)data;

	if (caps & WL_SEAT_CAPABILITY_POINTER && !c->Cursor.Pointer)
	{
		c->Cursor.Pointer = wl_seat_get_pointer(seat);
		c->Cursor.CursorSurface = wl_compositor_create_surface(c->WaylandCompositor);
		c->SetCursor(Window::Cursor::Default);
		wl_pointer_add_listener(c->Cursor.Pointer, &PointerListener, data);
	}
	if (caps & WL_SEAT_CAPABILITY_KEYBOARD)
	{
		c->Keyboard.WaylandKeyboard = wl_seat_get_keyboard(seat);
		c->Keyboard.KeyboardContext = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
		wl_keyboard_add_listener(c->Keyboard.WaylandKeyboard, &KeyboardListener, c);
	}
}

static wl_seat_listener SeatListener = {
	HandleSeatCapabilities,
};

static void HandleRegistryAddGlobal(void* data, wl_registry* wl_registry, uint32_t name, const char* interface, uint32_t version)
{
	WaylandConnection* c = (WaylandConnection*)data;

	std::string InterfaceString = interface;

	if (InterfaceString == wl_compositor_interface.name)
	{
		c->WaylandCompositor = (wl_compositor*)wl_registry_bind(wl_registry, name, &wl_compositor_interface, 1);
	}
	if (InterfaceString == wl_seat_interface.name)
	{
		if (version < 2)
		{
			app::error::Error("wl_output v3 required, v" + std::to_string(version) + " found", true);
		}
		c->WaylandSeat = (wl_seat*)wl_registry_bind(wl_registry, name, &wl_seat_interface, 1);

		wl_seat_add_listener(c->WaylandSeat, &SeatListener, c);
	}
	if (InterfaceString == wl_shell_interface.name)
	{
		c->WaylandShell = (wl_shell*)wl_registry_bind(wl_registry, name, &wl_shell_interface, 1);
	}
	if (InterfaceString == wl_shm_interface.name)
	{
		c->SharedMemory = (wl_shm*)wl_registry_bind(wl_registry, name, &wl_shm_interface, 1);
	}
	if (InterfaceString == wl_output_interface.name)
	{
		auto Output = (wl_output*)wl_registry_bind(wl_registry, name, &wl_output_interface, 1);
		wl_output_add_listener(Output, &output_listener, c);
	}
	if (InterfaceString == wl_data_device_manager_interface.name)
	{
		c->Clipboard.Connection = c;
		c->Clipboard.DataDeviceManager = (wl_data_device_manager*)wl_registry_bind(wl_registry, name,
			&wl_data_device_manager_interface, 1);
	}
}

static void HandleRegistryRemoveGlobal(void* data, wl_registry* wl_registry, uint32_t name)
{
}

static const wl_registry_listener RegistryListener = {
	HandleRegistryAddGlobal,
	HandleRegistryRemoveGlobal
};

void kui::systemWM::WaylandWindow::Create(Window* Parent, Vec2ui Size, Vec2ui Pos, std::string Title, bool Borderless, bool Resizable, bool AlwaysOnTop)
{
	std::unique_lock wg{ WindowUpdateMutex };
	Connection = WaylandConnection::GetConnection();

	this->Resizable = Resizable;
	this->Borderless = Borderless;
	Configured = false;
	FloatingSize = Size;

	InitEGL();

	WaylandSurface = wl_compositor_create_surface(Connection->WaylandCompositor);

	WaylandGLWindow = wl_egl_window_create(WaylandSurface, Size.X, Size.Y);

	GLSurface = eglCreateWindowSurface(GLDisplay, GLConfig, (EGLNativeWindowType)WaylandGLWindow, NULL);

	MakeContextCurrent();

	DecorContext = libdecor_new(Connection->WaylandDisplay, &LibDecorInterface);
	DecorFrame = libdecor_decorate(DecorContext, WaylandSurface, &DecorFrameInterface, this);
	libdecor_frame_set_app_id(DecorFrame, "kui-3-egl-wl-window");
	libdecor_frame_set_title(DecorFrame, Title.c_str());

	if (Borderless)
	{
		libdecor_frame_set_visibility(DecorFrame, false);
	}

	if (!Resizable)
	{
		libdecor_frame_set_min_content_size(DecorFrame, Size.X, Size.Y);
		libdecor_frame_set_max_content_size(DecorFrame, Size.X, Size.Y);
		libdecor_frame_unset_capabilities(DecorFrame, LIBDECOR_ACTION_RESIZE);
	}

	libdecor_frame_map(DecorFrame);

	wl_display_roundtrip(Connection->WaylandDisplay);

	Connection->Clipboard.LoadDeviceManager(Connection->Clipboard.DataDeviceManager);

	while (!Configured)
	{
		if (libdecor_dispatch(DecorContext, 0) < 0)
		{
			app::error::Error("libdecor_dispatch failed");
		}
	}

	std::unique_lock g{ WindowMutex };

	ActiveWindows.push_back(this);
}

void kui::systemWM::WaylandWindow::MakeContextCurrent() const
{
	if (eglGetCurrentContext() != GLContext)
	{
		eglMakeCurrent(GLDisplay, GLSurface, GLSurface, GLContext);
	}
}

void kui::systemWM::WaylandWindow::UpdateWindow()
{
	std::unique_lock g{ WindowUpdateMutex };
	if (Connection->PointerWindow == this)
	{
		Connection->SetCursor(Parent->HasFocus() ? ActiveCursor : Window::Cursor::Default);
		Connection->UpdateCursor();
		Parent->Input.MoveMouseWheel(Connection->Scrolled);
		Connection->Scrolled = 0;
	}
	else if (!Connection->PointerWindow)
	{
		Connection->SetCursor(Window::Cursor::Default);
		Connection->UpdateCursor();
		Connection->Scrolled = 0;
	}
	if (Connection->KeyboardWindow == this)
	{
		TextInput += Connection->Keyboard.UpdateRepeated(Connection);
	}
	if (libdecor_dispatch(DecorContext, 0) < 0)
	{
		app::error::Error("libdecor_dispatch failed");
	}
}

void kui::systemWM::WaylandWindow::Swap() const
{
	eglSwapBuffers(GLDisplay, GLSurface);
}

void kui::systemWM::WaylandWindow::Destroy()
{
	std::unique_lock wg{ WindowUpdateMutex };

	if (GLSurface)
	{
		eglDestroySurface(GLDisplay, GLSurface);
		GLSurface = nullptr;
	}
	if (WaylandGLWindow)
	{
		wl_egl_window_destroy(WaylandGLWindow);
		WaylandGLWindow = nullptr;
	}
	if (WaylandSurface)
	{
		wl_surface_destroy(WaylandSurface);
		WaylandSurface = nullptr;
	}
	if (GLContext)
	{
		eglDestroyContext(GLDisplay, GLContext);
	}

	wl_display_roundtrip(Connection->WaylandDisplay);

	if (DecorContext)
	{
		libdecor_frame_close(DecorFrame);
		libdecor_unref(DecorContext);
	}

	std::unique_lock g{ WindowMutex };


	if (Connection->PointerWindow == this)
	{
		Connection->PointerFocus = nullptr;
		Connection->PointerWindow = nullptr;
	}

	for (auto i = ActiveWindows.begin(); i < ActiveWindows.end(); i++)
	{
		if (*i == this)
		{
			if (Connection->PointerWindow == this)
			{
				Connection->PointerWindow = nullptr;
				Connection->PointerFocus = nullptr;
			}
			ActiveWindows.erase(i);
			break;
		}
	}
}

void kui::systemWM::WaylandWindow::SetTitle(std::string NewTitle) const
{
	libdecor_frame_set_title(DecorFrame, NewTitle.c_str());
}

void kui::systemWM::WaylandWindow::Minimize() const
{
	libdecor_frame_set_minimized(DecorFrame);
}

void kui::systemWM::WaylandWindow::RestoreWindow()
{
	if (Maximized)
	{
		Maximized = false;
		libdecor_frame_unset_maximized(DecorFrame);
	}
}

void kui::systemWM::WaylandWindow::Maximize()
{
	if (!Maximized)
	{
		Maximized = true;
		libdecor_frame_set_maximized(DecorFrame);
	}
}

void kui::systemWM::WaylandWindow::SetMinSize(Vec2ui NewMinSize)
{
	MinSize = NewMinSize;
	libdecor_frame_set_min_content_size(DecorFrame, NewMinSize.X, NewMinSize.Y);
}

void kui::systemWM::WaylandWindow::SetMaxSize(Vec2ui NewMaxSize)
{
	MaxSize = NewMaxSize;
	libdecor_frame_set_max_content_size(DecorFrame, NewMaxSize.X, NewMaxSize.Y);
}

void kui::systemWM::WaylandWindow::SetSize(Vec2ui NewSize)
{
	// TODO: Implement
}

WaylandConnection::ResizeCursor kui::systemWM::WaylandWindow::HandleHitTest(uint32_t Serial, bool Move)
{
	using Cursor = WaylandConnection::ResizeCursor;

	if (!Borderless)
	{
		return Cursor(0);
	}

	Vec2i Border = 8;

	Parent->Input.MousePosition =
		Vec2f((
			(float)MousePosition.X / (float)Parent->GetSize().X - 0.5f) * 2.0f,
			1.0f - ((float)MousePosition.Y / (float)Parent->GetSize().Y * 2.0f)
		);

	bool HoveringButton = Parent->UI.HoveredBox && dynamic_cast<UIButton*>(Parent->UI.HoveredBox);

	bool Grabbable = Parent->Input.MousePosition != Vec2f(99)
		&& !HoveringButton
		&& Parent->IsAreaGrabbableCallback
		&& Parent->IsAreaGrabbableCallback(Parent);

	// Make it easier to press buttons near the screen corners
	if (HoveringButton)
	{
		Border.X /= 2;
		Border.Y /= 2;
	}

	if (!Resizable)
	{
		if (Grabbable && Move)
		{
			libdecor_frame_move(DecorFrame, Connection->WaylandSeat, Serial);
		}
		return Cursor(0);
	}

	enum region_mask
	{
		client = 0b0000,
		left = 0b0001,
		right = 0b0010,
		top = 0b0100,
		bottom = 0b1000,
	};

	const int result =
		left * (MousePosition.X < (Border.X)) |
		right * (MousePosition.X >= (ContentSize.X - Border.X)) |
		top * (MousePosition.Y < (Border.Y)) |
		bottom * (MousePosition.Y >= (ContentSize.Y - Border.Y));

	libdecor_resize_edge ResizeEdge = LIBDECOR_RESIZE_EDGE_NONE;

	switch (result)
	{
	case left: ResizeEdge = LIBDECOR_RESIZE_EDGE_LEFT; break;
	case right: ResizeEdge = LIBDECOR_RESIZE_EDGE_RIGHT; break;
	case top: ResizeEdge = LIBDECOR_RESIZE_EDGE_TOP; break;
	case bottom: ResizeEdge = LIBDECOR_RESIZE_EDGE_BOTTOM; break;
	case top | left: ResizeEdge = LIBDECOR_RESIZE_EDGE_TOP_LEFT; break;
	case top | right: ResizeEdge = LIBDECOR_RESIZE_EDGE_TOP_RIGHT; break;
	case bottom | left: ResizeEdge = LIBDECOR_RESIZE_EDGE_BOTTOM_LEFT; break;
	case bottom | right: ResizeEdge = LIBDECOR_RESIZE_EDGE_BOTTOM_RIGHT; break;
	case client:
	default:
		if (Grabbable && Move)
		{
			libdecor_frame_move(DecorFrame, Connection->WaylandSeat, Serial);
		}
	}
	
	if (ResizeEdge != LIBDECOR_RESIZE_EDGE_NONE)
	{
		if (Move)
			libdecor_frame_resize(DecorFrame, Connection->WaylandSeat, Serial, ResizeEdge);

		switch (ResizeEdge)
		{
		case LIBDECOR_RESIZE_EDGE_TOP:
			return Cursor::ResizeUp;
		case LIBDECOR_RESIZE_EDGE_BOTTOM:
			return Cursor::ResizeDown;
		case LIBDECOR_RESIZE_EDGE_LEFT:
			return Cursor::ResizeLeft;
		case LIBDECOR_RESIZE_EDGE_TOP_LEFT:
			return Cursor::ResizeTopLeft;
		case LIBDECOR_RESIZE_EDGE_BOTTOM_LEFT:
			return Cursor::ResizeBottomLeft;
		case LIBDECOR_RESIZE_EDGE_RIGHT:
			return Cursor::ResizeRight;
		case LIBDECOR_RESIZE_EDGE_TOP_RIGHT:
			return Cursor::ResizeTopRight;
		case LIBDECOR_RESIZE_EDGE_BOTTOM_RIGHT:
			return Cursor::ResizeBottomRight;
		default:
			break;
		}
	}
	return Cursor(0);
}

WaylandWindow* kui::systemWM::WaylandWindow::GetFromSurface(wl_surface* s)
{
	for (WaylandWindow* i : ActiveWindows)
	{
		if (i->WaylandSurface == s)
		{
			return i;
		}
	}
	return nullptr;
}

kui::systemWM::WaylandConnection::WaylandConnection()
{
	WaylandDisplay = wl_display_connect(NULL);
	if (!WaylandDisplay)
	{
		app::error::Error("Failed to open Wayland display", true);
	}

	WaylandRegistry = wl_display_get_registry(WaylandDisplay);
	wl_registry_add_listener(WaylandRegistry, &RegistryListener, this);
	wl_display_roundtrip(WaylandDisplay);
	wl_display_roundtrip(WaylandDisplay);
}

wl_cursor* kui::systemWM::WaylandConnection::CreateCursor(Window::Cursor New)
{
	if (!Cursor.CursorTheme)
	{
		std::string Theme = GetDefaultCursor();
		Cursor.CursorTheme = wl_cursor_theme_load(Theme.empty() ? NULL : Theme.c_str(), 24, SharedMemory);
	}

	std::vector<std::string> NamesToTry;

	// This is sane
	switch (New)
	{
	case Window::Cursor::Default:
		NamesToTry = { "default", "left_ptr", "right_ptr"};
		break;
	case Window::Cursor::Hand:
		NamesToTry = { "hand2", "hand1", "hand" };
		break;
	case Window::Cursor::Text:
		NamesToTry = { "ibeam", "xterm" };
		break;

	case Window::Cursor::ResizeLeftRight:
		NamesToTry = { "ew-resize", "col-resize" };
		break;

	case Window::Cursor::ResizeUpDown:
		NamesToTry = { "ns-resize", "row-resize" };
		break;

	case Window::Cursor(ResizeCursor::ResizeTopRight):
		NamesToTry = { "ne-resize" };
		break;

	case Window::Cursor(ResizeCursor::ResizeUp):
		NamesToTry = { "n-resize", "ns-resize" };
		break;

	case Window::Cursor(ResizeCursor::ResizeTopLeft):
		NamesToTry = { "nw-resize" };
		break;

		case Window::Cursor(ResizeCursor::ResizeLeft):
		NamesToTry = { "w-resize", "ew-resize" };
		break;

	case Window::Cursor(ResizeCursor::ResizeBottomLeft):
		NamesToTry = { "sw-resize" };
		break;

	case Window::Cursor(ResizeCursor::ResizeDown):
		NamesToTry = { "s-resize", "ns-resize" };
		break;

	case Window::Cursor(ResizeCursor::ResizeBottomRight):
		NamesToTry = { "se-resize" };
		break;

	case Window::Cursor(ResizeCursor::ResizeRight):
		NamesToTry = { "e-resize", "ew-resize" };
		break;

	default:
		break;
	}
	wl_cursor* NewCursor = nullptr;
	for (auto& i : NamesToTry)
	{
		NewCursor = wl_cursor_theme_get_cursor(Cursor.CursorTheme, i.c_str());
		if (NewCursor)
			break;
	}

	if (!NewCursor)
	{
		if (New == Window::Cursor::Default)
		{
			app::error::Error("Failed to create the default cursor, is the cursor theme broken?", true);
			return nullptr;
		}
		std::cerr << "Failed to create cursor type with id " << std::to_string(int(New)) << ", using default" << std::endl;
		return CreateCursor(Window::Cursor::Default);
	}

	Cursor.LoadedCursors.insert({ New, NewCursor });
	return NewCursor;
}

void kui::systemWM::WaylandConnection::UpdateCursor()
{
	if (PointerWindow == nullptr)
		return;

	if (Cursor.CurrentCursorAnimation->image_count == 1)
	{
		return;
	}

	if (Cursor.CurrentCursorImage->delay < Cursor.AnimationTimer.Get() * 1000.0f)
	{
		NextCursorFrame();
		UpdateCursorGraphics();
	}
}

void kui::systemWM::WaylandConnection::NextCursorFrame()
{
	Cursor.AnimationFrame++;
	Cursor.AnimationTimer.Reset();
	if (Cursor.AnimationFrame >= Cursor.CurrentCursorAnimation->image_count)
	{
		Cursor.AnimationFrame = 0;
	}
	Cursor.CurrentCursorImage = Cursor.CurrentCursorAnimation->images[Cursor.AnimationFrame];
	wl_surface_attach(Cursor.CursorSurface, wl_cursor_image_get_buffer(Cursor.CurrentCursorImage), 0, 0);
}

void kui::systemWM::WaylandConnection::UpdateCursorGraphics() const
{
	wl_surface_attach(Cursor.CursorSurface, wl_cursor_image_get_buffer(Cursor.CurrentCursorImage), 0, 0);
	wl_pointer_set_cursor(Cursor.Pointer, Serial, Cursor.CursorSurface, Cursor.CurrentCursorImage->hotspot_x, Cursor.CurrentCursorImage->hotspot_y);
	wl_surface_damage(Cursor.CursorSurface, 0, 0, Cursor.CurrentCursorImage->width, Cursor.CurrentCursorImage->height);
	wl_surface_commit(Cursor.CursorSurface);
}

void kui::systemWM::WaylandConnection::SetCursor(Window::Cursor New)
{
	if (New < Window::Cursor::End && PointerWindow && PointerWindow->WindowResizeCursor != ResizeCursor(0))
	{
		return;
	}

	if (New == CurrentCursor)
	{
		return;
	}

	if (Cursor.LoadedCursors.contains(New))
	{
		Cursor.CurrentCursorAnimation = Cursor.LoadedCursors[New];
	}
	else
	{
		Cursor.CurrentCursorAnimation = CreateCursor(New);
	}
	CurrentCursor = New;
	Cursor.CurrentCursorImage = Cursor.CurrentCursorAnimation->images[0];
	UpdateCursorGraphics();
}

void kui::systemWM::WaylandWindow::InitEGL()
{
	static const EGLint ConfigAttributes[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
		EGL_NONE
	};


	GLDisplay = eglGetDisplay((EGLNativeDisplayType)Connection->WaylandDisplay);

	EGLint Major, Minor;
	if (!eglInitialize(GLDisplay, &Major, &Minor))
	{
		app::error::Error("Cannot initialise EGL!", true);
	}

	if (!eglBindAPI(EGL_OPENGL_API))
	{
		app::error::Error("Cannot bind EGL API!", true);
	}

	EGLint NumConfig;
	if (!eglChooseConfig(GLDisplay, ConfigAttributes, &GLConfig, 1, &NumConfig))
	{
		app::error::Error("No matching EGL configurations!", true);
	}

	GLContext = eglCreateContext(GLDisplay, GLConfig, EGL_NO_CONTEXT, NULL);

	if (GLContext == EGL_NO_CONTEXT)
	{
		app::error::Error("No EGL context!", true);
	}

	eglSwapInterval(GLDisplay, 0);
}

void kui::systemWM::WaylandWindow::SetCursor(Window::Cursor NewCursor)
{
	ActiveCursor = NewCursor;
}

WaylandConnection* kui::systemWM::WaylandConnection::GetConnection()
{
	std::unique_lock g{ ConnectionMutex };
	if (!Current)
	{
		Current = new WaylandConnection();
	}

	return Current;
}

static std::string TrimStr(std::string Input)
{
	Input.erase(Input.begin(), std::find_if(Input.begin(), Input.end(), [](char c) {
		return !std::isspace(c);
		}));

	Input.erase(std::find_if(Input.rbegin(), Input.rend(), [](char c) {
		return !std::isspace(c);
		}).base(), Input.end());
	return Input;
}

std::string kui::systemWM::WaylandConnection::GetIniFileValue(std::string FilePath, std::string Category, std::string Key)
{
	if (!std::filesystem::is_regular_file(FilePath))
	{
		return "";
	}
	std::ifstream FileStream = std::ifstream(FilePath);

	std::string CurrentCategory;

	while (true)
	{
		char Line[8192];
		FileStream.getline(Line, sizeof(Line));

		std::string LineString = Line;

		if (FileStream.eof() && LineString.empty())
			break;

		if (LineString.empty())
			continue;

		if (LineString.size() > 2 && LineString[0] == '[' && LineString[LineString.size() - 1] == ']')
		{
			CurrentCategory = TrimStr(LineString.substr(1, LineString.size() - 2));
			continue;
		}

		size_t Equals = LineString.find_first_of("=");

		if (Equals == std::string::npos)
			continue;

		std::string First = TrimStr(LineString.substr(0, Equals)), Last = (LineString.substr(Equals + 1));


		if (First == Key && CurrentCategory == Category)
		{
			std::cerr << "Found '[" << Category << "]." << Key << "' in '" << FilePath << "': '" << Last << "'" << std::endl;
			return Last;
		}
	}

	return "";
}

// Reads configuration files to find the correct mouse cursor. Wayland does this client side for whatever reason.
std::string kui::systemWM::WaylandConnection::GetDefaultCursor()
{
	std::string HomeDir = getenv("HOME");

	// KDE stores it's default cursor theme here instead of having a default theme for some reason.
	std::string KdeConfigFile = GetIniFileValue(HomeDir + "/.config/kcminputrc", "Mouse", "cursorTheme");

	if (!KdeConfigFile.empty())
		return KdeConfigFile;

	std::vector<std::string> FilesToCheck = {
		HomeDir + "/.icons/default/index.theme",
		HomeDir + "/.local/share/icons/default/index.theme",
	};

	for (const std::string& i : FilesToCheck)
	{
		std::string UserFile = ParseThemeFile(i);
		if (!UserFile.empty())
			return UserFile;
	}
	
	return ParseThemeFile("/usr/share/icons/default/index.theme");
}

std::string kui::systemWM::WaylandConnection::ParseThemeFile(std::string ThemePath)
{
	return GetIniFileValue(ThemePath, "Icon Theme", "Inherits");
}

void kui::systemWM::WaylandWindow::HandleKey(int Symbol, bool NewValue)
{
	static std::map<int, kui::Key> Keys =
	{
		{ XKB_KEY_Escape, Key::ESCAPE },
		{ XKB_KEY_BackSpace, Key::BACKSPACE },
		{ XKB_KEY_Tab, Key::TAB },
		{ XKB_KEY_KP_Space, Key::SPACE },
		{ XKB_KEY_Delete, Key::DELETE },
		{ XKB_KEY_plus, Key::PLUS },
		{ XKB_KEY_comma, Key::COMMA },
		{ XKB_KEY_period, Key::PERIOD },
		{ XKB_KEY_slash, Key::SLASH },
		{ XKB_KEY_0, Key::k0 },
		{ XKB_KEY_1, Key::k1 },
		{ XKB_KEY_2, Key::k2 },
		{ XKB_KEY_3, Key::k3 },
		{ XKB_KEY_4, Key::k4 },
		{ XKB_KEY_5, Key::k5 },
		{ XKB_KEY_6, Key::k6 },
		{ XKB_KEY_7, Key::k7 },
		{ XKB_KEY_8, Key::k8 },
		{ XKB_KEY_9, Key::k9 },
		{ XKB_KEY_semicolon, Key::SEMICOLON },
		{ XKB_KEY_less, Key::LESS },
		{ XKB_KEY_Return, Key::RETURN },
		{ XKB_KEY_bracketleft, Key::LEFTBRACKET },
		{ XKB_KEY_bracketright, Key::RIGHTBRACKET },
		{ XKB_KEY_Right, Key::RIGHT },
		{ XKB_KEY_Left, Key::LEFT },
		{ XKB_KEY_Up, Key::UP },
		{ XKB_KEY_Down, Key::DOWN },
		{ XKB_KEY_Shift_L, Key::LSHIFT },
		{ XKB_KEY_Control_L, Key::LCTRL },
		{ XKB_KEY_Alt_L, Key::LALT },
		{ XKB_KEY_Shift_R, Key::RSHIFT },
		{ XKB_KEY_Control_R, Key::RCTRL },
		{ XKB_KEY_Alt_R, Key::RALT },
		{ XKB_KEY_a, Key::a },
		{ XKB_KEY_b, Key::b },
		{ XKB_KEY_c, Key::c },
		{ XKB_KEY_d, Key::d },
		{ XKB_KEY_e, Key::e },
		{ XKB_KEY_f, Key::f },
		{ XKB_KEY_g, Key::g },
		{ XKB_KEY_h, Key::h },
		{ XKB_KEY_i, Key::i },
		{ XKB_KEY_j, Key::j },
		{ XKB_KEY_k, Key::k },
		{ XKB_KEY_l, Key::l },
		{ XKB_KEY_m, Key::m },
		{ XKB_KEY_n, Key::n },
		{ XKB_KEY_o, Key::o },
		{ XKB_KEY_p, Key::p },
		{ XKB_KEY_q, Key::q },
		{ XKB_KEY_r, Key::r },
		{ XKB_KEY_s, Key::s },
		{ XKB_KEY_t, Key::t },
		{ XKB_KEY_u, Key::u },
		{ XKB_KEY_v, Key::v },
		{ XKB_KEY_w, Key::w },
		{ XKB_KEY_x, Key::x },
		{ XKB_KEY_y, Key::y },
		{ XKB_KEY_z, Key::z },
	};

	if (!Keys.contains(Symbol))
		return;
	Parent->Input.SetKeyDown(Keys[Symbol], NewValue);
}

void kui::systemWM::WaylandWindow::SetBorderless(bool NewBorderless)
{
	if (Borderless != NewBorderless)
	{
		Borderless = NewBorderless;
		libdecor_frame_set_visibility(DecorFrame, !NewBorderless);
	}
}

void kui::systemWM::WaylandWindow::SetResizable(bool NewResizable) const
{
	if (NewResizable)
	{
		libdecor_frame_set_min_content_size(DecorFrame, MinSize.X, MinSize.Y);
		libdecor_frame_set_max_content_size(DecorFrame, MaxSize.X, MaxSize.Y);
		libdecor_frame_set_capabilities(DecorFrame, LIBDECOR_ACTION_RESIZE);
	}
	else
	{
		libdecor_frame_set_min_content_size(DecorFrame, ContentSize.X, ContentSize.Y);
		libdecor_frame_set_max_content_size(DecorFrame, ContentSize.X, ContentSize.Y);
		libdecor_frame_unset_capabilities(DecorFrame, LIBDECOR_ACTION_RESIZE);
	}
}

void kui::systemWM::WaylandKeyboardInfo::SetRepeated(std::string RepeatedString, int Code, int Symbol)
{
	this->RepeatedString = RepeatedString;
	InitialDelayDone = false;
	RepeatCode = Code;
	RepeatSymbol = Symbol;
	RepeatTimer.Reset();
}

void kui::systemWM::WaylandKeyboardInfo::ClearRepeated()
{
	RepeatedString.clear();
	RepeatSymbol = 0;
	InitialDelayDone = false;
}

// This is also done client side for some reason
std::string kui::systemWM::WaylandKeyboardInfo::UpdateRepeated(WaylandConnection* With)
{
	if (RepeatedString.empty() && RepeatSymbol == 0)
		return "";

	uint32_t ElapsedMs = uint32_t(RepeatTimer.Get() * 1000.0f);

	if (ElapsedMs > RepeatDelay && !InitialDelayDone)
	{
		InitialDelayDone = true;
		if (With->KeyboardWindow)
			With->KeyboardWindow->HandleKey(RepeatSymbol, true);
		RepeatTimer.Reset();
		return RepeatedString;
	}
	
	if (!InitialDelayDone)
		return "";

	if (ElapsedMs > RepeatRate)
	{
		RepeatTimer.Reset();
		if (With->KeyboardWindow)
			With->KeyboardWindow->HandleKey(RepeatSymbol, true);
		return RepeatedString;
	}
	return "";
}
#endif