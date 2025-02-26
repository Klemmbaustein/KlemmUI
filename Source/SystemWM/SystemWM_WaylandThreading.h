#pragma once
#include <condition_variable>
#include <functional>

// libdecor is *really* dumb
namespace kui::systemWM::wlThreading
{
	void RunOnMainThread(std::function<void()> fn);
	void AwaitRunOnMainThread(std::function<void()> fn);

	void UpdateMainThread();
	bool IsMainThread();
}