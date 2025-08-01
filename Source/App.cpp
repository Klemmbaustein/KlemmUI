#include <kui/App.h>
#include "SystemWM/SystemWM.h"
#include <iostream>

static std::function<void(std::string Message, bool IsFatal)> ErrorCallback = 
	[](std::string Message, bool)
	{
		std::cerr << Message << std::endl;
	};

void kui::app::MessageBox(std::string Text, std::string Title, MessageType Type)
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
