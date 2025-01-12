#pragma once
#include "../Font.h"
#include <any>
#include <functional>
#include <string>
#include <map>

namespace kui
{
	class UIBox;

	class AnyContainer
	{
	public:
		std::any Value;
		bool Empty = false;

		AnyContainer()
		{
			Empty = true;
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
		AnyContainer(Vec2f Value)
		{
			this->Value = Value;
		}
		AnyContainer(Vec3f Value)
		{
			this->Value = Value;
		}

		operator float()
		{
			if (Empty)
				return 0;
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
			if (Empty)
				return "";
			try
			{
				return std::any_cast<std::string>(Value);
			}
			catch (std::bad_any_cast)
			{
				return std::any_cast<const char*>(Value);
			}
		}
		Vec2f AsVec2()
		{
			if (Empty)
				return 0;
			return std::any_cast<Vec2f>(Value);
		}
		Vec3f AsVec3()
		{
			if (Empty)
				return 0;
			try
			{
				return std::any_cast<Vec3f>(Value);
			}
			catch (std::bad_any_cast)
			{
				return Vec3f(std::any_cast<float>(Value));
			}
		}
	};

	class MarkupLanguageManager
	{
		std::map<std::string, Font*> Fonts;
		struct GlobalInfo
		{
			AnyContainer Value;
			std::map<void*, std::function<void()>> OnChangedCallbacks;
		};

		std::map<std::string, GlobalInfo> Globals;
		std::function<std::string(std::string)> GetStringFunction;
	public:
		std::vector<std::pair<void*, std::function<void()>>> TranslationChangedCallbacks;

		AnyContainer ListenToGlobal(const char* GlobalName, AnyContainer DefaultValue, void* Target, std::function<void()> OnChanged);
		void RemoveGlobalListener(void* Target);

		void SetGlobal(const char* GlobalName, AnyContainer Value);

		void OnTranslationChanged();

		void AddFont(std::string FontName, Font* FontPointer);
		void SetDefaultFont(Font* FontPointer);
		Font* GetFont(std::string FontName);

		std::string GetString(std::string From);

		void SetGetStringFunction(std::function<std::string(std::string)> NewFunction);

		static MarkupLanguageManager* GetActive();
	};
}