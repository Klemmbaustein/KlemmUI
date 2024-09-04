#pragma once
#include "../UI/UIBox.h"
#include "../Rendering/Text/Font.h"
#include <any>
#include <functional>
#include <string>

namespace KlemmUI
{
	class AnyContainer
	{
	public:
		std::any Value;

		AnyContainer()
		{

		}

		AnyContainer(std::any Value)
		{
			this->Value = Value;
		}
		AnyContainer(const char* Value)
		{
			this->Value = Value;
		}
		AnyContainer(std::string Value)
		{
			this->Value = Value;
		}
		AnyContainer(float Value)
		{
			this->Value = Value;
		}
		AnyContainer(double Value)
		{
			this->Value = (float)Value;
		}
		AnyContainer(Vector2f Value)
		{
			this->Value = Value;
		}
		AnyContainer(Vector3f Value)
		{
			this->Value = Value;
		}

		operator float()
		{
			try
			{
				return std::any_cast<float>(Value);
			}
			catch (std::bad_any_cast)
			{
				return (float)std::any_cast<double>(Value);
			}
		}
		operator std::string()
		{
			try
			{
				return std::any_cast<std::string>(Value);
			}
			catch (std::bad_any_cast)
			{
				return std::any_cast<const char*>(Value);
			}
		}
		operator Vector2f()
		{
			return std::any_cast<Vector2f>(Value);
		}
		operator Vector3f()
		{
			return std::any_cast<Vector3f>(Value);
		}
	};

	class MarkupLanguageManager
	{
		std::map<std::string, Font*> Fonts;
		std::function<std::string(std::string)> GetStringFunction;
	public:
		void AddFont(std::string FontName, Font* FontPointer);
		void SetDefaultFont(Font* FontPointer);
		Font* GetFont(std::string FontName);

		std::string GetString(std::string From);

		void SetGetStringFunction(std::function<std::string(std::string)> NewFunction);

		static MarkupLanguageManager* GetActive();
	};
}