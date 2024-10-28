#version 330

in vec2 v_texcoords;
in vec2 v_position;
in float v_cornerIndex;
layout (location = 0) out vec4 f_color;
layout (location = 1) out vec4 f_alpha;

uniform vec3 u_color = vec3(1);
uniform vec3 u_backgroundColor = vec3(0);
uniform vec3 u_offset; // Scroll bar: X = scrolled distance; Y = MaxDistance; Z MinDistance
uniform float u_opacity = 1;
uniform float u_time = 0;

float smoothSelect(float a, float b, float value, float smoothness)
{
	return mix(a, b, clamp((value + (1 / smoothness)) * smoothness, 0, 1));
}

void main()
{	
	if (u_offset.y > v_position.y)
	{
		discard;
	}
	if (u_offset.z < v_position.y)
	{
		discard;
	}

	vec2 centeredCoords = (v_texcoords - 0.5) * 2;

	if (centeredCoords == vec2(0))
		discard;
	vec2 normalizedCoords = normalize(centeredCoords);

	float dist = length(centeredCoords);

	float opacity = smoothSelect(0, smoothSelect(1, 0, dist - 1, 20), dist - 0.75, 20);

	if (opacity < 0)
		discard;

	float spinTime = mod(u_time * 2, 2);
	float angle;
	if (spinTime < 1)
	{
		angle = (asin(normalizedCoords.y) / 3.141) + 0.5;
		if (centeredCoords.x < 0)
		{
			angle = 1 - angle + 1;
		}
	}
	else
	{
		angle = (asin(-normalizedCoords.y) / 3.141) + 0.5;	
		if (centeredCoords.x > 0)
		{
			angle = 1 - angle + 1;
		}
		angle += 1;
	}

	float value = smoothSelect(1, 0, (angle - spinTime - abs(sin(u_time * 2) / 4)) - 0.25, 50);
	value *= smoothSelect(0, 1, angle - spinTime - 0.01, 10);

	f_color = vec4(mix(u_backgroundColor, u_color, value), opacity);
	f_alpha.xyz = vec3(1);
	f_alpha.w = f_color.w;
}