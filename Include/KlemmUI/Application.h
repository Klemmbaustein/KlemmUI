#pragma once
#include <string>

namespace KlemmUI::Application
{
	std::string GetShaderPath();
	void SetShaderPath(std::string NewShaderPath);

	void Initialize(std::string ShaderPath);

	namespace Error
	{
		void Error(std::string Message, bool Fatal = false);
	}
}