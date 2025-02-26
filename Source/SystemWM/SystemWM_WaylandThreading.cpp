#if KLEMMUI_WITH_WAYLAND && !KLEMMUI_CUSTOM_SYSTEMWM
#include "SystemWM_WaylandThreading.h"
#include <utility>
#include <mutex>
#include <future>
#include <vector>

using namespace kui::systemWM;

static std::mutex MainThreadMutex;
static std::vector<std::pair<std::function<void()>, std::promise<void>*>> MainThreadTasks;

void wlThreading::RunOnMainThread(std::function<void()> fn)
{
	if (IsMainThread())
	{
		fn();
		return;
	}

	std::unique_lock g{ MainThreadMutex };
	MainThreadTasks.push_back({ fn, nullptr });
}

void wlThreading::AwaitRunOnMainThread(std::function<void()> fn)
{
	if (IsMainThread())
	{
		fn();
		return;
	}

	std::promise<void> p;

	{
		std::unique_lock g{ MainThreadMutex };
		MainThreadTasks.push_back({ fn, &p });
	}

	p.get_future().wait();
}

void kui::systemWM::wlThreading::UpdateMainThread()
{
	std::lock_guard g{ MainThreadMutex };

	for (auto& i : MainThreadTasks)
	{
		i.first();
		if (i.second)
			i.second->set_value();
	}
	MainThreadTasks.clear();
}

bool kui::systemWM::wlThreading::IsMainThread()
{
	return getpid() == gettid();
}
#endif