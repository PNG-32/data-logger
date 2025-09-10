#ifndef BITS_WAIT_H
#define BITS_WAIT_H

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
}

#endif