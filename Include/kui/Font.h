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
		static size_t Length(const std::vector<TextSegment>& TextSegments)
		{
			size_t Length = 0;
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
	class DrawableText
	{
		friend class Font;
		unsigned int VAO = 0, VBO = 0;
		unsigned int Texture = 0;
		Vec3f Color = 0;
		float Scale = 0;
		unsigned int NumVerts = 0;
		DrawableText(unsigned int VAO, unsigned int VBO, unsigned int NumVerts, unsigned int Texture, float Scale, Vec3f Color, float opacity);
	public:
		float Opacity = 1.0f;
		void Draw(ScrollObject* CurrentScrollObject, Vec2f Pos) const;
		~DrawableText();
	};

	/**
	* @brief
	* A font class containing information about a loaded font (.ttf) file.
	*/
	class Font
	{
		friend class DrawableText;
	private:
		static Shader* GetTextShader();
		unsigned int fontTexture = 0;
		unsigned int fontVao = 0;
		unsigned int fontVertexBufferId = 0;
		FontVertex* fontVertexBufferData = 0;
		uint32_t fontVertexBufferCapacity = 0;
	public:
		float CharacterSize = 0;

		struct Glyph
		{
			Vec2f Size;
			Vec2f Offset;
			Vec2f TotalSize;
			Vec2f TexCoordStart;
			Vec2f TexCoordOffset;
		};
		std::vector<Glyph> LoadedGlyphs;

		uint8_t TabSize = 4;

		size_t GetCharacterAtPosition(std::vector<TextSegment> Text, Vec2f Position, float Scale, bool Wrapped, float LengthBeforeWrap, uint32_t MaxLines);
		Font(std::string filename);
		Vec2f GetTextSize(std::vector<TextSegment> Text, float Scale, bool Wrapped, float LengthBeforeWrap, uint32_t MaxLines, Vec2f* EndPos = nullptr, size_t EndIndex = SIZE_MAX);
		DrawableText* MakeText(std::vector<TextSegment> Text, float Scale, Vec3f Color, float opacity, float LengthBeforeWrap, uint32_t MaxLines);
		~Font();

	};

}