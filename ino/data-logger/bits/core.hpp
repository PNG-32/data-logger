#ifndef BITS_CORE_H
#define BITS_CORE_H

#include "types.hpp"

/// @brief Helper classes & functions.
namespace Bits {
	/// @brief Delay functions.
	namespace Wait {
		/// @brief Delays a given number of microseconds.
		/// @param time Number of microseconds to wait for.
		[[gnu::always_inline]]
		inline void micros(uint32 const time)	{_delay_us(time);		}

		/// @brief Delays a given number of milliseconds.
		/// @param time Number of milliseconds to wait for.
		[[gnu::always_inline]]
		inline void millis(uint32 const time)	{_delay_ms(time);		}

		/// @brief Delays a given number of seconds.
		/// @param time Number of seconds to wait for.
		[[gnu::always_inline]]
		inline void seconds(float const time)	{millis(time * 1000);	}
	}

	/// @brief Resets the arduino.
	/// @warning Effectively crashes it! Do not use this without knowing what you're doing!
	[[gnu::always_inline, noreturn]]
	inline void reset() {
		As<void(void)>* crash = 0;
		crash();
	}
}

#endif