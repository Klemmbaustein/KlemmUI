#include <KlemmUI/Rendering/ShaderManager.h>
#include <KlemmUI/Application.h>
#include <KlemmUI/Resource.h>
using namespace KlemmUI;

KlemmUI::ShaderManager::~ShaderManager()
{
	for (auto& i : Shaders)
	{
		delete i.second;
	}
	Shaders.clear();
}

Shader* KlemmUI::ShaderManager::LoadShader(std::string VertexName, std::string FragmentName, std::string Name)
{
	auto LoadedShader = Shaders.find(Name);

	if (LoadedShader == Shaders.end())
	{
		Shader* NewShader = new Shader(Resource::GetStringFile(VertexName), Resource::GetStringFile(FragmentName));
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
