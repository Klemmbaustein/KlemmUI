#pragma once
#include <string>
#include <functional>

/**
* @file
*/

/**
* @brief
* Containing functions for the application.
*/
namespace kui::app
{
	enum class MessageBoxType
	{
		Info,
		Warn,
		Error,
	};

	void MessageBox(std::string Text, std::string Title, MessageBoxType Type);

	bool YesNoBox(std::string Text, std::string Title);

	std::string SelectFileDialog(bool PickFolders);

	/**
	 * @brief
	 * Application error handling.
	 */
	namespace error
	{
		/**
		 * @brief
		 * Function used for error messages.
		 * 
		 * This function calls the function set with kui::app::error::SetErrorMessageCallback().
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
		 * On windows, you might want this message to call `MessageBox()` or something similar.
		 */
		void SetErrorCallback(std::function<void(std::string Message, bool IsFatal)> Callback);
	}
}