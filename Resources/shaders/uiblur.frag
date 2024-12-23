#version 330

in vec2 v_texCoords;

out vec4 f_color;

uniform sampler2D u_background;
uniform vec2 u_scale;
uniform vec2 u_position;
uniform bool u_horizontal = false;

uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
	//vec2 end = u_pixelSize * 6;
	//vec2 iter = u_pixelSize / 2;

	//vec2 startPos = u_position;
	//vec2 endPos = u_position + u_scale;
	vec2 tex_offset = 1.0 / textureSize(u_background, 0); // gets size of single texel
	f_color.xyz = texture(u_background, v_texCoords).rgb;// * weight[0]; // current fragment's contribution
	f_color.w = 1;
	if(u_horizontal)
	{
		for(int i = 1; i < 5; ++i)
		{
			f_color.xyz += texture(u_background, v_texCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
			f_color.xyz += texture(u_background, v_texCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
		}
	}
	else
	{
		for(int i = 1; i < 5; ++i)
		{
			f_color.xyz += texture(u_background, v_texCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
			f_color.xyz += texture(u_background, v_texCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
		}
	}
}