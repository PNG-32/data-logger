#ifndef BITS_DATALOGGER_H
#define BITS_DATALOGGER_H

#include "types.hpp"
#include "sensor.hpp"
#include "clock.hpp"
#include "ldr.hpp"
#include "databank.hpp"

namespace Bits {
	struct DataLogger {
		struct PACKED Log {
			uint32			timestamp;
			Sensor::Value	value;
		};

		DataLogger(uint8 const clockPin, uint8 const sensorPin, uint8 const ldrPin):
			clock(clockPin, 0),
			sensor(sensorPin, clock.end()),
			ldr(ldrPin, sensor.end()),
			logger(ldr.end()) {
			if (!clock.adjusted())
				clock.adjust({F(__DATE__), F(__TIME__)});
		}

	private:
		Clock			clock;
		Sensor			sensor;
		LDR				ldr;
		DataBank<Log>	logger;
	};
}

#endif
