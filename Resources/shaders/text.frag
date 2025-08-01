//! #version 330
in vec2 TexCoords;
in vec2 v_position;
in vec3 v_color;
layout (location = 0) out vec4 color;
layout (location = 1) out vec4 alpha;
uniform vec3 u_offset; //X = Y offset; Y = MaxDistance; Z MinDistance
uniform sampler2D u_texture;
uniform vec3 textColor;
uniform float u_opacity;
uniform vec2 u_screenRes;
uniform vec3 transform;
#define NUM_SAMPLES 3

void main()
{
	if(u_offset.y > v_position.y)
	{
		discard;
	}
	if(u_offset.z < v_position.y)
	{
		discard;
	}
	float sampled = 0.0;
	vec2 offset = (0.8 / transform.z) / u_screenRes;
	for (int x = -NUM_SAMPLES; x < NUM_SAMPLES; x++)
	{
		for (int y = -NUM_SAMPLES; y < NUM_SAMPLES; y++)
		{
			sampled += texture(u_texture, TexCoords + offset * vec2(x, y)).a;
		}
	}
	sampled /= float(NUM_SAMPLES) * float(NUM_SAMPLES) * 2.0 * 2.0;
	color = vec4(v_color, sampled * u_opacity);
	alpha.xyz = vec3(1);
	alpha.w = color.w;
}