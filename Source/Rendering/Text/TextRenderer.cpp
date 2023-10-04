#define _CRT_SECURE_NO_WARNINGS
#include <Rendering/Text/TextRenderer.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include "../../Util/stb_truetype.h"
#include "../Shader.h"
#include <SDL.h>
#include <vector>
#include <iostream>
#include <Application.h>
#include <Rendering/ScrollObject.h>
#include <StringReplace.h>
#include <GL/glew.h>
#include <OS.h>

namespace _TextRenderer
{
	static Shader* TextShader = nullptr;
	std::vector<TextRenderer*> Renderers;
}

constexpr int FONT_BITMAP_WIDTH = 2700;
constexpr int FONT_BITMAP_PADDING = 32;
constexpr int FONT_MAX_UNICODE_CHARS = 1200;

void TextRenderer::CheckForTextShader()
{
	if (!_TextRenderer::TextShader)
	{
		_TextRenderer::TextShader = new Shader(Application::GetShaderPath() + "/text.vert", Application::GetShaderPath() + "/text.frag");
	}
}


size_t TextRenderer::GetCharacterIndexADistance(ColoredText Text, float Dist, float Scale, Vector2f& LetterOutLocation)
{
	size_t CharIndex = 0;
	float originalScale = Scale;
	Scale *= 2.0f;
	Dist /= 4.0f;
	float x = 0.f, y = CharacterSize;
	float LastX = 0.0f;
	size_t Wraps = 0;
	for (auto& seg : Text)
	{
		size_t LastWordIndex = SIZE_MAX;
		size_t LastWrapIndex = 0;
		Uint32 LastWordNumVertices = 0;
		std::wstring UTFString = OS::Utf8ToWstring(seg.Text);

		for (size_t i = 0; i < UTFString.size(); i++)
		{
			bool IsTab = UTFString[i] == '	';
			if (IsTab)
			{
				UTFString[i] = ' ';
			}
			// TODO: Correct handling of tabs like in the standalone ui library
			//for (int txIt = 0; txIt < (IsTab ? 4 : 1); txIt++)
			do
			{
				int GlyphIndex = (int)UTFString[i] - 32;
				if (GlyphIndex < 0)
				{
					continue;
				}
				if (GlyphIndex > FONT_MAX_UNICODE_CHARS)
				{
					GlyphIndex = FONT_MAX_UNICODE_CHARS - 31;
				}
				Glyph g = LoadedGlyphs[GlyphIndex];
				x += g.TotalSize.X;
			} while (++CharIndex % TabSize && IsTab);
			if (UTFString[i] == ' ')
			{
				LastWordIndex = i;
			}

			float CurrentDist = x / Application::AspectRatio / 450;
			float CurrentLastDist = LastX / Application::AspectRatio / 450;
			if (Dist < CurrentDist)
			{
				if (abs(Dist - CurrentDist) > abs(Dist - CurrentLastDist))
				{
					return CharIndex - 1;
				}
				return CharIndex;
			}
			LastX = x;
		}
	}
	return CharIndex;
}

TextRenderer::TextRenderer(std::string filename, float CharacterSizeInPixels)
{
	this->Filename = filename;
	CheckForTextShader();
	_TextRenderer::Renderers.push_back(this);
	stbtt_bakedchar* cdata = new stbtt_bakedchar[96];
	Uint8* ttfBuffer = (Uint8*)malloc(1 << 20);
	if (ttfBuffer == NULL)
	{
		throw "EPIC MALLOC FAILURE";
	}

	fread(ttfBuffer, 1, 1 << 20, fopen(Filename.c_str(), "rb"));
	stbtt_fontinfo finf;
	stbtt_InitFont(&finf, ttfBuffer, stbtt_GetFontOffsetForIndex(ttfBuffer, 0));


	uint8_t* GlypthBitmap = new uint8_t[FONT_BITMAP_WIDTH * FONT_BITMAP_WIDTH](0);
	int Offset = 0;
	int xcoord = 0;
	int ycoord = 0;
	int maxH = 0;

	LoadedGlyphs.clear();

	for (int i = 32; i <= FONT_MAX_UNICODE_CHARS + 1; i++)
	{
		Glyph New;
		int glyph = i;
		if (i > FONT_MAX_UNICODE_CHARS)
		{
			glyph = '#';
		}
		int w, h, xoff, yoff;
		auto bmp = stbtt_GetCodepointBitmap(&finf,
			0.05f,
			0.05f,
			glyph,
			&w,
			&h,
			&xoff,
			&yoff);

		int advW, leftB;
		stbtt_GetCodepointHMetrics(&finf, glyph, &advW, &leftB);

		New.TotalSize.X = (float)advW / 400.0f;
		New.TotalSize.Y = 0;

		if (xcoord + w + FONT_BITMAP_PADDING > FONT_BITMAP_WIDTH)
		{
			xcoord = 0;
			ycoord += maxH + FONT_BITMAP_PADDING;
		}

		New.TexCoordStart = Vector2(
			(float)xcoord / FONT_BITMAP_WIDTH,
			(float)ycoord / FONT_BITMAP_WIDTH);

		New.TexCoordOffset = Vector2(
			(float)(w + 3) / FONT_BITMAP_WIDTH,
			(float)(h + 3) / FONT_BITMAP_WIDTH);

		New.Offset = Vector2((float)xoff, (float)yoff) / 20.0f;
		New.Size = Vector2((float)w, (float)h) / 20.0f;

		CharacterSize = std::max(New.Size.Y + New.Offset.Y, CharacterSize);

		// Give some additional space for better anti aliasing
		New.Size += Vector2(3.0f / 20.0f, 3.0f / 20.0f);

		if (w == 0 || h == 0 || i == ' ')
		{
			New.TexCoordStart = 0;
			New.TexCoordOffset = 0;
			LoadedGlyphs.push_back(New);
			continue;
		}

		for (int ith = 0; ith < h; ith++)
		{
			for (int itw = 0; itw < w; itw++)
			{
				GlypthBitmap[(ith + ycoord) * FONT_BITMAP_WIDTH + (xcoord + itw)] = bmp[ith * w + itw];
			}
		}
		maxH = std::max(maxH, h);
		xcoord += w + FONT_BITMAP_PADDING;
		LoadedGlyphs.push_back(New);
	}

	glGenTextures(1, &fontTexture);
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_ALPHA,
		FONT_BITMAP_WIDTH,
		FONT_BITMAP_WIDTH,
		0,
		GL_ALPHA,
		GL_UNSIGNED_BYTE,
		GlypthBitmap);
	// can free temp_bitmap at this point
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	glGenVertexArrays(1, &fontVao);
	glBindVertexArray(fontVao);
	glGenBuffers(1, &fontVertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, fontVertexBufferId);

	fontVertexBufferCapacity = 35;
	fontVertexBufferData = new FontVertex[fontVertexBufferCapacity * 6];

	glBufferData(GL_ARRAY_BUFFER, sizeof(FontVertex) * 6 * fontVertexBufferCapacity, 0, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(FontVertex), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FontVertex), (const void*)offsetof(FontVertex, texCoords));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(FontVertex), (const void*)offsetof(FontVertex, color));
	glBindVertexArray(0);

	free(ttfBuffer);
	delete[] GlypthBitmap;
}

Vector2f TextRenderer::GetTextSize(ColoredText Text, float Scale, bool Wrapped, float LengthBeforeWrap)
{
	size_t CharIndex = 0;
	float originalScale = Scale;
	Scale *= 2.0f;
	float x = 0.f, y = CharacterSize;
	float MaxX = 0.0f;
	size_t Wraps = 0;
	for (auto& seg : Text)
	{
		size_t LastWordIndex = SIZE_MAX;
		size_t LastWrapIndex = 0;
		Uint32 LastWordNumVertices = 0;
		std::wstring UTFString = OS::Utf8ToWstring(seg.Text);

		for (size_t i = 0; i < UTFString.size(); i++)
		{
			bool IsTab = UTFString[i] == '	';
			if (IsTab)
			{
				UTFString[i] = ' ';
			}
			// TODO: Correct handling of tabs like in the standalone ui library
			//for (int txIt = 0; txIt < (IsTab ? 4 : 1); txIt++)
			do
			{
				int GlyphIndex = (int)UTFString[i] - 32;
				if (GlyphIndex < 0)
				{
					continue;
				}
				if (GlyphIndex > FONT_MAX_UNICODE_CHARS)
				{
					GlyphIndex = FONT_MAX_UNICODE_CHARS - 31;
				}
				Glyph g = LoadedGlyphs[GlyphIndex];
				x += g.TotalSize.X;
			} while (++CharIndex % TabSize && IsTab);
			if (UTFString[i] == ' ')
			{
				LastWordIndex = i;
			}

			MaxX = std::max(x, MaxX);

			if (x * Scale / 450 > LengthBeforeWrap && Wrapped)
			{
				Wraps++;
				if (LastWordIndex != SIZE_MAX && LastWordIndex != LastWrapIndex)
				{
					i = LastWordIndex;
					LastWrapIndex = i;
				}
				x = 0;
				y += CharacterSize;
			}
		}
	}
	return (Vector2f(MaxX / Application::AspectRatio / 450, y / 450 * 5)) * Scale;
}


DrawableText* TextRenderer::MakeText(ColoredText Text, Vector2f Pos, float Scale, Vector3f32 Color, float opacity, float LengthBeforeWrap)
{
	size_t CharIndex = 0;
	for (auto& i : Text)
	{
		std::string NewString = i.Text;
		for (size_t it = 0; it < i.Text.size(); it++)
		{
			if (i.Text[it] == '	')
			{
				i.Text[it] = ' ';
				do 
				{
					NewString.insert(NewString.begin() + it, ' ');
				} while (++CharIndex % TabSize);
			}
			else
			{
				CharIndex++;
			}
		}
		i.Text = NewString;
	}
	GLuint newVAO = 0, newVBO = 0;
	glGenVertexArrays(1, &newVAO);
	glBindVertexArray(newVAO);
	glGenBuffers(1, &newVBO);
	glBindBuffer(GL_ARRAY_BUFFER, newVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(FontVertex) * 6 * fontVertexBufferCapacity, 0, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(FontVertex), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FontVertex), (const void*)offsetof(FontVertex, texCoords));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(FontVertex), (const void*)offsetof(FontVertex, color));
	glBindVertexArray(0);

	float originalScale = Scale;
	Scale *= 2.0f;
	Pos.X = Pos.X * 450 * Application::AspectRatio;
	Pos.Y = Pos.Y * -450;
	glBindVertexArray(newVAO);
	glBindBuffer(GL_ARRAY_BUFFER, newVBO);
	uint32_t len = (uint32_t)TextSegment::CombineToString(Text).size();
	if (fontVertexBufferCapacity < len)
	{
		fontVertexBufferCapacity = len;
		glBufferData(GL_ARRAY_BUFFER, sizeof(FontVertex) * 6 * fontVertexBufferCapacity, 0, GL_DYNAMIC_DRAW);
		delete[]fontVertexBufferData;
		fontVertexBufferData = new FontVertex[fontVertexBufferCapacity * 6];
	}
	float MaxHeight = 0.0f;
	float x = 0.f, y = 0.f;
	FontVertex* vData = fontVertexBufferData;
	Uint32 numVertices = 0;
	for (auto& seg : Text)
	{
		size_t LastWordIndex = SIZE_MAX;
		size_t LastWrapIndex = 0;
		Uint32 LastWordNumVertices = 0;
		FontVertex* LastWordVDataPtr = nullptr;
		std::wstring UTFString = OS::Utf8ToWstring(seg.Text);

		for (size_t i = 0; i < UTFString.size(); i++)
		{
			int GlyphIndex = (int)UTFString[i] - 32;
			if (GlyphIndex > FONT_MAX_UNICODE_CHARS)
			{
				GlyphIndex = FONT_MAX_UNICODE_CHARS - 31;
			}
			Glyph g = LoadedGlyphs[GlyphIndex];

			Vector2f32 StartPos = Vector2f32(x, y) + g.Offset - Vector2f32(0, CharacterSize / 2.0f);

			vData[0].position = StartPos + Vector2f32(0, g.Size.Y); vData[0].texCoords = g.TexCoordStart + Vector2f32(0, g.TexCoordOffset.Y);
			vData[1].position = StartPos + g.Size;               vData[1].texCoords = g.TexCoordStart + g.TexCoordOffset;
			vData[2].position = StartPos + Vector2f32(g.Size.X, 0); vData[2].texCoords = g.TexCoordStart + Vector2f32(g.TexCoordOffset.X, 0);
			vData[3].position = StartPos;                        vData[3].texCoords = g.TexCoordStart;
			vData[4].position = StartPos + Vector2f32(0, g.Size.Y); vData[4].texCoords = g.TexCoordStart + Vector2f32(0, g.TexCoordOffset.Y);
			vData[5].position = StartPos + Vector2f32(g.Size.X, 0); vData[5].texCoords = g.TexCoordStart + Vector2f32(g.TexCoordOffset.X, 0);
			vData[0].color = seg.Color;		vData[1].color = seg.Color;
			vData[2].color = seg.Color;		vData[3].color = seg.Color;
			vData[4].color = seg.Color;		vData[5].color = seg.Color;
			x += g.TotalSize.X;

			if (UTFString[i] == L' ')
			{
				LastWordIndex = i;
				LastWordNumVertices = numVertices;
				LastWordVDataPtr = vData;
			}

			MaxHeight = std::max(StartPos.Y + g.Offset.Y, MaxHeight);
			vData += 6;
			numVertices += 6;
			if (x * Scale / 450 > LengthBeforeWrap)
			{
				if (LastWordIndex != SIZE_MAX && LastWordIndex != LastWrapIndex)
				{
					i = LastWordIndex;
					LastWrapIndex = i;
					vData = LastWordVDataPtr;
					numVertices = LastWordNumVertices;
				}
				x = 0;
				y += CharacterSize * 5.0f;
			}
		}
	}
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(FontVertex) * numVertices, fontVertexBufferData);
	return new DrawableText(newVAO, newVBO, numVertices, fontTexture, Pos, Scale, Color, opacity);
}

TextRenderer::~TextRenderer()
{
	unsigned int i = 0;
	for (TextRenderer* r : _TextRenderer::Renderers)
	{
		if (r == this)
			_TextRenderer::Renderers.erase(_TextRenderer::Renderers.begin() + i);
		i++;
	}
	glDeleteTextures(1, &fontTexture);
	glDeleteBuffers(1, &fontVertexBufferId);
	glDeleteBuffers(1, &fontVao);
	if (fontVertexBufferData)
	{
		delete[] fontVertexBufferData;
	}
}

void OnWindowResized()
{
}

DrawableText::DrawableText(unsigned int VAO, unsigned int VBO, unsigned int NumVerts, unsigned int Texture,
	Vector2f Position, float Scale, Vector3f32 Color, float opacity)
{
	this->Position = Position;
	this->Scale = Scale;
	this->NumVerts = NumVerts;
	this->VAO = VAO;
	this->VBO = VBO;
	this->Opacity = opacity;
	this->Texture = Texture;
	this->Color = Color;
}

void DrawableText::Draw(ScrollObject* CurrentScrollObject)
{
	TextRenderer::CheckForTextShader();
	glBindVertexArray(VAO);
	_TextRenderer::TextShader->Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glUniform1i(glGetUniformLocation(_TextRenderer::TextShader->GetShaderID(), "u_texture"), 0);
	glUniform3f(glGetUniformLocation(_TextRenderer::TextShader->GetShaderID(), "textColor"), Color.X, Color.Y, Color.Z);
	glUniform1f(glGetUniformLocation(_TextRenderer::TextShader->GetShaderID(), "u_aspectratio"), Application::AspectRatio);
	glUniform3f(glGetUniformLocation(_TextRenderer::TextShader->GetShaderID(), "transform"), (float)Position.X, (float)Position.Y, Scale);
	glUniform1f(glGetUniformLocation(_TextRenderer::TextShader->GetShaderID(), "u_opacity"), Opacity);
	if (CurrentScrollObject != nullptr)
	{
		glUniform3f(glGetUniformLocation(_TextRenderer::TextShader->GetShaderID(), "u_offset"),
			-CurrentScrollObject->Percentage, CurrentScrollObject->Position.Y, CurrentScrollObject->Position.Y - CurrentScrollObject->Scale.Y);
	}
	else
		glUniform3f(glGetUniformLocation(_TextRenderer::TextShader->GetShaderID(), "u_offset"), 0, -1000, 1000);
	glDrawArrays(GL_TRIANGLES, 0, NumVerts);
}

DrawableText::~DrawableText()
{
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}
