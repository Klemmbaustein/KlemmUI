#version 330
layout(location = 0) in vec2 a_position;
layout(location = 1) in float a_cornerIndex;

out vec2 v_texCoords;

uniform vec2 u_scale;
uniform vec2 u_position;

void main()
{
	gl_Position = vec4((a_position - 0.5) * 2, 0, 1);
	v_texCoords = a_position * u_scale + u_position;
}