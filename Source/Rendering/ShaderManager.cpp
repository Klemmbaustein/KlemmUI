#include <kui/Rendering/ShaderManager.h>
#include <kui/App.h>
#include <kui/Resource.h>
using namespace kui;

kui::ShaderManager::~ShaderManager()
{
	for (auto& i : Shaders)
	{
		delete i.second;
	}
	Shaders.clear();
}

Shader* kui::ShaderManager::LoadShader(std::string VertexName, std::string FragmentName, std::string Name)
{
	auto LoadedShader = Shaders.find(Name);

	if (LoadedShader == Shaders.end())
	{
		Shader* NewShader = new Shader(resource::GetStringFile(VertexName), resource::GetStringFile(FragmentName));
		Shaders.insert(std::pair(Name, NewShader));
		return NewShader;
	}
	else
	{
		return LoadedShader->second;
	}
}

Shader* kui::ShaderManager::GetShader(std::string Name)
{
	return Shaders.at(Name);
}
