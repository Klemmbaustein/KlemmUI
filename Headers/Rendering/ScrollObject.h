#pragma once
#include "../Math/Vector2.h"
#include <set>

class ScrollObject
{
public:
	Vector2f Position;
	Vector2f Scale;
	float Percentage = 0;
	static std::set<ScrollObject*> GetAllScrollObjects();
	ScrollObject(Vector2f Position, Vector2f Scale, float MaxScroll);
	~ScrollObject();
	void ScrollUp();
	void ScrollDown();
	float Speed = 8;
	float MaxScroll = 10;
};