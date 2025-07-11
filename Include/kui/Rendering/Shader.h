#pragma once
#include <string>
#include "../Vec2.h"
#include "../Vec3.h"
#include <map>

namespace kui
{
	class Shader
	{
	public:
		unsigned int GetShaderID();

		Shader(std::string VertexPath, std::string FragmentPath);
		~Shader();
		void Bind();
		void Unbind();
		void SetBool(const std::string& Name, bool Value);
		void SetInt(const std::string& Name, int Value);
		void SetFloat(const std::string& Name, float Value);
		void SetVec2(const std::string& Name, Vec2f Value);
		void SetVec3(const std::string& Name, Vec3f Value);
		unsigned int GetUniformLocation(const std::string& Name) const;
	private:
		unsigned int ShaderID = 0;
		mutable std::map<std::string, unsigned int> Uniforms;
		void CheckCompileErrors(unsigned int ShaderID, std::string Type);

	};
}