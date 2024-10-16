#version 330

in vec2 v_texCoords;

out vec4 f_color;

uniform sampler2D u_background;
uniform vec2 u_pixelSize;
uniform vec2 u_scale;
uniform vec2 u_position;

void main()
{
	float it = 0;
	vec2 end = u_pixelSize * 6;
	vec2 iter = u_pixelSize / 2;
	f_color.xyz = vec3(0);

	vec2 startPos = u_position;
	vec2 endPos = u_position + u_scale;
	for (float x = -end.x; x < end.x; x += iter.x)
	{
		for (float y = -end.y; y < end.y; y += iter.y)
		{
			vec2 pos = v_texCoords + vec2(x, y);

			if (pos.x < startPos.x || pos.y < startPos.y)
				continue;

			if (pos.x > endPos.x || pos.y > endPos.y)
				continue;
			
			f_color.xyz += texture(u_background, pos).xyz;
			it++;
		}
	}
	f_color.xyz /= it;
	f_color.w = 1;
}