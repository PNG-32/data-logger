#ifndef BITS_AVRX_MAIN_H
#define BITS_AVRX_MAIN_H

#include "core.hpp"
#include "io.hpp"

/// @brief Initialization.
void init();

/// @brief Main loop.
void mainLoop();

/// @brief Update loop.
void update();

/// @brief Relinquishes execution to the update loop for a given amount of milliseconds.
/// @param millis Milliseconds to wait for.
/// @warning This function should NEVER be called from the update loop!
void wait(uint32 const millis) {
	uint32 const current = Bits::AVRX::millis();
	while ((Bits::AVRX::millis() - current) < millis)
		update();
}

int main() {
	Bits::AVRX::init();
	Bits::AVRX::IO::init();
	Bits::AVRX::IO::PWM::init();
	init();
	while (true) {
		Bits::AVRX::update();
		mainLoop();
	}
	return 0;
}

#endif