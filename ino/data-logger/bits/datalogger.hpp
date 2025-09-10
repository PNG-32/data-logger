#ifndef BITS_DATALOGGER_H
#define BITS_DATALOGGER_H

#include "types.hpp"
#include "sensor.hpp"
#include "clock.hpp"
#include "ldr.hpp"
#include "databank.hpp"
#include "display.hpp"

namespace Bits {
	struct DataLogger {
		struct PACKED Info {
			uint8 firstInit: 1;
		};

		struct PACKED Log {
			uint32			timestamp;
			Sensor::Value	value;
			uint8_t			luminosity;
		};

		struct LEDPins {
			avr_pin
				red,
				yellow,
				green
			;
		};

		DataLogger(
			avr_pin const	sensorPin	= 2,
			avr_pin const	ldrPin		= A0,
			LEDPins const&	ledPins		= {3, 4, 5}
		):
			info(0),
			clock(info.end()),
			sensor(sensorPin, clock.end()),
			ldr(ldrPin, sensor.end()),
			db(ldr.end()),
			display(),
			led(ledPins) {}

		void begin() {
			Serial.begin(9600);
			clock.begin();
			ldr.begin();
			display.begin();
			if (!clock.adjusted())
				clock.adjust({F(__DATE__), F(__TIME__)});
			pinMode(led.red,	OUTPUT);
			pinMode(led.yellow,	OUTPUT);
			pinMode(led.green,	OUTPUT);
		}

		void update() {
			bool const inTheDangerZone = sensor.inTheDangerZone() || ldr.inTheDangerZone();
			if (inTheDangerZone && !cooldown) {
				//db.record();
				cooldown = 1000;
			} else --cooldown;
		}

	private:
		Record<Info>	info;
		Clock			clock;
		Sensor			sensor;
		LDR				ldr;
		DataBank<Log>	db;
		Display			display;
		uint16			cooldown = 0;
		LEDPins			led;

	};
}

#endif
