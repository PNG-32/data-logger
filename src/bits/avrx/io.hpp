#ifndef BITS_AVRX_IO_H
#define BITS_AVRX_IO_H

#include "core.hpp"

/// @brief MCU input/output operations.
namespace Bits::AVRX::IO {
	/// @brief Pin mode.
	enum class Mode {
		/// @brief Input.
		AIOM_INPUT,
		/// @brief Output.
		AIOM_OUTPUT
	};

	/// @brief Blitter functions.	
	namespace Blit {
		/// @brief Sets a given mask of digital pins to a given mode.
		/// @param pins Pin mask.
		/// @param mode Mode to set pins to.
		inline void dpms(word const pins, Mode const mode) {
			switch (mode) {
				case Mode::AIOM_OUTPUT:
					DDRD |= (pins & 0xFF);
					DDRB |= ((pins >> 8) & 0x3F);
				break;
				case Mode::AIOM_INPUT:
					DDRD &= ~(pins & 0xFF);
					DDRB &= ~((pins >> 8) & 0x3F);
				break;
			}
		}

		/// @brief Sets a given mask of analog pins to a given mode.
		/// @param pins Pin mask.
		/// @param mode Mode to set pins to.
		inline void apms(byte const pins, Mode const mode) {
			switch (mode) {
				case Mode::AIOM_INPUT:	DDRC &= ~pins;	break;
				case Mode::AIOM_OUTPUT:	DDRC |= pins;	break;
			}
		}


		/// @brief Returns the current state of every digital pin.
		/// @return State of every digital pin.
		inline word dpgs() {
			return (
				static_cast<word>(PIND)
				| (static_cast<word>(PINB) << 8)
			);
		}
		
		/// @brief Sets a given mask of digital pins to a state.
		/// @param pins Pin mask.
		/// @param state State to set pins to.
		inline void dpds(word const pins, bool const state) {
			if (state) {
				PORTD |= (pins & 0xFF);
				PORTB |= ((pins >> 8) & 0x3F);
			} else {
				PORTD &= ~(pins & 0xFF);
				PORTB &= ~((pins >> 8) & 0x3F);
			}
		}

		/// @brief Sets a given mask of analog pins to a state.
		/// @param pins Pin mask.
		/// @param state State to set pins to.
		inline void apds(byte const pins, bool const state) {
			if (state) {
				PORTC |= (pins & 0x8F);
			} else {
				PORTC &= ~(pins & 0x8F);
			}
		}
	}
	
	/// @brief Pulse-Width Modulation.
	namespace PWM {
		/// @brief Enables PWM output.
		inline void enable() {
			ATOMIC_SAFE Globals::pwmOut |= 0x8000;
		}

		/// @brief Disables PWM output.
		inline void disable() {
			ATOMIC_SAFE Globals::pwmOut &= ~0x8000;
			Blit::dpds(Globals::pwmOut, false);
		}

		/// @brief Sets the PWM cycle.
		/// @param cycle PWM cycle, on a range of  0 (0%) to 255 (100%).
		inline void cycle(byte const cycle) {
			if (!cycle) disable();
			else ATOMIC_SAFE OCR0A = cycle;
		}

		/// @brief Sets the PWM pins to use.
		/// @param pins Pin mask to use.
		inline void pins(word const pins) {
			disable();
			ATOMIC_SAFE {
				auto const pm = pins & 0x7FFF;
				if (pm) {
					Blit::dpds(pm, true);
					Globals::pwmOut &= 0x8000;
					Globals::pwmOut |= pm;
				}
			}
		}

		/// @brief Sets the PWM pin to use.
		/// @param pin Pin to use.
		inline void pin(byte const pin) {
			pins(1 << (pin & 0xf));
		}

		/// @brief Initializes the PWM subsystem.
		inline void init() {
			ATOMIC_SAFE {
				TCCR0A	|= (1 << COM0A1) | (1 << WGM01) | (1 << WGM00);
				TCCR0B	|= (1 << CS01) | (1 << CS00);
			}
		}
	}

	/// @brief Initializes the core input/output subsystem.
	inline void init() {
		ATOMIC_SAFE {
			// Initialize ADC
			ADMUX	|= (1<<REFS0);
			ADCSRA	|= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADEN);
		}
	}

	/// @brief Sets a given pin to a given mode.
	/// @param pin Pin to set mode for.
	/// @param mode Mode to set pin to.
	/// @note Analog pins start from 14.
	inline void mode(byte const pin, Mode const mode) {
		if (pin < 14)	Blit::dpms(1 << (pin & 0xF), mode);
		else			Blit::apms(1 << (pin & 0x7), mode);
	}

	/// @brief Returns the current state of a pin.
	/// @param pin Pin to get state for.
	/// @return Pin state.
	/// @note Analog pins start from 14.
	inline word read(byte const pin) {
		if (pin < 14)
			return (Blit::dpgs() >> pin) & 0b1;
		else {
			ADMUX = (ADMUX & 0xf8) | (pin & 0x07);
			ADCSRA |= (1 << ADSC);
			while (ADCSRA & (1 << ADSC)) yield();
			return ADC;
		}
	}

	/// @brief Sets a given pin to a given state.
	/// @param pin Pin to set mode for.
	/// @param state State to set pin to.
	/// @note Analog pins start from 14.
	inline void write(byte const pin, bool const state) {
		if (pin < 14)
			Blit::dpds(1 << (pin & 0xF), state);
		else {
			Blit::apds(1 << (pin & 0x7), state);
		}
	}

	/// @brief Converts an analog pin to its equivalent ID.
	/// @param pin Analog pin.
	/// @return Pin ID.	
	consteval byte analog(byte const pin) {return pin + 14;}

	/// @brief Built-in LED pin.
	constexpr byte const LED_PIN = 13;

	/// @brief Analog pin 0.
	constexpr byte const A0 = analog(0);
	/// @brief Analog pin 1.
	constexpr byte const A1 = analog(1);
	/// @brief Analog pin 2.
	constexpr byte const A2 = analog(2);
	/// @brief Analog pin 3.
	constexpr byte const A3 = analog(3);
	/// @brief Analog pin 4.
	constexpr byte const A4 = analog(4);
	/// @brief Analog pin 5.
	constexpr byte const A5 = analog(5);
	/// @brief Analog pin 6.
//	constexpr byte const A6 = analog(6);
	/// @brief Analog pin 7.
//	constexpr byte const A7 = analog(7);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wliteral-suffix"
/// @brief Input/output literals
namespace Bits::Literals::AVRX::IO {
	consteval byte operator ""analog	(uint64 const pin)	{return Bits::AVRX::IO::analog(pin);}
	namespace Shorthand {
		consteval byte operator ""a		(uint64 const pin)	{return Bits::AVRX::IO::analog(pin);}
	}
}
#pragma GCC diagnostic pop

ISR(TIMER0_COMPA_vect) {
	if (Bits::AVRX::Globals::pwmOut & 0x8000) {
		Bits::AVRX::IO::Blit::dpds(
			Bits::AVRX::Globals::pwmOut,
			!(Bits::AVRX::IO::Blit::dpgs() & Bits::AVRX::Globals::pwmOut)
		);
	}
}

#endif