#pragma once
#include "../Vector2.h"
#include <set>

namespace KlemmUI
{

	class ScrollObject
	{
		static std::set<ScrollObject*> AllScrollObjects;
	public:
		Vector2f Position;
		Vector2f Scale;
		float Percentage = 0;
		static std::set<ScrollObject*> GetAllScrollObjects();
		ScrollObject(Vector2f Position, Vector2f Scale, float MaxScroll);
		~ScrollObject();
		void ScrollUp();
		void ScrollDown();
		float Speed = 12;
		bool Active = true;
		float MaxScroll = 10;
	};

}