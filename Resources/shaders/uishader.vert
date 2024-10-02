#version 330

layout(location = 0) in vec2 a_position;
layout(location = 1) in float a_cornerIndex;
uniform vec3 u_offset; //X = Y offset; Y = MaxDistance
uniform vec4 u_transform = vec4(vec2(0), vec2(1)); // xy = position zw = scale
uniform vec2 u_screenRes = vec2(1600, 900);
out vec2 v_position;
out vec2 v_texcoords;
out float v_cornerIndex;

void main()
{
	ivec2 pixelPos = ivec2((a_position.xy * u_transform.zw + vec2(0, -u_offset.x) + u_transform.xy) * u_screenRes);
	gl_Position = vec4(pixelPos / u_screenRes, 0, 1);
	v_texcoords = a_position;
	v_position = gl_Position.xy;
	v_cornerIndex = a_cornerIndex;
}