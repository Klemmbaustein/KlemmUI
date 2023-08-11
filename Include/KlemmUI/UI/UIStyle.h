#pragma once
#include "../Math/Vector3.h"
#include "UIBox.h"
#include "../Application.h"
#include <iostream>

class UIStyle
{
public:
	double UpPadding = 0.01;
	double DownPadding = 0.01;
	double RightPadding = 0.01;
	double LeftPadding = 0.01;

	std::string Name;

	UIStyle(std::string Name);
	virtual ~UIStyle();

	UIStyle* SetPadding(double Up, double Down, double Left, double Right);
	UIStyle* SetPadding(double AllDirs);

	void ApplyTo(UIBox* Target);

	UIBox::BorderType Border = UIBox::BorderType::None;
	float BorderSize = 0;

	Vector2f MinSize = 0;
	Vector2f MaxSize = 999;

	void UpdateStyle();
	template<typename T>
	T* ToSafeElemPtr(UIBox* Target)
	{
		static_assert(std::derived_from<T, UIBox>);
		T* Elem = dynamic_cast<T*>(Target);

		if (!Elem)
		{
			Application::Error("Failed to apply styling \""
				+ Name
				+ "\". Target is not \""
				+ std::string(typeid(T).name())
				+ "\" but \"" 
				+ std::string(typeid(*Target).name()) + "\"");
			throw 1;
		}
		return Elem;
	}

protected:
	virtual void ApplyDerived(UIBox* Target);
};