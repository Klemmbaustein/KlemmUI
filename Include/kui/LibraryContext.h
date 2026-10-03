#pragma once
#include <kui/Window.h>
#include <kui/UIResourceProvider.h>
#include <iostream>

namespace kui
{
	template<typename T>
	class UIContextRef
	{
	public:
		UIContextRef(T* Context)
			: Context(Context)
		{
			if (Context)
			{
				Context->RefCount++;
			}
		}

		UIContextRef(const UIContextRef& Other)
		{
			if (Context && Context->RefCount.fetch_sub(1) == 1)
			{
				delete Context;
			}

			Other.Context->RefCount += 1;
			Context = Other.Context;
		}

		UIContextRef& operator=(const UIContextRef& Other)
		{
			if (Context && Context->RefCount.fetch_sub(1) == 1)
			{
				delete Context;
			}

			Other.Context->RefCount += 1;
			Context = Other.Context;
			return *this;
		}

		~UIContextRef()
		{
			if (Context && Context->RefCount.fetch_sub(1) == 1)
			{
				delete Context;
			}
		}

		T* Get() const
		{
			return Context;
		}

		T* operator->() const
		{
			return Context;
		}

	private:
		T* Context;
	};

	class UIThreadContext
	{
	public:
		~UIThreadContext();

		std::atomic<uint32_t> RefCount;

		kui::Window* ActiveWindow = nullptr;
		bool HasMainWindow = false;

		static UIThreadContext* Get();
	};

	class UIContext
	{
	public:

		static constexpr uint64_t LIBRARY_VERSION_ID = 0;

		uint64_t VersionIdentifier = LIBRARY_VERSION_ID;
		std::atomic<uint32_t> RefCount;
		std::vector<resource::UIResourceProvider*> Resources;

		std::string AppId;
		bool ResourceErrorOnFail = true;
		UIContext();
		virtual ~UIContext();

		static UIContext* Get();
		static bool SetActive(UIContext* NewContext);

		std::vector<kui::Window*> ActiveWindows;
	};
}