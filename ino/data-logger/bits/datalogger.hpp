#ifndef BITS_DATALOGGER_H
#define BITS_DATALOGGER_H

#include "types.hpp"
#include "sensor.hpp"
#include "clock.hpp"
#include "databank.hpp"

namespace Bits {
	struct DataLogger {
		struct Log {
			uint32			timestamp;
			Sensor::Value	value;
		};

		DataLogger(uint8 const clockPin, uint8 const sensorPin):
			clock(clockPin, 0),
			sensor(sensorPin, clock.address() + sizeof(Clock::Info)),
			logger(sensor.address() + sizeof(Sensor::Info)) {
			if (!clock.adjusted())
				clock.adjust({F(__DATE__), F(__TIME__)});
		}

	private:
		Clock			clock;
		Sensor			sensor;
		DataBank<Log>	logger;
	};
}

#endif
