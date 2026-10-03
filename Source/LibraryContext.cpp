#include <kui/LibraryContext.h>
#include <iostream>

using namespace kui;

static UIContextRef<UIContext> CurrentContext = new UIContext();
static thread_local UIContextRef<UIThreadContext> ThreadContext = nullptr;

kui::UIContext::UIContext()
{
}

kui::UIContext::~UIContext()
{
}

UIContext* kui::UIContext::Get()
{
	return CurrentContext.Get();
}

bool kui::UIContext::SetActive(UIContext* NewContext)
{
	if (NewContext->VersionIdentifier != LIBRARY_VERSION_ID)
	{
		return false;
	}
	CurrentContext = UIContextRef(NewContext);

	return true;
}

kui::UIThreadContext::~UIThreadContext()
{
}

UIThreadContext* kui::UIThreadContext::Get()
{
	if (!ThreadContext.Get())
	{
		ThreadContext = UIContextRef<UIThreadContext>(new UIThreadContext());
	}

	return ThreadContext.Get();
}
