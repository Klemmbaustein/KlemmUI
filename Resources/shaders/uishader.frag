//! #version 330

in vec2 v_texcoords;
in vec2 v_position;
in float v_cornerIndex;
layout (location = 0) out vec4 f_color;
layout (location = 1) out vec4 f_alpha;

uniform vec3 u_color;
uniform vec3 u_borderColor;
uniform int u_useTexture;
uniform sampler2D u_texture;
uniform vec3 u_offset; // Scroll bar: X = scrolled distance; Y = MaxDistance; Z MinDistance
uniform float u_opacity;
uniform bool u_drawBorder;
uniform bool u_drawCorner;
uniform float u_borderScale;
uniform float u_cornerScale;
uniform vec4 u_transform;
uniform float u_aspectRatio;
uniform vec2 u_screenRes;
uniform int u_cornerFlags;
uniform int u_borderFlags;

#define NUM_SAMPLES 2

bool isBorderVisible(int index)
{
	return (u_borderFlags & (1 << index)) != 0;
}

void main()
{
	vec2 scale = u_transform.zw * vec2(u_aspectRatio, 1.0);
	vec2 scaledTexCoords = v_texcoords * scale + (0.025 / u_screenRes);
	vec2 nonAbsCenteredTexCoords = (scaledTexCoords - scale / 2.0) * 2.0;
	vec2 centeredTexCoords = abs(nonAbsCenteredTexCoords);
	
	int cornerIndex = int(round(v_cornerIndex));
	
	if (u_offset.y > v_position.y)
	{
		discard;
	}
	if (u_offset.z < v_position.y)
	{
		discard;
	}
	if (u_useTexture == 1)
	{
		vec4 sampled = vec4(0.0);
		vec2 offset = (0.5 / scale) / u_screenRes;
		int samples = 0;
		for (int x = -NUM_SAMPLES; x < NUM_SAMPLES; x++)
		{
			for (int y = -NUM_SAMPLES; y < NUM_SAMPLES; y++)
			{
				vec4 newS = texture(u_texture, v_texcoords + offset * vec2(x, y));
				if (newS.a > 0.0)
				{
					sampled.xyz += newS.xyz;
					samples++;
				}
				sampled.w += newS.w;
			}
		}
		sampled.xyz /= float(samples);
		sampled.w /= float(NUM_SAMPLES * NUM_SAMPLES) * 2.0 * 2.0;
		f_color = vec4(clamp(u_color * sampled.rgb, vec3(0.0), vec3(1.0)), u_opacity);
		f_color.a *= sampled.w;
	}
	else
	{
		f_color = vec4(u_color, u_opacity);
	}

	if (u_drawCorner && (u_cornerFlags & (1 << cornerIndex)) != 0
		&& (centeredTexCoords.y >= scale.y - u_cornerScale) && (centeredTexCoords.x >= scale.x - u_cornerScale))
	{
		float borderSize = pow((length((scale - u_cornerScale) - centeredTexCoords) / u_cornerScale), u_cornerScale * 1000.0);
		f_color.a *= clamp(1.0 / borderSize, 0.0, 1.0);

		if (u_drawBorder && u_cornerScale > u_borderScale)
		{
			float cornerDistance = (length((scale - u_cornerScale) - centeredTexCoords));
			f_color.rgb = mix(f_color.rgb, u_borderColor, clamp((u_borderScale - (u_cornerScale - cornerDistance)) / u_borderScale * 4.0, 0.0, 1.0));
		}
	}

	if (u_drawBorder)
	{
		if ((nonAbsCenteredTexCoords.x >= scale.x - u_borderScale) && isBorderVisible(0))
		{
			f_color.rgb = u_borderColor;
		}
		else if ((nonAbsCenteredTexCoords.x <= -scale.x + u_borderScale) && isBorderVisible(1))
		{
			f_color.rgb = u_borderColor;
		}
		else if ((nonAbsCenteredTexCoords.y >= scale.y - u_borderScale) && isBorderVisible(2))
		{
			f_color.rgb = u_borderColor;
		}
		else if ((nonAbsCenteredTexCoords.y <= -scale.y + u_borderScale) && isBorderVisible(3))
		{
			f_color.rgb = u_borderColor;
		}
	}
	f_alpha.xyz = vec3(1.0);
	f_alpha.w = f_color.w;
}