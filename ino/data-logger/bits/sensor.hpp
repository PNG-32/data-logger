#ifndef BITS_SENSOR_H
#define BITS_SENSOR_H

#include "types.hpp"
#include "record.hpp"

#include <RTClib.h>

namespace Bits {
	struct Sensor {
		enum class Unit {
			BSU_KELVIN,
			BSU_CELSIUS,
			BSU_FARENHEIT,
		};

		struct Info {
			Unit unit = Unit::BSU_KELVIN;
		};

		struct Value {
			int16 temperature;
			int16 humidity;
		};

		Sensor(uint8 const pin, uint16 const info):
			pin(pin), info(info), dht(pin, DHT22) {
		}

		Value read() {
			float 
				t = dht.readTemperature()	* 100,
				h = dht.readHumidity()		* 100
			;
			Value v{t, h};
			switch (info.get().unit) {
				default: break;
				case Unit::BSU_KELVIN:		v.temperature -= 27315;					break;
				case Unit::BSU_CELSIUS:												break;
				case Unit::BSU_FARENHEIT:	v.temperature = (v * 1.8 + 32.0) * 100;	break;
			}
			return v;
		}

	private:
		uint8 const		pin;
		Record<Info>	info;
		DHT				dht;
	};
}

#endif
