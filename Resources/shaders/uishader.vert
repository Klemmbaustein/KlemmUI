//! #version 330

layout(location = 0) in vec2 a_position;
layout(location = 1) in float a_cornerIndex;
uniform vec2 u_scrollOffset;
uniform vec4 u_transform; // xy = position zw = scale
uniform vec2 u_screenRes;
out vec2 v_position;
out vec2 v_texcoords;
out float v_cornerIndex;

void main()
{
	v_texcoords = a_position;
	vec2 pixelPos = (v_texcoords * u_transform.zw + u_scrollOffset + u_transform.xy);
	gl_Position = vec4(pixelPos, 0, 1);
	v_position = gl_Position.xy;
	v_cornerIndex = a_cornerIndex;
}