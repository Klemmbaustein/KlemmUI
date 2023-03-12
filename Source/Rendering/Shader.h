#pragma once
#include <string>
#include <Math/Vector3.h>
#include <Math/Vector2.h>

class Shader
{
	unsigned int ShaderID = 0;
	void CheckCompileErrors(unsigned int ShaderID, std::string Type);
public:
	unsigned int GetShaderID();

	Shader(const char* VertexPath, const char* FragmentPath, const char* GeometryPath = nullptr);
	~Shader();
	void Bind();
	void Unbind();
	void SetBool(const std::string& Name, bool Value);
	void SetInt(const std::string& Name, int Value);
	void SetFloat(const std::string& Name, float Value);
	void SetVec2(const std::string& Name, Vector2f Value);
	void SetVec3(const std::string& Name, Vector3f Value);
};