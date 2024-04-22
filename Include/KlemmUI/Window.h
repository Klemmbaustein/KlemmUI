#pragma once
#include <string>
#include "Vector2.h"
#include "Input.h"
#include "Timer.h"
#include <vector>
#include <atomic>
#include "Rendering/ShaderManager.h"
#include "UI/UIManager.h"


namespace KlemmUI
{
	class UIBox;
	
	/**
	* @brief
	* A KlemmUI window.
	* 
	* Every new UI element will be added to the active window. By default, the active window is the window that was last updated or created.
	* The active window can be set using the KlemmUI::Window::SetWindowActive() function.
	* The active window is thread local. On a different thread, another window might be active.
	* 
	* Everything related to a window should be done on the thread it was created on.
	*/
	class Window
	{
		void* SDLWindowPtr = nullptr;

		Vector2ui WindowSize;

		void UpdateSize();
		Timer WindowDeltaTimer;

		float FrameDelta = 0;

		void WaitFrame();

		std::atomic<bool> ShouldClose = false;
		std::atomic<bool> ShouldUpdateSize = false;
		static std::vector<Window*> ActiveWindows;

		void* GLContext = nullptr;
	public:
		
		/**
		 * @brief
		 * Sets this window as the active window.
		 * 
		 * Any new UI elements will be added to the active window.
		 */
		void SetWindowActive();

		void OnResized();

		/**
		* @brief
		* Sets a minimum size for the window.
		*/
		void SetMinSize(Vector2ui MinimumSize);
		/**
		* @brief
		* Sets a maximum size for the window.
		*/
		void SetMaxSize(Vector2ui MaximumSize);

		/**
		* @brief
		* Gets a list of all currently active windows.
		*/
		static const std::vector<Window*>& GetActiveWindows();

		/**
		 * @brief
		 * Either maximizes or restores the window.
		 */
		void SetWindowFullScreen(bool NewIsFullScreen);

		/// Returns if the window is maximized.
		bool GetWindowFullScreen();

		void SetMinimized(bool NewIsMinimized);
		bool GetMinimized();

		/**
		* @brief
		* Target framerate of the window.
		* A value of 0 uses the framerate of the window's monitor.
		*/
		uint32_t TargetFPS = 0;

		/**
		* @brief
		* The time elapsed since the Window was created.
		*/
		float Time = 0;

		/**
		* @brief
		* Closes the window.
		* 
		* This function can be called from any thread.
		*/
		void Close();

		/**
		* @brief
		* Returns the time between the last two calls to UpdateWindow.
		*/
		float GetDeltaTime() const;

		/// The input manager of this window.
		InputManager Input = InputManager(this);
		/// The shader manager of this window.
		ShaderManager Shaders;
		/// The ui manager of this window.
		UIManager UI;

		/// Centered window position.
		static const Vector2ui POSITION_CENTERED;

		/// Default window size. Window has 60% of the screen's resolution.
		static const Vector2ui SIZE_DEFAULT;

		/**
		* @brief
		* Flags for window construction.
		*/
		enum class WindowFlag
		{
			///No window flags.
			None           = 0b00000,
			/// Borderless window.
			Borderless     = 0b00001,
			/// The window is resizable.
			Resizable      = 0b00010,
			/// The window should appear on top of all other windows.
			AlwaysOnTop    = 0b00100,
			/// The window should start maximized.
			FullScreen     = 0b01000,
			/// The window is a tooltip.
			Tooltip    = 0b10000,
		};

		/**
		* @brief
		* Constructs a window with the given parameters
		*/
		Window(std::string Name, WindowFlag Flags, Vector2ui WindowPos = POSITION_CENTERED, Vector2ui WindowSize = SIZE_DEFAULT);
		virtual ~Window();

		/**
		 * @brief
		 * Gets the currently active window.
		 * 
		 * The active window is thread-local. Each thread can have a different active window.
		 */
		static Window* GetActiveWindow();

		void* GetSDLWindowPtr() const;

		/**
		* @brief
		* Updates the current window, re-draws the screen if necessary.
		* 
		* ```
		* while (MyWindow.UpdateWindow())
		* {
		*     // ...
		* }
		* ```
		* 
		* @return
		* Returns if the window should continue being shown.
		*/
		bool UpdateWindow();

		/**
		* @brief
		* Gets the window's aspect ratio.
		* 
		* Aspect ratio = Width / Height.
		*/
		float GetAspectRatio() const;

		bool HasFocus();

		/**
		* @brief
		* Gets the window size in pixels.
		*/
		Vector2ui GetSize() const;

		void SetPosition(Vector2ui Pos);

		/**
		* @brief
		* Sets the grabbable callback.
		* 
		* For a borderless window, the window manager has to know which area is grabbable by the mouse cursor, eg. the mouse cursor.
		*/
		bool(*IsAreaGrabbableCallback)(KlemmUI::Window* Target);
		void(*OnResizedCallback)(KlemmUI::Window* Target);

		/**
		* @brief
		* Sets the window flags.
		*/
		void SetWindowFlags(WindowFlag NewFlags);
		WindowFlag GetWindowFlags() const;
		void MakeContextCurrent();

		void CancelClose();

		/**
		 * @brief
		 * The border color used when the window is borderless.
		 */
		Vector3f BorderColor = 1;

		enum class Cursor
		{
			Default,
			Hand,
			Text,
			Cursor_End
		};

		Cursor CurrentCursor = Cursor::Default;

		/**
		 * @brief
		 * Sets the window's title.
		 */
		void SetTitle(std::string NewTitle);

		float DPIMultiplier = 1;

		float GetDPI() const;

	private:
		bool IsMainWindow = false;
		float DPI = 1;
		void UpdateDPI();
		void HandleCursor();
		void* Cursors[(int)Cursor::Cursor_End];
		int ToSDLWindowFlags(WindowFlag Flags);
		WindowFlag CurrentWindowFlags;
	};

	Window::WindowFlag operator|(Window::WindowFlag a, Window::WindowFlag b);
	Window::WindowFlag operator&(Window::WindowFlag a, Window::WindowFlag b);
}