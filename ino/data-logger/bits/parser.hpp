#ifndef BITS_PARSER_H
#define BITS_PARSER_H

#include "databank.hpp"
#include "clock.hpp"
#include "sensor.hpp"
#include "ldr.hpp"

namespace Bits {
	template <class T>
	struct Parser {
		DataBank<T>&	db;
		Clock&			clock;
		Sensor&			sensor;
		LDR&			ldr;

		enum class Error {
			BPE_OK,
			BPE_NOT_A_COMMAND,
			BPE_MISSING_COMMAND,
			BPE_INVALID_COMMAND,
			BPE_MISSING_ARGUMENT,
			BPE_INVALID_ARGUMENT,
			BPE_MISSING_VALUE,
			BPE_INVALID_VALUE
		};

		Error evaluate(String const& str) const {
			if (!str.length()) return Error::BPE_MISSING_COMMAND;
			String const command = arg(str, 0);
			if (command[0] != '@') return Error::BPE_NOT_A_COMMAND;
			if (
				command == "@set"
			||	command == "@s"
			) {
				String const param = arg(str, 1);
				if (!param.length()) return Error::BPE_MISSING_ARGUMENT;
				if (
					param == "temperature"
				||	param == "temp"
				||	param == "t"
				) {
					String const min	= arg(str, 2);
					String const max	= arg(str, 3);
					String const unit	= arg(str, 4);
					if (
						!min.length()
					||	!max.length()
					) return Error::BPE_MISSING_VALUE;
					auto const minVal = min.toFloat(), maxVal = max.toFloat();
					if (maxVal <= minVal) return Error::BPE_INVALID_VALUE;
					auto v = sensor.getThreshold();
					Bits::Sensor::Unit base;
					if (
						unit.length()
					||	unit == "celcius"
					||	unit == "celsius"
					||	unit == "centigrade"
					||	unit == "cel"
					||	unit == "c"
					) base = Bits::Sensor::Unit::BSU_CELSIUS;
					else if (
						unit == "farenheit"
					||	unit == "far"
					||	unit == "f"
					) base = Bits::Sensor::Unit::BSU_FARENHEIT;
					else if (
						unit == "kelvin"
					||	unit == "kel"
					||	unit == "k"
					) base = Bits::Sensor::Unit::BSU_KELVIN;
					v.min.temperature = sensor.toCelcius(minVal * 100, base);
					v.max.temperature = sensor.toCelcius(maxVal * 100, base);
					sensor.setThreshold(v);
				} else if (
					param == "humidity"
				||	param == "hum"
				||	param == "h"
				) {
					String const min = arg(str, 2);
					String const max = arg(str, 3);
					if (
						!min.length()
					||	!max.length()
					) return Error::BPE_MISSING_VALUE;
					auto const minVal = min.toFloat(), maxVal = max.toFloat();
					if (maxVal <= minVal) return Error::BPE_INVALID_VALUE;
					auto v = sensor.getThreshold();
					v.min.humidity = minVal * 100;
					v.max.humidity = maxVal * 100;
					sensor.setThreshold(v);
				} else if (
					param == "datetime"
				||	param == "dt"
				||	param == "d"
				) {
					String const dt = arg(str, 2);
					if (!dt.length())
						return Error::BPE_MISSING_VALUE;
					auto const date = DateTime(dt.c_str());
					if (!date.isValid()) return Error::BPE_INVALID_VALUE;
					clock.adjust(date);
				} else if (
					param == "timezone"
				||	param == "zone"
				||	param == "z"
				) {
					String const zone = arg(str, 2);
					if (!zone.length())
						return Error::BPE_MISSING_VALUE;
					String const hour = arg(zone, 0, ':'), minute = arg(zone, 1, ':');
					if (
						!hour.length()
					||	!minute.length()
					) return Error::BPE_MISSING_VALUE;
					int8 const z = (hour.toInt() * 4) + minute.toInt() / 15;
					clock.setTimeZone(z);
				} else if (
					param == "luminosity"
				||	param == "ldr"
				||	param == "l"
				) {
					String const min = arg(str, 2);
					String const max = arg(str, 3);
					if (
						!min.length()
					||	!max.length()
					) return Error::BPE_MISSING_VALUE;
					auto const minVal = min.toInt(), maxVal = max.toInt();
					if (maxVal <= minVal) return Error::BPE_INVALID_VALUE;
					auto v = ldr.getThreshold();
					v.min = minVal;
					v.max = maxVal;
					ldr.setThreshold(v);
				} else if (
					param == "rawadjustment"
				||	param == "rawadjust"
				||	param == "radj"
				||	param == "ra"
				||	param == "a"
				) {
					String const min = arg(str, 2);
					String const max = arg(str, 3);
					if (
						!min.length()
					||	!max.length()
					) return Error::BPE_MISSING_VALUE;
					auto const minVal = min.toInt(), maxVal = max.toInt();
					if (maxVal <= minVal) return Error::BPE_INVALID_VALUE;
					auto v = ldr.getAdjustment();
					v.min = minVal;
					v.max = maxVal;
					ldr.setAdjustment(v);
				} else return Error::BPE_INVALID_ARGUMENT;
			} else if (
				command == "@calibrate"
			||	command == "@c"
			) {
				// TODO: calibration
			} else if (
				command == "@view"
			||	command == "@v"
			) {
				String const param = arg(str, 1);
				if (!param.length()) return Error::BPE_MISSING_ARGUMENT;
				if (
					param == "log"
				||	param == "l"
				) {
					for (usize i = 0; i < db.size(); ++i)
						Serial.println(db[i].toString(sensor));
				} else if (
					param == "thresholds"
				||	param == "t"
				) {
					printSensorThresholds();
					printLDRThresholds();
				} else if (
					param == "timezone"
				||	param == "z"
				) {
					printTimezone();
				} else if (
					param == "rawadjustment"
				||	param == "rawadjust"
				||	param == "radj"
				||	param == "ra"
				||	param == "a"
				) {
					printLDRAdjustment();
				} else return Error::BPE_INVALID_ARGUMENT;
			} else return Error::BPE_INVALID_COMMAND;
			return Error::BPE_OK;
		}

	private:
		void printSensorThresholds() const {
			Serial.print("Temperature (");
			Serial.print(static_cast<char>(sensor.getUnit()));
			Serial.println("):");
			auto const t = sensor.getThreshold();
			auto const
				min = sensor.toCurrentUnit(t.min),
				max = sensor.toCurrentUnit(t.max)
			; 
			Serial.print("    MIN: ");
			Serial.println(min.temperature / 100.0);
			Serial.print("    MAX: ");
			Serial.println(min.temperature / 100.0);
			Serial.println("Humidity (%):");
			Serial.print("    MIN: ");
			Serial.println(min.humidity / 100.0);
			Serial.print("    MAX: ");
			Serial.println(min.humidity / 100.0);
		}

		void printLDRThresholds() const {
			auto const t = ldr.getThreshold();
			Serial.println("Luminosity (%):");
			Serial.print("    MIN: ");
			Serial.println(t.min);
			Serial.print("    MAX: ");
			Serial.println(t.max);
		}

		void printLDRAdjustment() const {
			auto const a = ldr.getAdjustment();
			Serial.println("Adjustment (RAW):");
			Serial.print("    MIN: ");
			Serial.println(a.min);
			Serial.print("    MAX: ");
			Serial.println(a.max);
		}

		void printTimezone() const {
			Serial.print("Timezone: ");
			auto const z = clock.getTimeZone();
			Serial.print(z / 4);
			Serial.print(":");
			Serial.println((static_cast<uint8>(z < 0 ? -z : z) % 4) * 15);
		}

		// https://stackoverflow.com/questions/9072320/split-string-into-string-array
		static String arg(String const& data, usize const index, char const sep = ' ') {
			ssize found = 0;
			ssize strIndex[] = {0, -1};
			ssize maxIndex = data.length()-1;
			for(int i=0; i<=maxIndex && found<=index; i++){
				if(data.charAt(i)==sep || i==maxIndex){
					found++;
					strIndex[0] = strIndex[1]+1;
					strIndex[1] = (i == maxIndex) ? i+1 : i;
				}
			}
			return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
		}
	};
}

#endif