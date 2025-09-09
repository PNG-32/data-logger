#include "bits/bits.hpp"

void init() {
	Bits::AVRX::IO::mode(Bits::AVRX::IO::LED_PIN, Bits::AVRX::IO::Mode::AIOM_OUTPUT);
}

void mainLoop() {
	Bits::AVRX::IO::write(Bits::AVRX::IO::LED_PIN, true);
	Bits::AVRX::Wait::millis(500);
	Bits::AVRX::IO::write(Bits::AVRX::IO::LED_PIN, false);
	Bits::AVRX::Wait::millis(2000);
}

void update() {
	Bits::AVRX::yield();
}