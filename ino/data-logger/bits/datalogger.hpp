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
		struct PACKED Log {
			uint32			timestamp;
			Sensor::Value	value;
			uint8_t			luminosity;
		};

		DataLogger(uint8 const sensorPin = 2, uint8 const ldrPin = A0):
			clock(0),
			sensor(sensorPin, clock.end()),
			ldr(ldrPin, sensor.end()),
			db(ldr.end()),
			display() {}

		void begin() {
			Serial.begin(9600);
			clock.begin();
			ldr.begin();
			display.begin();
			if (!clock.adjusted())
				clock.adjust({F(__DATE__), F(__TIME__)});
		}

		void update() {
			bool const inTheDangerZone = sensor.inTheDangerZone() || ldr.inTheDangerZone();
			if (inTheDangerZone && !cooldown) {
				//db.record();
				cooldown = 1000;
			} else --cooldown;
		}

	private:
		Clock			clock;
		Sensor			sensor;
		LDR				ldr;
		DataBank<Log>	db;
		Display			display;
		uint16			cooldown = 0;
	};
}

#endif
