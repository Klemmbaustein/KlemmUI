#include <kui/Rendering/Shader.h>
#include "OpenGL.h"
#include <fstream>
#include <sstream>
#include <kui/App.h>
#include <iostream>

using namespace kui;

void Shader::CheckCompileErrors(unsigned int ShaderID, std::string Type)
{
	GLint success;
	GLchar infoLog[1024];
	if (Type != "PROGRAM")
	{
		glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(ShaderID, 1024, nullptr, infoLog);
			app::error::Error("Shader compilation error of type: "
				+ Type
				+ "\n"
				+ std::string(infoLog)
				+ "\n -- --------------------------------------------------- -- ", true);
		}
	}
	else
	{
		glGetProgramiv(ShaderID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(ShaderID, 1024, nullptr, infoLog);
			app::error::Error("Shader linking error of type: "
				+ Type
				+ "\n"
				+ std::string(infoLog)
				+ "\n -- --------------------------------------------------- -- ", true);
		}
	}
}

unsigned int Shader::GetShaderID()
{
	return ShaderID;
}

Shader::Shader(std::string VertexSource, std::string FragmentSource)
{
//#if KLEMMUI_WEB_BUILD
	VertexSource = "#version 300 es\nprecision highp float;\n" + VertexSource;
	FragmentSource = "#version 300 es\nprecision highp float;\n" + FragmentSource;
//#else
//	VertexSource = "#version 330\n" + VertexSource;
//	FragmentSource = "#version 330\n" + FragmentSource;
//#endif

	const char* vShaderCode = VertexSource.c_str();
	const char* fShaderCode = FragmentSource.c_str();
	// 2. compile shaders
	unsigned int vertex, fragment;

	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, nullptr);
	glCompileShader(vertex);
	CheckCompileErrors(vertex, "VERTEX");
	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, nullptr);
	glCompileShader(fragment);
	CheckCompileErrors(fragment, "FRAGMENT");
	// if geometry shader is given, compile geometry shader
	unsigned int geometry = 0;

	// shader Program
	ShaderID = glCreateProgram();
	glAttachShader(ShaderID, vertex);
	glAttachShader(ShaderID, fragment);

	glLinkProgram(ShaderID);
	CheckCompileErrors(ShaderID, "PROGRAM");
	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader::~Shader()
{
	glDeleteProgram(ShaderID);
}

void Shader::Bind()
{
	glUseProgram(ShaderID);
}

void Shader::Unbind()
{
	glUseProgram(0);
}

unsigned int Shader::GetUniformLocation(const std::string& Name) const
{
	auto Found = Uniforms.find(Name);
	if (Found != Uniforms.end())
		return Found->second;

	unsigned int Location = glGetUniformLocation(ShaderID, Name.c_str());

	Uniforms.insert({ Name, Location });

	return Location;
}


void Shader::SetBool(const std::string& Name, bool Value)
{
	glUniform1i(GetUniformLocation(Name), (int)Value);
}

void Shader::SetInt(const std::string& Name, int Value)
{
	glUniform1i(GetUniformLocation(Name), Value);
}

void Shader::SetFloat(const std::string& Name, float Value)
{
	glUniform1f(GetUniformLocation(Name), Value);
}

void Shader::SetVec2(const std::string& Name, Vec2f Value)
{
	glUniform2f(GetUniformLocation(Name), Value.X, Value.Y);
}

void Shader::SetVec3(const std::string& Name, Vec3f Value)
{
	glUniform3f(GetUniformLocation(Name), Value.X, Value.Y, Value.Z);
}
