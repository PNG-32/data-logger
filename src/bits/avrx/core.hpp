#ifndef BITS_AVRX_CORE_H
#define BITS_AVRX_CORE_H

#if defined(MPU_atmega168p)
#define F_CPU 8000000UL
constexpr auto const EEPROM_SIZE	= 512;
#elif defined(MPU_atmega328p)
#define F_CPU 16000000UL
constexpr auto const EEPROM_SIZE	= 1024;
#else
#error "Unknown/Unsupported MPU!"
#endif

#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>

#include <util/delay.h>
#include <util/atomic.h>

#include "../types.hpp"

#define ATOMIC_SAFE ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
#define ATOMIC_FAST ATOMIC_BLOCK(ATOMIC_FORCEON)

/// @brief AVR Extensions.
namespace Bits::AVRX {
	/// @brief Yields precisely one CPU cycle.
	[[gnu::always_inline]]
	inline void yield() {asm volatile ("nop");}

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

	struct Globals {
		inline static uint32	millis	= 0;
		inline static word		pwmOut	= 0;
	};

	void init() {
		ATOMIC_SAFE {
			TCCR1A	= 0;
			TCCR1B	= (1 << WGM12) | (1 << CS10);
			OCR1A	= ((F_CPU / 1000) / 8) - 1;
			TIMSK1	= (1 << OCIE1A);
		}
	}

	uint32 millis() {
		uint32 millis;
		ATOMIC_SAFE millis = Globals::millis;
		return millis;
	}

	void update() {
	}
}

ISR(TIMER1_COMPA_vect) {
	++Bits::AVRX::Globals::millis;
}

#endif