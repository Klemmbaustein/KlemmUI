#pragma once
#include <string>
#include <vector>
#include "../../Math/Vector3.h"

struct TextSegment
{
	std::string Text;
	Vector3f32 Color;
	TextSegment(std::string Text, Vector3f32 Color)
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
	bool operator==(const TextSegment& b) const
	{
		return Text == b.Text && Color.X == b.Color.X && Color.Y == b.Color.Y && Color.Z && b.Color.Z;
	}
};

typedef std::vector<TextSegment> ColoredText;