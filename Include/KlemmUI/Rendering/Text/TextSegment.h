#pragma once
#include <string>
#include <vector>
#include "../../Vector3.h"

namespace KlemmUI
{
	/**
	 * @brief
	 * A segment of a rendered text with a color and text attribute.
	 */
	struct TextSegment
	{
		/// The text of the segment
		std::string Text;
		/// The color of the segment
		Vector3f Color;
		TextSegment(std::string Text, Vector3f Color)
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
			return (Text == b.Text) && (Color.X == b.Color.X) && (Color.Y == b.Color.Y) && (Color.Z == b.Color.Z);
		}
	};

}