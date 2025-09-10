#ifndef BITS_SENSOR_H
#define BITS_SENSOR_H

#include "types.hpp"
#include "record.hpp"

#include <DHT.h>

namespace Bits {
	struct Sensor {
		enum class Unit: char {
			BSU_CELSIUS		= 'C',
			BSU_FARENHEIT	= 'F',
			BSU_KELVIN		= 'K',
		};

		struct PACKED Value {
			int16 temperature;
			int16 humidity;
		};

		struct PACKED Threshold {
			Value min, max;
		};

		struct PACKED Info {
			Unit unit = Unit::BSU_CELSIUS;
		};

		Sensor(avr_pin const pin, eeprom_address const info):
			pin(pin), info(info), dht(pin, DHT22) {
		}

		Value read() const {
			float const t = dht.readTemperature() * 100;
			Value v{t, dht.readHumidity() * 100};
			switch (info.get().unit) {
				default:
				case Unit::BSU_CELSIUS: break;
				case Unit::BSU_FARENHEIT:	v.temperature = (t * 1.8 + 32.0) * 100;	break;
				case Unit::BSU_KELVIN:		v.temperature -= 27315;					break;
			}
			return v;
		}

		Value readRaw() const {
			return Value{
				dht.readTemperature()	* 100,
				dht.readHumidity()		* 100
			};
		}

		void setUnit(Unit const unit) {
			Info i = info;
			i.unit = unit;
			info = i;
		}

		Unit getUnit() const {
			return info.get().unit;
		}

		uint16 address() const	{return info.address();				}
		uint16 end() const		{return address()	+ sizeof(Info);	}

	private:
		uint8 const		pin;
		Record<Info>	info;
		DHT				dht;
	};
}

#endif
