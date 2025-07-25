#pragma once
#include <cstdint>

namespace kui
{
	/**
	* @brief
	* A timer class that accurately measures time since it's creation.
	*
	* This class uses the CPU performance counter to accurately measure time.
	*/
	struct Timer
	{
		Timer();
		/**
		* @brief
		* Resets the timer.
		*/
		void Reset();
		/**
		* @brief
		* Returns the time in seconds since construction or the last call to Reset().
		*/
		float Get() const;
	private:
		uint64_t Time = 0;
	};

}