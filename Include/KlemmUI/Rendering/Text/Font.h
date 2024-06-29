#pragma once
#include "../../Vector2.h"
#include "../../Vector3.h"
#include "TextSegment.h"
#include <cstdint>

namespace KlemmUI
{
	class ScrollObject;

	struct FontVertex
	{
		Vector2f position;
		Vector2f texCoords;
		Vector3f color = 1;
	};
	//class ScrollObject;
	class Shader;
	class DrawableText
	{
		friend class Font;
		unsigned int VAO = 0, VBO = 0;
		unsigned int Texture = 0;
		Vector3f Color = 0;
		//ScrollObject* CurrentScrollObject;
		float Scale = 0;
		Vector2f Position;
		unsigned int NumVerts = 0;
		DrawableText(unsigned int VAO, unsigned int VBO, unsigned int NumVerts, unsigned int Texture, Vector2f Position, float Scale, Vector3f Color, float opacity);
	public:
		float Opacity = 1.f;
		void Draw(ScrollObject* CurrentScrollObject) const;
		~DrawableText();
	};

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
			Vector2f Size;
			Vector2f Offset;
			Vector2f TotalSize;
			Vector2f TexCoordStart;
			Vector2f TexCoordOffset;
		};
		std::vector<Glyph> LoadedGlyphs;

		uint8_t TabSize = 4;
		std::string Filename;
		size_t GetCharacterIndexADistance(std::vector<TextSegment> Text, float Dist, float Scale);
		Font(std::string filename);
		Vector2f GetTextSize(std::vector<TextSegment> Text, float Scale, bool Wrapped, float LengthBeforeWrap);
		DrawableText* MakeText(std::vector<TextSegment> Text, Vector2f Pos, float Scale, Vector3f Color, float opacity, float LengthBeforeWrap);
		~Font();

	};

}