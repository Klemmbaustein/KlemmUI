#include <KlemmUI/Rendering/Text/Font.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include "../../Util/stb_truetype.hpp"
#include <KlemmUI/Rendering/Shader.h>
#include <cstdint>
#include <vector>
#include <KlemmUI/Application.h>
#include <KlemmUI/Rendering/ScrollObject.h>
#include <KlemmUI/StringReplace.h>
#include <GL/glew.h>
#include <KlemmUI/Window.h>
#include <filesystem>
#include "../../Internal.h"

using namespace KlemmUI;


constexpr int FONT_BITMAP_WIDTH = 3000;
constexpr int FONT_BITMAP_PADDING = 32;
constexpr int FONT_MAX_UNICODE_CHARS = 800;

const std::string TextShaderName = "TextShader";

Shader* Font::GetTextShader()
{
	return Window::GetActiveWindow()->Shaders.GetShader(TextShaderName);
}

size_t Font::GetCharacterIndexADistance(std::vector<TextSegment> Text, float Dist, float Scale)
{
	Scale *= 2.5f;
	std::u32string TextString = Internal::GetUnicodeString(TextSegment::CombineToString(Text));
	TextString.append({ (uint32_t)' ' });
	float MaxHeight = 0.0f;
	float x = 0.f;
	size_t i = 0;
	size_t CharIndex = 0;
	for (auto& c : TextString)
	{
		bool IsTab = false;
		if (c == L'\t')
		{
			c = L' ';
			IsTab = true;
		}
		if (c >= 32)
		{
			int GlyphIndex = (int)c - 32;
			if (GlyphIndex < 0)
			{
				continue;
			}
			if (GlyphIndex > FONT_MAX_UNICODE_CHARS)
			{
				GlyphIndex = FONT_MAX_UNICODE_CHARS - 31;
			}
			Glyph g = LoadedGlyphs[GlyphIndex];
			do
			{
				x += g.TotalSize.X;
			} while (++CharIndex % TabSize && IsTab);
			MaxHeight = std::max(g.Size.Y + g.Offset.Y, MaxHeight);
			float ldst = x / 450 / Window::GetActiveWindow()->GetAspectRatio() * Scale;
			if (ldst > Dist)
			{
				if (ldst > Dist + (g.TotalSize.X / 800 / Window::GetActiveWindow()->GetAspectRatio() * Scale))
				{
					return std::min(i, TextSegment::CombineToString(Text).size());
				}

				return std::min(i + 1, TextSegment::CombineToString(Text).size());
			}
		}
		i++;
	}

	return std::min(i, TextSegment::CombineToString(Text).size());
}

Font::Font(std::string Filename)
{
	if (!std::filesystem::exists(Filename))
	{
		Application::Error::Error("Failed to load font: " + Filename, true);
	}

	Window::GetActiveWindow()->Shaders.LoadShader("text.vert", "text.frag", TextShaderName);

	uint8_t* ttfBuffer = (uint8_t*)malloc(1 << 20);
	if (ttfBuffer == nullptr)
	{
		Application::Error::Error("Failed to allocate space for font bitmap");
		return;
	}

	size_t ret = fread(ttfBuffer, 1, 1 << 20, fopen(Filename.c_str(), "rb"));
	if (!ret || !ttfBuffer)
	{
		Application::Error::Error("Failed to load font: " + Filename);
		return;
	}
	stbtt_fontinfo finf;
	stbtt_InitFont(&finf, ttfBuffer, stbtt_GetFontOffsetForIndex(ttfBuffer, 0));


	uint8_t* GlypthBitmap = new uint8_t[FONT_BITMAP_WIDTH * FONT_BITMAP_WIDTH]();
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
			glyph = 0x000025A1;
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

		New.Offset = Vector2f((float)xoff, (float)yoff) / 20.0;
		New.Size = Vector2f((float)w, (float)h) / 20.0;

		CharacterSize = std::max(New.Size.Y + New.Offset.Y, (float)CharacterSize);

		if (New.Size != 0)
		{
			// Give some additional space for better anti aliasing
			New.Size += Vector2(3.0f / 20.0f, 3.0f / 20.0f);
		}

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
		free(bmp);
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

	glBufferData(GL_ARRAY_BUFFER, sizeof(FontVertex) * 6 * fontVertexBufferCapacity, 0, GL_STATIC_DRAW);
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

Vector2f Font::GetTextSize(std::vector<TextSegment> Text, float Scale, bool Wrapped, float LengthBeforeWrap)
{
	Scale *= 2.5f;
	LengthBeforeWrap = LengthBeforeWrap * Window::GetActiveWindow()->GetAspectRatio() / Scale;
	float x = 0.f, y = CharacterSize * 5;
	float MaxX = 0.0f;
	FontVertex* vData = fontVertexBufferData;
	uint32_t numVertices = 0;
	size_t CharIndex = 0;
	for (auto& seg : Text)
	{
		size_t LastWordIndex = SIZE_MAX;
		size_t LastWrapIndex = 0;
		uint32_t LastWordNumVertices = 0;
		FontVertex* LastWordVDataPtr = nullptr;
		std::u32string SegmentText = Internal::GetUnicodeString(seg.Text);
		for (size_t i = 0; i < SegmentText.size(); i++)
		{
			bool IsTab = SegmentText[i] == '\t';
			if (IsTab)
			{
				SegmentText[i] = ' ';
			}

			if (SegmentText[i] >= 32)
			{
				int GlyphIndex = (int)SegmentText[i] - 32;
				if (GlyphIndex < 0)
				{
					continue;
				}
				if (GlyphIndex > FONT_MAX_UNICODE_CHARS)
				{
					GlyphIndex = FONT_MAX_UNICODE_CHARS - 31;
				}
				Glyph g = LoadedGlyphs[GlyphIndex];
				do
				{
					x += g.TotalSize.X;
				} while (++CharIndex % TabSize && IsTab);

				if (SegmentText[i] == ' ')
				{
					LastWordIndex = i;
					LastWordNumVertices = numVertices;
					LastWordVDataPtr = vData;
				}

				vData += 6;
				numVertices += 6;
				MaxX = std::max(MaxX, x);
			}
			if ((x / 225 > LengthBeforeWrap && Wrapped) || SegmentText[i] == (int)'\n')
			{
				if (LastWordIndex != SIZE_MAX && LastWordIndex != LastWrapIndex)
				{
					i = LastWordIndex;
					LastWrapIndex = i;
					vData = LastWordVDataPtr;
					numVertices = LastWordNumVertices;
				}
				x = 0;
				y += CharacterSize * 5;
			}
		}
	}
	return Vector2f(MaxX / 450 / Window::GetActiveWindow()->GetAspectRatio() * Scale, y / 450 * Scale);
}


DrawableText* Font::MakeText(std::vector<TextSegment> Text, Vector2f Pos, float Scale, Vector3f Color, float opacity, float LengthBeforeWrap)
{
	size_t CharIndex = 0;
	for (auto& i : Text)
	{
		for (size_t it = 0; it < i.Text.size(); it++)
		{
			if (i.Text[it] == '\t')
			{
				i.Text[it] = ' ';
				while (++CharIndex % TabSize)
				{
					i.Text.insert(i.Text.begin() + it++, ' ');
				}
			}
			else
			{
				CharIndex++;
			}
		}
	}

	GLuint newVAO = 0, newVBO = 0;
	glGenVertexArrays(1, &newVAO);
	glBindVertexArray(newVAO);
	glGenBuffers(1, &newVBO);
	glBindBuffer(GL_ARRAY_BUFFER, newVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(FontVertex) * 6 * fontVertexBufferCapacity, 0, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(FontVertex), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FontVertex), (const void*)offsetof(FontVertex, texCoords));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(FontVertex), (const void*)offsetof(FontVertex, color));
	glBindVertexArray(0);

	Scale *= 2.5f;
	LengthBeforeWrap = LengthBeforeWrap * Window::GetActiveWindow()->GetAspectRatio() / Scale;
	Pos.X = Pos.X * 450 * Window::GetActiveWindow()->GetAspectRatio();
	Pos.Y = Pos.Y * -450;
	glBindVertexArray(newVAO);
	glBindBuffer(GL_ARRAY_BUFFER, newVBO);
	uint32_t len = (uint32_t)TextSegment::CombineToString(Text).size();
	if (fontVertexBufferCapacity < len)
	{
		fontVertexBufferCapacity = len;
		glBufferData(GL_ARRAY_BUFFER, sizeof(FontVertex) * 6 * fontVertexBufferCapacity, 0, GL_STATIC_DRAW);
		delete[] fontVertexBufferData;
		fontVertexBufferData = new FontVertex[fontVertexBufferCapacity * 6];
	}
	float MaxHeight = 0.0f;
	float x = 0.f, y = 0.f;
	FontVertex* vData = fontVertexBufferData;
	uint32_t numVertices = 0;
	for (auto& seg : Text)
	{
		size_t LastWordIndex = SIZE_MAX;
		size_t LastWrapIndex = 0;
		uint32_t LastWordNumVertices = 0;
		FontVertex* LastWordVDataPtr = nullptr;
		std::u32string UTFString = Internal::GetUnicodeString(seg.Text);

		for (size_t i = 0; i < UTFString.size(); i++)
		{
			int GlyphIndex = (int)UTFString[i] - 32;
			if (GlyphIndex >= 0)
			{
				if (GlyphIndex > FONT_MAX_UNICODE_CHARS)
				{
					GlyphIndex = FONT_MAX_UNICODE_CHARS - 31;
				}
				Glyph g = LoadedGlyphs[GlyphIndex];

				Vector2 StartPos = Vector2(x, y) + g.Offset;
				if (g.Size != 0)
				{
					vData[0].position = StartPos + Vector2f(0, g.Size.Y); vData[0].texCoords = g.TexCoordStart + Vector2f(0, g.TexCoordOffset.Y);
					vData[1].position = StartPos + g.Size;                vData[1].texCoords = g.TexCoordStart + g.TexCoordOffset;
					vData[2].position = StartPos + Vector2f(g.Size.X, 0); vData[2].texCoords = g.TexCoordStart + Vector2f(g.TexCoordOffset.X, 0);
					vData[3].position = StartPos;                         vData[3].texCoords = g.TexCoordStart;
					vData[4].position = StartPos + Vector2f(0, g.Size.Y); vData[4].texCoords = g.TexCoordStart + Vector2f(0, g.TexCoordOffset.Y);
					vData[5].position = StartPos + Vector2f(g.Size.X, 0); vData[5].texCoords = g.TexCoordStart + Vector2f(g.TexCoordOffset.X, 0);
					vData[0].color = seg.Color;		vData[1].color = seg.Color;
					vData[2].color = seg.Color;		vData[3].color = seg.Color;
					vData[4].color = seg.Color;		vData[5].color = seg.Color;
					vData += 6;
					numVertices += 6;
				}
				x += g.TotalSize.X;

				if (UTFString[i] == ' ')
				{
					LastWordIndex = i;
					LastWordNumVertices = numVertices;
					LastWordVDataPtr = vData;
				}
				MaxHeight = std::max(StartPos.Y + g.Offset.Y, MaxHeight);
			}
			if (x / 225 > LengthBeforeWrap || UTFString[i] == (int)'\n')
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

Font::~Font()
{
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
	Vector2f Position, float Scale, Vector3f Color, float opacity)
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

void DrawableText::Draw(ScrollObject* CurrentScrollObject) const
{
	Shader* TextShader = Font::GetTextShader();
	glBindVertexArray(VAO);
	TextShader->Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	TextShader->SetInt("u_texture", 0);
	TextShader->SetVec3("textColor", Vector3f(Color.X, Color.Y, Color.Z));
	TextShader->SetFloat("u_aspectratio", Window::GetActiveWindow()->GetAspectRatio());
	TextShader->SetVec3("transform", Vector3f((float)Position.X, (float)Position.Y, Scale));
	TextShader->SetVec2("u_screenRes", Vector2f(Window::GetActiveWindow()->GetSize().Y * 1.5f));
	TextShader->SetFloat("u_opacity", Opacity);
	if (CurrentScrollObject != nullptr)
	{
		TextShader->SetVec3("u_offset",
			Vector3f(-CurrentScrollObject->Percentage, CurrentScrollObject->Position.Y, CurrentScrollObject->Position.Y - CurrentScrollObject->Scale.Y));
	}
	else
		TextShader->SetVec3("u_offset", Vector3f(0.0f, -1000.0f, 1000.0f));
	glDrawArrays(GL_TRIANGLES, 0, NumVerts);
}

DrawableText::~DrawableText()
{
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}
