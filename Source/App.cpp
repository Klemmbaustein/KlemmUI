#include <kui/App.h>
#include "SystemWM/SystemWM.h"

static std::function<void(std::string Message, bool IsFatal)> ErrorCallback = 
	[](std::string Message, bool)
	{
		std::fputs(Message.c_str(), stderr);
	};

void kui::app::MessageBox(std::string Text, std::string Title, MessageBoxType Type)
{
	systemWM::MessageBox(Text, Title, int(Type));
}

void kui::app::error::Error(std::string Message, bool Fatal)
{
	ErrorCallback(Message, Fatal);
	if (Fatal)
	{
		abort();
	}
}

void kui::app::error::SetErrorCallback(std::function<void(std::string Message, bool IsFatal)> Callback)
{
	ErrorCallback = Callback;
}

bool kui::app::YesNoBox(std::string Text, std::string Title)
{
	return systemWM::YesNoBox(Text, Title);
}

std::string kui::app::SelectFileDialog(bool PickFolders)
{
	return systemWM::SelectFileDialog(PickFolders);
}
