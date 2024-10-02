#version 330

in vec2 v_texCoords;

out vec4 f_color;

uniform sampler2D u_background;
uniform vec2 u_pixelSize;

void main()
{
	float it = 0;
	vec2 end = u_pixelSize * 4;
	vec2 iter = u_pixelSize;
	for (float x = -end.x; x < end.x; x += iter.x)
	{
		for (float y = -end.y; y < end.y; y += iter.y)
		{
			f_color.xyz += texture(u_background, v_texCoords + vec2(x, y)).xyz;
			it++;
		}
	}
	f_color.xyz /= it;
	f_color.w = 1;
}