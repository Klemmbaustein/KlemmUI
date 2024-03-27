#pragma once
#include <string>

/**
* @file
*/

/**
* @brief
* Containing functions for the application.
*/
namespace KlemmUI::Application
{
	std::string GetShaderPath();

	/**
	 * @brief
	 * Sets the application shader path.
	 */
	void SetShaderPath(std::string NewShaderPath);

	/**
	 * @brief
	 * Initializes the library.
	 * 
	 * @param ShaderPath
	 * The location where the KlemmUI shader files are located.
	 * This value can be changed with KlemmUI::Application::SetShaderPath()
	 */
	void Initialize(std::string ShaderPath);
	
	/**
	 * @brief
	 * Application error handling.
	 */
	namespace Error
	{
		/**
		 * @brief
		 * Function used for error messages.
		 * 
		 * This function calls the function set with KlemmUI::Application::Error::SetErrorMessageCallback().
		 * 
		 * @param Message
		 * The error message.
		 * 
		 * @param Fatal
		 * If this is true, the program will crash after displaying the message.
		 */
		void Error(std::string Message, bool Fatal = false);

		/**
		 * @brief
		 * Sets the error message callback.
		 * 
		 * The default callback will print the error message to standard output.
		 * 
		 * On windows, you might want this message to call `CreateMessageBoxA()` or something similar.
		 */
		void SetErrorCallback(void(*Callback)(std::string Message));
	}
}