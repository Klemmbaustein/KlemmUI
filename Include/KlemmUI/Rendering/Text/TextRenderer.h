#pragma once
#include "../../Vector2.h"
#include "../../Vector3.h"
#include "TextSegment.h"
#include <cstdint>

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
	friend class TextRenderer;
	unsigned int VAO, VBO;
	unsigned int Texture;
	Vector3f Color;
	//ScrollObject* CurrentScrollObject;
	float Scale;
	Vector2f Position;
	unsigned int NumVerts;
	DrawableText(unsigned int VAO, unsigned int VBO, unsigned int NumVerts, unsigned int Texture, Vector2f Position, float Scale, Vector3f Color, float opacity);
public:
	float Opacity = 1.f;
	void Draw(ScrollObject* CurrentScrollObject);
	~DrawableText();
};

class TextRenderer
{
	friend class DrawableText;
private:
	static void CheckForTextShader();
	unsigned int fontTexture;
	unsigned int fontVao;
	unsigned int fontVertexBufferId;
	FontVertex* fontVertexBufferData = 0;
	uint32_t fontVertexBufferCapacity;
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
	size_t GetCharacterIndexADistance(ColoredText Text, float Dist, float Scale);
	TextRenderer(std::string filename);
	Vector2f GetTextSize(ColoredText Text, float Scale, bool Wrapped, float LengthBeforeWrap);
	DrawableText* MakeText(ColoredText Text, Vector2f Pos, float Scale, Vector3f Color, float opacity, float LengthBeforeWrap);
	~TextRenderer();

};