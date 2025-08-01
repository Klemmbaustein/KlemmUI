#pragma once
#include "Vec2.h"
#include "Vec3.h"
#include <vector>
#include <cstdint>

namespace kui
{
	class ScrollObject;
	/**
	 * @brief
	 * A segment of a rendered text with a color and text value.
	 */
	struct TextSegment
	{
		/// The text of the segment
		std::string Text;
		/// The color of the segment
		Vec3f Color;
		TextSegment(std::string Text, Vec3f Color)
		{
			this->Text = Text;
			this->Color = Color;
		}

		static std::string CombineToString(const std::vector<TextSegment>& TextSegments)
		{
			std::string ret;
			for (const auto& i : TextSegments)
			{
				ret.append(i.Text);
			}
			return ret;
		}
		static std::size_t Length(const std::vector<TextSegment>& TextSegments)
		{
			std::size_t Length = 0;
			for (const auto& i : TextSegments)
			{
				Length += i.Text.size();
			}
			return Length;
		}

		bool operator==(const TextSegment& b) const
		{
			return (Text == b.Text) && (Color.X == b.Color.X) && (Color.Y == b.Color.Y) && (Color.Z == b.Color.Z);
		}
	};

	struct FontVertex
	{
		Vec2f position;
		Vec2f texCoords;
		Vec3f color = 1;
	};

	class Shader;
	class FontRenderData
	{
	public:
		virtual ~FontRenderData() = default;
	};

	class DrawableText
	{
		friend class Font;
	public:
		DrawableText(float Scale, Vec3f Color, float Opacity);
		Vec3f Color = 0;
		float Scale = 0;
		float Opacity = 1.0f;
		virtual void Draw(ScrollObject* CurrentScrollObject, Vec2f Pos);
		virtual ~DrawableText();
	};

	/**
	* @brief
	* A font class containing information about a loaded font (.ttf) file.
	*/
	class Font
	{
	public:
		float CharacterSize = 0;
		friend class DrawableText;

		struct Glyph
		{
			Vec2f Size;
			Vec2f Offset;
			Vec2f TotalSize;
			Vec2f TexCoordStart;
			Vec2f TexCoordOffset;
		};

		struct RenderGlyph
		{
			Glyph* Target = nullptr;
			Vec3f Color;
			Vec2f Position;
		};

		std::vector<Glyph> LoadedGlyphs;

		uint8_t TabSize = 4;
		FontRenderData* RenderData = nullptr;

		std::size_t GetCharacterAtPosition(std::vector<TextSegment> Text, Vec2f Position, float Scale,
			bool Wrapped, float LengthBeforeWrap, uint32_t MaxLines);
		Font(std::string filename);
		Vec2f GetTextSize(std::vector<TextSegment> Text, float Scale, bool Wrapped,
			float LengthBeforeWrap, uint32_t MaxLines, Vec2f* EndPos = nullptr, std::size_t EndIndex = SIZE_MAX);
		DrawableText* MakeText(std::vector<TextSegment> Text, float Scale, Vec3f Color,
			float opacity, float LengthBeforeWrap, uint32_t MaxLines);
		~Font();

	};

}