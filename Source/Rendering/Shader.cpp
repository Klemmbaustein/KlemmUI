#include <KlemmUI/Rendering/Shader.h>
#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <KlemmUI/Application.h>

using namespace KlemmUI;

void Shader::CheckCompileErrors(unsigned int ShaderID, std::string Type)
{
	GLint success;
	GLchar infoLog[1024];
	if (Type != "PROGRAM")
	{
		glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(ShaderID, 1024, NULL, infoLog);
			Application::Error::Error("Shader compilation error of type: "
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
			glGetProgramInfoLog(ShaderID, 1024, NULL, infoLog);
			Application::Error::Error("Shader linking error of type: "
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

Shader::Shader(std::string VertexSource, std::string FragmentSource, std::string GeometrySource)
{
	const char* vShaderCode = VertexSource.c_str();
	const char* fShaderCode = FragmentSource.c_str();
	// 2. compile shaders
	unsigned int vertex, fragment;
	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	CheckCompileErrors(vertex, "VERTEX");
	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	CheckCompileErrors(fragment, "FRAGMENT");
	// if geometry shader is given, compile geometry shader
	unsigned int geometry;
	if (!GeometrySource.empty())
	{
		const char* gShaderCode = GeometrySource.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		CheckCompileErrors(geometry, "GEOMETRY");
	}
	// shader Program
	ShaderID = glCreateProgram();
	glAttachShader(ShaderID, vertex);
	glAttachShader(ShaderID, fragment);
	if (!GeometrySource.empty())
		glAttachShader(ShaderID, geometry);
	glLinkProgram(ShaderID);
	CheckCompileErrors(ShaderID, "PROGRAM");
	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (!GeometrySource.empty())
		glDeleteShader(geometry);

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

void Shader::SetBool(const std::string& Name, bool Value)
{
	glUniform1i(glGetUniformLocation(ShaderID, Name.c_str()), (int)Value);
}

void Shader::SetInt(const std::string& Name, int Value)
{
	glUniform1i(glGetUniformLocation(ShaderID, Name.c_str()), Value);
}

void Shader::SetFloat(const std::string& Name, float Value)
{
	glUniform1f(glGetUniformLocation(ShaderID, Name.c_str()), Value);
}

void Shader::SetVec2(const std::string& Name, Vector2f Value)
{
	glUniform2f(glGetUniformLocation(ShaderID, Name.c_str()), Value.X, Value.Y);
}

void Shader::SetVec3(const std::string& Name, Vector3f Value)
{
	glUniform3f(glGetUniformLocation(ShaderID, Name.c_str()), Value.X, Value.Y, Value.Z);
}
