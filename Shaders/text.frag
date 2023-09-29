#version 330 
in vec2 TexCoords;
in vec2 v_position;
in vec3 v_color;
out vec4 color;
uniform vec3 u_offset; //X = Y offset; Y = MaxDistance; Z MinDistance
uniform sampler2D u_texture;
uniform vec3 textColor;
uniform vec2 u_texSize;
uniform float u_opacity = 1.0f;

#define NUM_SAMPLES 2

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
	float sampled = 0;

	vec2 offset = 2 / u_texSize;

	for (int x = 0; x < NUM_SAMPLES; x++)
	{
		for (int y = 0; y < NUM_SAMPLES; y++)
		{
			sampled += texture(u_texture, TexCoords + (vec2(x, y) / (NUM_SAMPLES)) * offset).a;
		}
	}

	color = vec4(v_color, sampled / pow(NUM_SAMPLES, 2) * u_opacity);
}  