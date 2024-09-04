#pragma once
#include <string>
#include "Shader.h"
#include <unordered_map>

namespace KlemmUI
{
	class ShaderManager
	{
		std::unordered_map<std::string, Shader*> Shaders;
	public:
		~ShaderManager();

		Shader* LoadShader(std::string VertexName, std::string FragmentName, std::string Name);

		Shader* GetShader(std::string Name);
	};
}