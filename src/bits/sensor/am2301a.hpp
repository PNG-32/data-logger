#ifndef BITS_SENSOR_AM2301A_H
#define BITS_SENSOR_AM2301A_H

#include "../avrx/avrx.hpp"

/// @brief Sensors.
namespace Bits::Sensor {
	struct AM2301A {
		AM2301A(byte const pin): pin(pin) {}

		void poll() {
			using namespace AVRX;
			IO::mode(pin, IO::Mode::AIOM_OUTPUT);
			IO::write(pin, true);
			Wait::millis(2);
			IO::write(pin, false);
			IO::mode(pin, IO::Mode::AIOM_INPUT);
		}

	private:
		sword hum;
		sword temp;
		word crc;

		byte const pin;
	};
}

#endif