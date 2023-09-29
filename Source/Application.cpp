#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "Application.h"
#include <GL/glew.h>
#include <iostream>
#include "Rendering/VertexBuffer.h"
#include "Rendering/Shader.h"
#include "Rendering/Framebuffer.h"
#include "UI/UIBackground.h"
#include "UI/UIText.h"
#include "UI/UIButton.h"
#include "Input.h"
#include "Math/MathHelpers.h"
#include "Rendering/ScrollObject.h"
#include <UI/UITextField.h>

void GLAPIENTRY
MessageCallback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam
)
{
	if ((type == GL_DEBUG_TYPE_ERROR || type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR || type == GL_DEBUG_TYPE_PORTABILITY))
	{
		Application::Error("OpenGL error: (" + std::to_string(id) + "): " + std::string(message));
		throw 0;
	}
}

namespace Application
{
	std::string NewTypedText;
	float AspectRatio = 16.f / 9.f;
	SDL_Window* Window = nullptr;
	bool Quit = false;
	unsigned int DesiredRefreshRate = 60;
	std::string WindowTitle = "Window";
	Vector2ui WindowResolution = Vector2ui(640, 480);
	Shader* PostProcessShader = nullptr;
	std::vector<ButtonEvent> ButtonEvents;
	float DeltaTime = 0;
	float Time = 0;

	bool(*IsMovableCallback)() = nullptr;
	void(*ErrorMessageCallback)(std::string Message) = [](std::string Message) {
		std::cerr << "[KlemmUI Error]: " << Message << std::endl;
	};

	void(*OnResizedCallback)() = nullptr;

	bool IsBorderless = false;

	bool PreviousHasWindowFocus = false;
	Vector3f32 BorderlessWindowOutlineColor = Vector3f32(0, 0.5, 1);
	constexpr int MOUSE_GRAB_PADDING = 8;
	bool IsWindowFullscreen = false;

	SDL_HitTestResult HitTestCallback(SDL_Window* Window, const SDL_Point* Area, void* Data)
	{
		int Width, Height;
		SDL_GetWindowSize(Window, &Width, &Height);
		int x;
		int y;
		SDL_GetMouseState(&x, &y);
		Input::MouseLocation = Vector2(((float)Area->x / (float)Width - 0.5f) * 2.0f, 1.0f - ((float)Area->y / (float)Height * 2.0f));

		if (Area->y < MOUSE_GRAB_PADDING)
		{
			if (Area->x < MOUSE_GRAB_PADDING)
			{
				return SDL_HITTEST_RESIZE_TOPLEFT;
			}
			else if (Area->x > Width - MOUSE_GRAB_PADDING)
			{
				return SDL_HITTEST_RESIZE_TOPRIGHT;
			}
			else
			{
				return SDL_HITTEST_RESIZE_TOP;
			}
		}
		else if (Area->y > Height - MOUSE_GRAB_PADDING)
		{
			if (Area->x < MOUSE_GRAB_PADDING)
			{
				return SDL_HITTEST_RESIZE_BOTTOMLEFT;
			}
			else if (Area->x > Width - MOUSE_GRAB_PADDING)
			{
				return SDL_HITTEST_RESIZE_BOTTOMRIGHT;
			}
			else
			{
				return SDL_HITTEST_RESIZE_BOTTOM;
			}
		}
		else if (Area->x < MOUSE_GRAB_PADDING)
		{
			return SDL_HITTEST_RESIZE_LEFT;
		}
		else if (Area->x > Width - MOUSE_GRAB_PADDING)
		{
			return SDL_HITTEST_RESIZE_RIGHT;
		}
		else if (!UI::HoveredBox && IsMovableCallback && IsMovableCallback())
		{
			return SDL_HITTEST_DRAGGABLE;
		}

		return SDL_HITTEST_NORMAL;
	}

	void SetApplicationTitle(std::string NewTitle)
	{
		WindowTitle = NewTitle;
		SDL_SetWindowTitle(Window, NewTitle.c_str());
	}
	std::string GetApplicationTitle()
	{
		return WindowTitle;
	}
	Vector2ui GetWindowResolution()
	{
		return WindowResolution;
	}
	void SetWindowResolution(Vector2ui NewResolution)
	{
		AspectRatio = (float)NewResolution.X / (float)NewResolution.Y;
		WindowResolution = NewResolution;
		OnResizedCallback();
		UIBox::ForceUpdateUI();
	}

	std::string ShaderPath = "Shaders";

	void SetShaderPath(std::string NewPath)
	{
		ShaderPath = NewPath;
	}

	const std::string& GetShaderPath()
	{
		return ShaderPath;
	}

	void SetErrorMessageCallback(void(*Callback)(std::string))
	{
		ErrorMessageCallback = Callback;
	}

	void SetOnWindowResizedCallback(void(*Callback)())
	{
		OnResizedCallback = Callback;
	}

	void Error(std::string Message)
	{
		ErrorMessageCallback(Message);
	}

	void(*GetErrorMessageCallback(std::string))(std::string)
	{
		return ErrorMessageCallback;
	}

	float Timer::TimeSinceCreation()
	{
		Uint64 PerfCounterFrequency = SDL_GetPerformanceFrequency();
		Uint64 EndCounter = SDL_GetPerformanceCounter();
		Uint64 counterElapsed = EndCounter - Time;
		float Elapsed = ((float)counterElapsed) / ((float)PerfCounterFrequency);
		return Elapsed;
	}
	Timer::Timer()
	{
		Time = SDL_GetPerformanceCounter();
	}

	SDL_Cursor** SystemCursors;

	void SetActiveMouseCursor(MouseCursorType NewType)
	{
		SDL_SetCursor(SystemCursors[NewType]);
	}
	MouseCursorType GetMouseCursorFromHoveredButtons()
	{
		if (dynamic_cast<UIButton*>(UI::HoveredBox))
		{
			return CURSOR_HAND;
		}
		if (dynamic_cast<UITextField*>(UI::HoveredBox))
		{
			return CURSOR_TEXT_HOVER;
		}
		return CURSOR_NORMAL;
	}
}

void HandleEvents()
{
	int x;
	int y;
	Application::NewTypedText.clear();
	SDL_GetMouseState(&x, &y);
	Input::MouseLocation = Vector2f((x / (float)Application::WindowResolution.X - 0.5f) * 2, 1 - (y / (float)Application::WindowResolution.Y * 2));
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		Input::MouseMovement = Vector2f();
		switch (e.type)
		{
		default:
			break;
		case SDL_WINDOWEVENT:
			if (e.window.event == SDL_WINDOWEVENT_CLOSE)
			{
				Application::Quit = true;
			}
			if (e.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				Application::SetWindowResolution(Vector2ui(e.window.data1, e.window.data2));
			}
			break;
		case SDL_MOUSEMOTION:
			Input::MouseMovement += Vector2f(e.motion.xrel / 12.f, -e.motion.yrel / 12.f);
			break;
		case SDL_KEYDOWN:
			Input::SetKeyDown(e.key.keysym.sym, true);
			switch (e.key.keysym.sym)
			{
			case SDLK_LEFT:
				if (TextInput::TextIndex == 0)
				{
					TextInput::TextRow--;
				}
				TextInput::TextIndex = std::max(std::min(TextInput::TextIndex - 1, (int)TextInput::Text.size()), 0);
				break;
			case  SDLK_RIGHT:
				if (TextInput::TextIndex + 1 > TextInput::Text.size())
				{
					TextInput::TextRow++;
				}
				TextInput::TextIndex = std::max(std::min(TextInput::TextIndex + 1, (int)TextInput::Text.size()), 0);
				break;
			case SDLK_DOWN:
				TextInput::TextRow++;
				break;
			case SDLK_UP:
				TextInput::TextRow--;
				break;
			case SDLK_DELETE:
				if (TextInput::TextIndex >= TextInput::Text.size())
				{
					TextInput::DeletePresses++;
				}
				TextInput::TextIndex++;
			case SDLK_BACKSPACE:
				if(TextInput::TextIndex == 0)
					TextInput::BackspacePresses++;
				TextInput::TextIndex = std::min((size_t)TextInput::TextIndex, TextInput::Text.size());
				if (TextInput::PollForText && TextInput::Text.length() > 0)
				{
					if (TextInput::TextIndex == TextInput::Text.size())
					{
						TextInput::Text.pop_back();
					}
					else if (TextInput::TextIndex > 0)
					{
						TextInput::Text.erase(TextInput::TextIndex - 1, 1);
					}
					TextInput::TextIndex = std::max(std::min(TextInput::TextIndex - 1, (int)TextInput::Text.size()), 0);
				}
				break;
			case SDLK_TAB:
				TextInput::TextIndex = std::min((size_t)TextInput::TextIndex, TextInput::Text.size());
				TextInput::Text.insert(TextInput::TextIndex, std::string("	"));
				TextInput::TextIndex += 1;
				break;
			case SDLK_ESCAPE:
				TextInput::PollForText = false;
				break;
			case SDLK_RETURN:
				TextInput::ReturnPresses++;
				TextInput::PollForText = false;
				break;
			case SDLK_v:
				if (Input::IsKeyDown(SDLK_LCTRL) || Input::IsKeyDown(SDLK_RCTRL))
				{
					std::string ClipboardText = SDL_GetClipboardText();
					std::string FilteredClipboardText;
					std::set<char> UnwantedClipboardCharacters = {'\r', '\0'}; // Windwos supplies newlines in \r\n. The text editor expects \n.
					for (auto i : ClipboardText)
					{
						if (!UnwantedClipboardCharacters.contains(i))
						{
							FilteredClipboardText.append({ i });
						}
					}
					TextInput::Text.insert(TextInput::TextIndex, FilteredClipboardText);
					if (FilteredClipboardText.find('\n') != std::string::npos)
					{
						TextInput::TextIndex = FilteredClipboardText.size() - FilteredClipboardText.find_last_of('\n') - 1;
					}
					else
					{
						TextInput::TextIndex += FilteredClipboardText.size();
					}
					Application::NewTypedText = FilteredClipboardText;
					TextInput::NumPastes++;
				}
				break;
			}
			break;
		case SDL_KEYUP:
			Input::SetKeyDown(e.key.keysym.sym, false);
			break;
		case SDL_MOUSEBUTTONDOWN:
			switch (e.button.button)
			{
			case SDL_BUTTON_RIGHT:
				Input::IsRMBDown = true;
				TextInput::PollForText = false;
				break;
			case SDL_BUTTON_LEFT:
				Input::IsLMBDown = true;
				break;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			switch (e.button.button)
			{
			case SDL_BUTTON_RIGHT:
				Input::IsRMBDown = false;
				break;
			case SDL_BUTTON_LEFT:
				Input::IsLMBDown = false;
				break;
			}
		case SDL_TEXTINPUT:
			if (TextInput::PollForText && e.text.text[0] >= 32 && e.text.text[0] <= 128)
			{
				TextInput::TextIndex = std::min((size_t)TextInput::TextIndex, TextInput::Text.size());
				TextInput::Text.insert(TextInput::TextIndex, std::string(e.text.text));
				TextInput::TextIndex += strlen(e.text.text);
				Application::NewTypedText = std::string(e.text.text);
			}
			break;
		case SDL_MOUSEWHEEL:
			for (ScrollObject* s : ScrollObject::GetAllScrollObjects())
			{
				if (e.wheel.y < 0)
					s->ScrollUp();
				else
					s->ScrollDown();
			}
			break;
		}
	}
}

void DrawUI()
{
	bool RedrawAfter = UIBox::DrawAllUIElements() || Application::PreviousHasWindowFocus != Application::GetWindowHasFocus();

	if (!RedrawAfter) return;

	Application::PreviousHasWindowFocus = Application::GetWindowHasFocus();

	Application::PostProcessShader->Bind();
	glViewport(0, 0, (GLsizei)Application::WindowResolution.X, (GLsizei)Application::WindowResolution.Y);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, UIBox::GetUIFramebuffer());
	Application::PostProcessShader->SetInt("u_ui", 0);
	Application::PostProcessShader->SetInt("u_hasWindowBorder", !Application::GetFullScreen() && Application::IsBorderless);
	Application::PostProcessShader->SetVec2("u_screenRes", Application::WindowResolution);
	Application::PostProcessShader->SetVec3("u_borderColor", Application::GetWindowHasFocus() ? Application::BorderlessWindowOutlineColor : 0.5);

	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	Application::PostProcessShader->Unbind();

	SDL_GL_SetSwapInterval(0);
	SDL_GL_SwapWindow(Application::Window);
}

void Application::UpdateWindow()
{
	Timer Frametime;
	HandleEvents();

	for (auto& e : ButtonEvents)
	{
		if (e.Function)
			e.Function();
		else if (e.FunctionIndex)
			e.FunctionIndex(e.Index);
	}
	ButtonEvents.clear();

	DrawUI();
	
	float DesiredDelta = 1.f / (float)DesiredRefreshRate;
	float TimeToWait = std::max(DesiredDelta - Frametime.TimeSinceCreation(), 0.f);
	SDL_Delay((int)(TimeToWait * 1000.f));
	DeltaTime = 1.f / DesiredRefreshRate;
	Time += DeltaTime;
}

bool Application::GetWindowHasFocus()
{
	return SDL_GetKeyboardFocus() == Window;
}

bool Application::GetFullScreen()
{
	auto flag = SDL_GetWindowFlags(Window);
	return flag & SDL_WINDOW_MAXIMIZED;
}

void Application::SetWindowMovableCallback(bool(*NewFunction)())
{
	IsMovableCallback = NewFunction;
}

void Application::SetBorderlessWindowOutlineColor(Vector3f32 NewColor)
{
	BorderlessWindowOutlineColor = NewColor;
}

void Application::Minimize()
{
	SDL_MinimizeWindow(Window);
}

void Application::SetFullScreen(bool NewFullScreen)
{
	if (NewFullScreen)
	{
		SDL_MaximizeWindow(Window);
	}
	else
	{
		SDL_RestoreWindow(Window);
	}
}

void Application::SetMinWindowSize(Vector2ui NewSize)
{
	SDL_SetWindowMinimumSize(Window, NewSize.X, NewSize.Y);
}

void Application::SetClipboard(std::string NewClipboardText)
{
	SDL_SetClipboardText(NewClipboardText.c_str());
}

const std::string& Application::GetNewTypedText()
{
	return NewTypedText;
}

void Application::Initialize(std::string WindowName, int Flags, Vector2ui DefaultResolution)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
	{
		Application::Error("SDL_Init failed: " + std::string(SDL_GetError()));
		exit(1);
	}
	SDL_SetHint("SDL_BORDERLESS_WINDOWED_STYLE", "1");
	SDL_SetHint("SDL_BORDERLESS_RESIZABLE_STYLE", "1");

	SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(0, &dm);
	DesiredRefreshRate = dm.refresh_rate;
	if (DefaultResolution == 0)
	{
		WindowResolution = Vector2ui((uint64_t)(dm.w / 1.5f), (uint64_t)(dm.h / 1.5f));
	}
	else
	{
		WindowResolution = DefaultResolution;
	}

	AspectRatio = (float)WindowResolution.X / (float)WindowResolution.Y;
	int WindowFlags = SDL_WINDOW_OPENGL;
	if (!(Flags & NO_RESIZE_BIT))
	{
		WindowFlags |= SDL_WINDOW_RESIZABLE;
	}
	if (Flags & ALWAYS_TOP_BIT)
	{
		WindowFlags |= SDL_WINDOW_ALWAYS_ON_TOP;
	}
	Window = SDL_CreateWindow(WindowName.c_str(),
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WindowResolution.X, WindowResolution.Y, WindowFlags);

	if (!Window)
	{
		Application::Error("SDL2 failed to create a window: " + std::string(SDL_GetError()));
	}

	if (Flags & MAXIMIZED_BIT)
	{
		SDL_MaximizeWindow(Window);
	}
	if (Flags & BORDERLESS_BIT)
	{
		IsBorderless = true;
		SDL_SetWindowBordered(Window, SDL_FALSE);
		SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");
		if (!(Flags & NO_RESIZE_BIT))
		{
			SDL_SetWindowHitTest(Window, HitTestCallback, 0);
		}
	}

	SystemCursors = new SDL_Cursor*[4]
	{
		SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW),
		SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND),
		SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR),
		SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM),
	};

	auto GLContext = SDL_GL_CreateContext(Window);

	if (!GLContext)
	{
		Application::Error("SDL_GL_CreateContext failed: " + std::string(SDL_GetError()));
	}

	const auto GlewInitErr = glewInit();

	if (GlewInitErr != GLEW_OK)
	{
		Application::Error(" glewInit() failed: " + std::string((const char*)glewGetErrorString(GlewInitErr)));
		exit(1);
		return;
	}

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDebugMessageCallback(MessageCallback, 0);
	SetMinWindowSize(Vector2ui(640, 480));

	PostProcessShader = new Shader(Application::GetShaderPath() + "/postprocess.vert", Application::GetShaderPath() + "/postprocess.frag");
	UIBox::InitUI();
}


