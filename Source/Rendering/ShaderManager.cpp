#include <KlemmUI/Rendering/ShaderManager.h>
#include <KlemmUI/Application.h>

Shader* KlemmUI::ShaderManager::LoadShader(std::string VertexName, std::string FragmentName, std::string Name)
{
	auto LoadedShader = Shaders.find(Name);

	if (LoadedShader == Shaders.end())
	{
		Shader* NewShader = new Shader(Application::GetShaderPath() + "/" + VertexName, Application::GetShaderPath() + "/" + FragmentName);
		Shaders.insert(std::pair(Name, NewShader));
		return NewShader;
	}
	else
	{
		return LoadedShader->second;
	}
}

Shader* KlemmUI::ShaderManager::GetShader(std::string Name)
{
	return Shaders.at(Name);
}
