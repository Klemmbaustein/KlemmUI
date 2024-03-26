#include <KlemmUI/Application.h>
#include <iostream>

namespace KlemmUI::Application
{
	static std::string ShaderPath;
}

std::string KlemmUI::Application::GetShaderPath()
{
	return ShaderPath;
}

void KlemmUI::Application::SetShaderPath(std::string NewShaderPath)
{
	ShaderPath = NewShaderPath;
}

void KlemmUI::Application::Error::Error(std::string Message, bool Fatal)
{
	std::puts(Message.c_str());
	if (Fatal)
	{
		abort();
	}
}
