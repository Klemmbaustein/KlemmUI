#pragma once
#include <string>
#include "Vector2.h"
#include "Input.h"
#include "Timer.h"
#include <vector>
#include <atomic>

namespace KlemmUI
{
	/**
	* @brief
	* A KlemmUI window.
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
		std::atomic<bool> ShouldUpdate = false;
		static std::vector<Window*> ActiveWindows;


	public:
		
		void OnResized();

		/**
		* @brief
		* Gets a list of all currently active windows.
		*/
		static const std::vector<Window*>& GetActiveWindows();

		/**
		* @brief
		* Target framerate of the window.
		* A value of 0 uses the framerate of the window's monitor.
		*/
		uint32_t TargetFPS = 0;

		/**
		* @brief
		*/
		float Time = 0;

		void Close();

		/**
		* @brief
		* Returns the time between the last two calls to UpdateWindow.
		*/
		float GetDeltaTime() const;

		InputManager Input;

		static const Vector2ui POSITION_CENTERED;

		Window(std::string Name, Vector2ui WindowPos, Vector2ui WindowSize);
		virtual ~Window();

		static Window* GetActiveWindow();

		void SetAsActiveWindow();
		static void ClearActiveWindow();

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

		float GetAspectRatio() const;
		Vector2ui GetSize() const;
	};
}