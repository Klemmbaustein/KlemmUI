#include <kui/Font.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include "Util/stb_truetype.hpp"
#include <kui/Rendering/Shader.h>
#include <kui/Rendering/RenderBackend.h>
#include <cstdint>
#include <vector>
#include <kui/App.h>
#include <kui/UI/UIScrollBox.h>
#include <kui/Resource.h>
#include <kui/Window.h>
#include "Internal/Internal.h"
#include <iostream>
using namespace kui;

using std::size_t;

constexpr int FONT_BITMAP_WIDTH = 2000;
constexpr int FONT_BITMAP_PADDING = 16;
constexpr int FONT_MAX_UNICODE_CHARS = 1600;

static void ReplaceTabs(std::vector<TextSegment>& Text, size_t TabSize, char ReplaceWith = ' ')
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
			else if (i.Text[it] == '\n')
			{
				CharIndex = 0;
			}
			else
			{
				CharIndex++;
			}
		}
	}

}

size_t Font::GetCharacterAtPosition(std::vector<TextSegment> Text, Vec2f Position, float Scale, bool Wrapped, float LengthBeforeWrap, uint32_t MaxLines)
{
	Scale /= CharacterSize / 6.0f;
	float MaxHeight = 0.0f;
	float x = 0.f, y = 0;
	size_t i = 0;
	size_t CharIndex = 0;
	size_t Nearest = SIZE_MAX;
	float NearestVerticalDist = INFINITY;
	float LastVerticalDist = INFINITY;
	LengthBeforeWrap = LengthBeforeWrap * Window::GetActiveWindow()->GetAspectRatio() / Scale;
	uint32_t CurrentLine = 0;

	for (auto& seg : Text)
	{
		size_t LastWordIndex = SIZE_MAX;
		size_t LastWrapIndex = 0;
		std::u32string SegmentText = internal::GetUnicodeString(seg.Text, true);
		bool FoundInCurrentLine = false;
		for (size_t i = 0; i < SegmentText.size(); i++, CharIndex++)
		{
			if (CurrentLine > MaxLines)
			{
				break;
			}
			float CharSize = 0;
			bool IsTab = SegmentText[i] == int('\t');
			if (IsTab)
			{
				IsTab = true;
				SegmentText[i] = ' ';
			}
			if (SegmentText[i] < 32)
			{
				if (SegmentText[i] == int('\n'))
				{
					LastVerticalDist = -Position.Y - (y + CharacterSize / 2) / 450 * Scale;
					x = 0;
					y += CharacterSize;
					CurrentLine++;

					FoundInCurrentLine = false;
				}
			}
			else
			{
				int GlyphIndex = (int)SegmentText[i] - 32;
				if (GlyphIndex < 0)
				{
					continue;
				}
				if (GlyphIndex > LoadedGlyphs.size())
				{
					GlyphIndex = int(LoadedGlyphs.size() - 1);
				}

				Glyph g = LoadedGlyphs[GlyphIndex];

				if (IsTab)
				{
					int Multiplier = (TabSize - CharIndex % TabSize);
					g.TotalSize.X *= Multiplier;
					CharIndex += Multiplier - 1;
				}

				if (SegmentText[i] == ' ')
				{
					LastWordIndex = i;
				}

				if (((x + g.TotalSize.X) / 450 > LengthBeforeWrap && Wrapped))
				{
					x = 0;
					y += CharacterSize;
					if (LastWordIndex != SIZE_MAX && LastWordIndex != LastWrapIndex)
					{
						i = LastWordIndex;
						LastWrapIndex = i;
						if (Nearest > i)
						{
							Nearest = SIZE_MAX;
						}
						continue;
					}
					LastVerticalDist = -Position.Y - (y + CharacterSize / 2) / 450 * Scale;
					CurrentLine++;

					FoundInCurrentLine = false;
					if (CurrentLine > MaxLines)
					{
						break;
					}
				}

				x += g.TotalSize.X;
				CharSize = g.TotalSize.X;
			}
			float CurrentDistance = x / 450 / Window::GetActiveWindow()->GetAspectRatio() * Scale;
			float VerticalDist = -Position.Y - (y + CharacterSize / 2) / 450 * Scale;

			if (std::abs(LastVerticalDist) < std::abs(VerticalDist) && Nearest == SIZE_MAX)
			{
				return i;
			}
			bool IsOnCurrentLine = false;
			if (std::abs(VerticalDist) < std::abs(NearestVerticalDist))
			{
				Nearest = SIZE_MAX;
				IsOnCurrentLine = true;
			}

			if (CurrentDistance > Position.X && !FoundInCurrentLine && IsOnCurrentLine)
			{
				if (CurrentDistance > Position.X + (CharSize / 800 / Window::GetActiveWindow()->GetAspectRatio() * Scale))
				{
					Nearest = std::min(i, TextSegment::CombineToString(Text).size());
				}
				else
				{
					Nearest = std::min(i + 1, TextSegment::CombineToString(Text).size());
				}
				FoundInCurrentLine = true;
				NearestVerticalDist = VerticalDist;
			}
		}
	}

	return std::min(Nearest, TextSegment::CombineToString(Text).size());
}

Font::Font(std::string FileName)
{
	if (!resource::FileExists(FileName))
	{
		app::error::Error("Failed to find font resource: " + FileName);
		return;
	}

	resource::BinaryData TextData = resource::GetBinaryFile(FileName);

	stbtt_fontinfo finf;
	stbtt_InitFont(&finf, TextData.Data, stbtt_GetFontOffsetForIndex(TextData.Data, 0));


	uint8_t* GlyphBitmap = new uint8_t[FONT_BITMAP_WIDTH * FONT_BITMAP_WIDTH]();
	int xcoord = 0;
	int ycoord = 0;
	int maxH = 0;

	LoadedGlyphs.clear();
	int ascent, descent, lineGap;
	stbtt_GetFontVMetrics(&finf, &ascent, &descent, &lineGap);
	ascent = int(float(ascent) * 0.025f);
	descent = int(float(descent) * 0.025f);
	lineGap = int(float(lineGap) * 0.025f);

	for (int i = 32; i <= FONT_MAX_UNICODE_CHARS + 1; i++)
	{
		Glyph New;
		int glyph = i;
		int w, h, xoff, yoff;
		auto bmp = stbtt_GetCodepointBitmap(&finf,
			0.025f,
			0.025f,
			glyph,
			&w,
			&h,
			&xoff,
			&yoff);

		yoff += (12 + descent) * 2;

		int advW, leftB;
		stbtt_GetCodepointHMetrics(&finf, glyph, &advW, &leftB);

		New.TotalSize.X = (float)advW / 400.0f;
		New.TotalSize.Y = 0;

		if (xcoord + w + FONT_BITMAP_PADDING > FONT_BITMAP_WIDTH)
		{
			xcoord = 0;
			ycoord += maxH + FONT_BITMAP_PADDING;
		}

		New.TexCoordStart = Vec2(
			(float)xcoord / FONT_BITMAP_WIDTH,
			(float)ycoord / FONT_BITMAP_WIDTH);

		New.TexCoordOffset = Vec2(
			(float)(w + 3) / FONT_BITMAP_WIDTH,
			(float)(h + 3) / FONT_BITMAP_WIDTH);

		New.Offset = Vec2f((float)xoff, (float)yoff) / 10.0;
		New.Size = Vec2f((float)w, (float)h) / 10.0;

		CharacterSize = std::max(New.Size.Y + std::max(New.Offset.Y, 0.0f), CharacterSize);

		if (New.Size != 0)
		{
			// Give some additional space for better anti aliasing
			New.Size += Vec2(3.0f / 10.0f, 3.0f / 10.0f);
		}

		if (w == 0 || h == 0 || i == ' ')
		{
			New.TexCoordStart = 0;
			New.TexCoordOffset = 0;
			LoadedGlyphs.push_back(New);
			continue;
		}

		if (ycoord + h > FONT_BITMAP_WIDTH)
		{
			break;
		}

		for (int ith = 0; ith < h; ith++)
		{
			for (int itw = 0; itw < w; itw++)
			{
				GlyphBitmap[(ith + ycoord) * FONT_BITMAP_WIDTH + (xcoord + itw)] = bmp[ith * w + itw];
			}
		}
		maxH = std::max(maxH, h);
		xcoord += w + FONT_BITMAP_PADDING;
		LoadedGlyphs.push_back(New);
		free(bmp);
	}

	this->RenderData = Window::GetActiveWindow()->UI.Render->MakeFont(GlyphBitmap, FONT_BITMAP_WIDTH, FONT_BITMAP_WIDTH, CharacterSize);

	resource::FreeBinaryFile(TextData);

	delete[] GlyphBitmap;
}

Vec2f Font::GetTextSize(std::vector<TextSegment> Text, float Scale, bool Wrapped, float LengthBeforeWrap, uint32_t MaxLines, Vec2f* EndPos, size_t EndIndex)
{
	Scale /= CharacterSize / 6.0f;
	LengthBeforeWrap = LengthBeforeWrap * Window::GetActiveWindow()->GetAspectRatio() / Scale;
	float x = 0.f, y = CharacterSize;
	float MaxX = 0.0f;
	float LastMaxX = 0.0f;
	size_t CharIndex = 0;
	size_t TabCharIndex = 0;
	size_t LastWrapCharIndex = 0;
	uint32_t CurrentLine = 0;
	bool FoundEndPos = false;
	for (auto& seg : Text)
	{
		size_t LastWordIndex = SIZE_MAX;
		size_t LastWrapIndex = 0;
		std::u32string SegmentText = internal::GetUnicodeString(seg.Text, true);
		for (size_t i = 0; i < SegmentText.size(); i++, CharIndex++, TabCharIndex++)
		{
			if (CurrentLine > MaxLines)
			{
				break;
			}
			bool IsTab = SegmentText[i] == '\t';
			if (IsTab)
			{
				SegmentText[i] = ' ';
			}
			if (EndPos && CharIndex == EndIndex)
			{
				EndPos->X = x / 450 / Window::GetActiveWindow()->GetAspectRatio() * Scale;
				EndPos->Y = y / 450 * Scale;
				FoundEndPos = true;
			}
			if (SegmentText[i] >= 32)
			{
				int GlyphIndex = int(SegmentText[i] - 32);
				if (GlyphIndex < 0)
				{
					continue;
				}
				if (GlyphIndex > LoadedGlyphs.size())
				{
					GlyphIndex = int(LoadedGlyphs.size() - 1);
				}
				Glyph g = LoadedGlyphs[GlyphIndex];

				if (IsTab)
				{
					int Multiplier = (TabSize - TabCharIndex % TabSize);
					g.TotalSize.X *= Multiplier;
					TabCharIndex += Multiplier - 1;
				}

				if (SegmentText[i] == ' ')
				{
					LastWordIndex = i;
					LastMaxX = MaxX;
					LastWrapCharIndex = CharIndex;
				}

				if (((x + g.TotalSize.X) / 450 > LengthBeforeWrap && Wrapped))
				{
					CurrentLine++;
					if (CurrentLine > MaxLines)
					{
						break;
					}
					x = 0;
					y = CharacterSize * CurrentLine + CharacterSize;
					if (LastWordIndex != SIZE_MAX && LastWordIndex != LastWrapIndex)
					{
						i = LastWordIndex;
						MaxX = LastMaxX;
						LastWrapIndex = i;
						CharIndex = LastWrapCharIndex;
						continue;
					}
				}

				x += g.TotalSize.X;

				MaxX = std::max(MaxX, x);
			}
			if (SegmentText[i] == (int)'\n')
			{
				x = 0;
				CurrentLine++;
				y = CharacterSize * CurrentLine + CharacterSize;
			}
		}
	}
	if (EndPos && !FoundEndPos)
	{
		EndPos->X = x / 450 / Window::GetActiveWindow()->GetAspectRatio() * Scale;
		EndPos->Y = y / 450 * Scale;
	}

	return Vec2f(MaxX / 450 / Window::GetActiveWindow()->GetAspectRatio() * Scale, y / 450 * Scale);
}


DrawableText* Font::MakeText(std::vector<TextSegment> Text, float Scale, Vec3f Color, float opacity, float LengthBeforeWrap, uint32_t MaxLines)
{
	Scale /= CharacterSize / 6.0f;
	ReplaceTabs(Text, TabSize);
	LengthBeforeWrap = LengthBeforeWrap * Window::GetActiveWindow()->GetAspectRatio() / Scale;

	std::vector<RenderGlyph> GlyphData;

	std::size_t LastGlyphLength = 0;
	uint32_t CurrentLine = 0;
	float x = 0.f, y = 0.f;

	for (auto& seg : Text)
	{
		size_t LastWordIndex = SIZE_MAX;
		size_t LastWrapIndex = 0;
		FontVertex* LastWordVDataPtr = nullptr;
		std::u32string UTFString = internal::GetUnicodeString(seg.Text);

		for (size_t i = 0; i < UTFString.size(); i++)
		{
			if (CurrentLine > MaxLines)
			{
				break;
			}
			int GlyphIndex = (int)UTFString[i] - 32;
			if (GlyphIndex >= 0)
			{
				if (GlyphIndex > LoadedGlyphs.size())
				{
					GlyphIndex = int(LoadedGlyphs.size() - 31);
				}
				Glyph& g = LoadedGlyphs[GlyphIndex];

				if (UTFString[i] == ' ')
				{
					LastGlyphLength = GlyphData.size();
					LastWordIndex = i;
				}

				if ((x + g.TotalSize.X) / 450 > LengthBeforeWrap)
				{
					CurrentLine++;
					if (CurrentLine > MaxLines)
					{
						break;
					}
					x = 0;
					y = CharacterSize * CurrentLine;
					if (LastWordIndex != SIZE_MAX && LastWordIndex != LastWrapIndex)
					{
						i = LastWordIndex;
						LastWrapIndex = i;
						GlyphData.resize(LastGlyphLength);
						continue;
					}
				}

				Vec2 StartPos = Vec2(x, y) + g.Offset;
				if (g.Size != 0)
				{
					GlyphData.push_back(RenderGlyph{
							.Target = &g,
							.Color = seg.Color,
							.Position = StartPos,
						});
				}
				x += g.TotalSize.X;
			}
			if (UTFString[i] == (int)'\n')
			{
				x = 0;
				CurrentLine++;
				y = CharacterSize * CurrentLine;
				if (CurrentLine > MaxLines)
				{
					break;
				}
			}
		}
	}
	return Window::GetActiveWindow()->UI.Render->MakeText(RenderData, GlyphData, Scale, Color, opacity);
}

Font::~Font()
{
	if (RenderData)
	{
		delete RenderData;
	}
}

DrawableText::DrawableText(float Scale, Vec3f Color, float Opacity)
{
	this->Scale = Scale;
	this->Opacity = Opacity;
	this->Color = Color;
}

void kui::DrawableText::Draw(ScrollObject* CurrentScrollObject, Vec2f Pos)
{

}

DrawableText::~DrawableText()
{
}
