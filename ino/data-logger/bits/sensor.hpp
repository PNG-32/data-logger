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
			Unit		unit		= Unit::BSU_CELSIUS;
			Threshold	threshold;
		};

		Sensor(avr_pin const pin, eeprom_address const info):
			pin(pin), info(info), dht(pin, DHT22) {
		}

		void begin() {
			dht.begin();
			info.begin();
		}

		Value read() const {
			return toCurrentUnit(readRaw());
		}

		Value readRaw() const {
			return Value{
				dht.readTemperature()	* 100,
				dht.readHumidity()		* 100
			};
		}

		Value toCurrentUnit(Value v) const {
			switch (info.get().unit) {
				default:
				case Unit::BSU_CELSIUS: break;
				case Unit::BSU_FARENHEIT:	v.temperature = (v.temperature * 1.8 + 3200);	break;
				case Unit::BSU_KELVIN:		v.temperature += 27315;							break;
			}
			return v;
		}
		
		constexpr int16 toCelcius(int16 const value, Unit const unit) {
			switch (unit) {
				default:
				case Unit::BSU_CELSIUS:		return value;
				case Unit::BSU_FARENHEIT:	return (value - 3200) * (5.0/9.0);
				case Unit::BSU_KELVIN:		return value - 27315;
			}
		}

		void setUnit(Unit const unit) {
			Info i = info;
			i.unit = unit;
			info = i;
		}

		Unit getUnit() const {
			return info.get().unit;
		}

		void setThreshold(Threshold const& threshold) {
			Info i = info;
			i.threshold = threshold;
			info = i;
		}

		Threshold getThreshold() const {
			return info.get().threshold;
		}

		bool inTheDangerZone() const {
			return !inTheSafeZone();
		}

		bool inTheSafeZone() const {
			auto const v			= readRaw();
			auto const threshold	= info.get().threshold;
			return (
				isTemperatureOK(v.temperature, threshold)
			&&	isHumidityOK(v.humidity, threshold)
			);
		}

		uint16 address() const	{return info.address();				}
		uint16 end() const		{return address()	+ sizeof(Info);	}

	private:
		constexpr static bool isTemperatureOK(int16 const t, Threshold const& threshold) {
			return (
				threshold.min.temperature <= t
			&&	t <= threshold.max.temperature
			);
		}

		constexpr static bool isHumidityOK(int16 const h, Threshold const& threshold) {
			return (
				threshold.min.humidity <= h
			&&	h <= threshold.max.humidity
			);
		}

		uint8 const		pin;
		Record<Info>	info;
		DHT				dht;
	};
}

#endif
