#pragma once
#include "../Vec2.h"
#include <set>

namespace kui
{

	class ScrollObject
	{
		static std::set<ScrollObject*> AllScrollObjects;
	public:
		Vec2f Position;
		Vec2f Scale;
		float Percentage = 0;
		static std::set<ScrollObject*> GetAllScrollObjects();
		ScrollObject(Vec2f Position, Vec2f Scale, float MaxScroll, bool Register = true);
		~ScrollObject();
		void ScrollUp();
		void ScrollDown();
		float Speed = 12;
		bool Active = true;
		float MaxScroll = 10;
	};

}