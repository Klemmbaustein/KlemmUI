#pragma once
#include <string>
#include "../Vector2.h"
#include "../Vector3.h"

class Shader
{
	unsigned int ShaderID = 0;
	void CheckCompileErrors(unsigned int ShaderID, std::string Type);
public:
	unsigned int GetShaderID();

	Shader(std::string VertexPath, std::string FragmentPath, std::string GeometryPath = "");
	~Shader();
	void Bind();
	void Unbind();
	void SetBool(const std::string& Name, bool Value);
	void SetInt(const std::string& Name, int Value);
	void SetFloat(const std::string& Name, float Value);
	void SetVec2(const std::string& Name, Vector2f Value);
	void SetVec3(const std::string& Name, Vector3f Value);
};