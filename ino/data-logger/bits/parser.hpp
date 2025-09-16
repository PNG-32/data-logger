#ifndef BITS_PARSER_H
#define BITS_PARSER_H

#include "databank.hpp"
#include "clock.hpp"
#include "sensor.hpp"
#include "ldr.hpp"

/// @brief Helper classes & functions.
namespace Bits {
	/// @brief Serial interface parser.
	/// @tparam T Data bank entry type.
	template <class T>
	struct Parser {
		/// @brief Data bank.
		DataBank<T>&	db;
		/// @brief Clock.
		Clock&			clock;
		/// @brief Sensor.
		Sensor&			sensor;
		/// @brief LDR.
		LDR&			ldr;
		
		/// @brief Response.
		struct Response {
			/// @brief Response type.
			enum class Type: uint8 {
				BPRT_OK,
				BPRT_NOT_A_COMMAND,
				BPRT_MISSING_COMMAND,
				BPRT_INVALID_COMMAND,
				BPRT_MISSING_ARGUMENT,
				BPRT_INVALID_ARGUMENT,
				BPRT_MISSING_VALUE,
				BPRT_INVALID_VALUE
			};
			
			/// @brief Response type.
			Type	type	= Type::BPRT_OK;
			/// @brief Response ID.
			uint32	id		= 0;

			/// @brief Empty constructor.
			constexpr Response() {}

			/// @brief Other constructors.	
			constexpr Response(Type const type):					type(type)			{}
			constexpr Response(uint8 const id):						id(id)				{}
			constexpr Response(Type const type, uint8 const id):	type(type), id(id)	{}
			constexpr Response(uint8 const id, Type const type):	type(type), id(id)	{}
		};

		/// @brief Evaluates a command string.
		/// @param str Command string to evaluate.
		/// @return Response.
		Response evaluate(String const& str) const {
			if (!str.length()) return {Response::Type::BPRT_MISSING_COMMAND};
			String const command = arg(str, 0);
			if (command[0] != '@') return {Response::Type::BPRT_NOT_A_COMMAND};
			if (
				command == "@set"
			||	command == "@s"
			) {
				String const param = arg(str, 1);
				if (!param.length()) return {Response::Type::BPRT_MISSING_ARGUMENT};
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
					) return {Response::Type::BPRT_MISSING_VALUE};
					auto const minVal = min.toFloat(), maxVal = max.toFloat();
					if (maxVal <= minVal) return {Response::Type::BPRT_INVALID_VALUE};
					auto v = sensor.getThreshold();
					Bits::Sensor::Unit base;
					if (
						!unit.length()
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
					return {2};
				} else if (
					param == "unit"
				||	param == "u"
				) {
					String const unit	= arg(str, 2);
					if (!unit.length())
						return {Response::Type::BPRT_MISSING_VALUE};
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
					else return {Response::Type::BPRT_INVALID_VALUE};
					sensor.setUnit(base);
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
					) return {Response::Type::BPRT_MISSING_VALUE};
					auto const minVal = min.toFloat(), maxVal = max.toFloat();
					if (maxVal <= minVal) return {Response::Type::BPRT_INVALID_VALUE};
					auto v = sensor.getThreshold();
					v.min.humidity = minVal * 100;
					v.max.humidity = maxVal * 100;
					sensor.setThreshold(v);
					return {3};
				} else if (
					param == "datetime"
				||	param == "dt"
				||	param == "d"
				) {
					String const dt = arg(str, 2);
					if (!dt.length())
						return {Response::Type::BPRT_MISSING_VALUE};
					auto const date = DateTime(dt.c_str());
					if (!date.isValid()) return {Response::Type::BPRT_INVALID_VALUE};
					clock.adjust(date);
				} else if (
					param == "timezone"
				||	param == "zone"
				||	param == "z"
				) {
					String const zone = arg(str, 2);
					if (!zone.length())
						return {Response::Type::BPRT_MISSING_VALUE};
					String const hour = arg(zone, 0, ':'), minute = arg(zone, 1, ':');
					if (
						!hour.length()
					||	!minute.length()
					) return {Response::Type::BPRT_MISSING_VALUE};
					int8 const z = (hour.toInt() * 4) + minute.toInt() / 15;
					clock.setTimeZone(z);
					return {1};
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
					) return {Response::Type::BPRT_MISSING_VALUE};
					auto const minVal = min.toInt(), maxVal = max.toInt();
					if (maxVal <= minVal) return {Response::Type::BPRT_INVALID_VALUE};
					auto v = ldr.getThreshold();
					v.min = minVal;
					v.max = maxVal;
					ldr.setThreshold(v);
					return {4};
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
					) return {Response::Type::BPRT_MISSING_VALUE};
					auto const minVal = min.toInt(), maxVal = max.toInt();
					if (maxVal <= minVal) return {Response::Type::BPRT_INVALID_VALUE};
					auto v = ldr.getAdjustment();
					v.min = minVal;
					v.max = maxVal;
					ldr.setAdjustment(v);
					return {5};
				} else return {Response::Type::BPRT_INVALID_ARGUMENT};
			} else if (
				command == "@calibrate"
			||	command == "@c"
			) {
				// TODO: calibration
					return {5};
			} else if (
				command == "@view"
			||	command == "@v"
			) {
				String const param = arg(str, 1);
				if (!param.length()) return {Response::Type::BPRT_MISSING_ARGUMENT};
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
				||	param == "zone"
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
				} else return {Response::Type::BPRT_INVALID_ARGUMENT};
			} else if (
				command == "@wipe"
			||	command == "@w"
			) {
				String const param = arg(str, 1);
				if (!param.length()) return {Response::Type::BPRT_MISSING_ARGUMENT};
				if (param == "absolutelyeverything") {
					for (usize i = 0; i < EEPROM.length(); ++i)
						EEPROM[i] = 0;
					return {-1};
				} else if (param == "log")
					db.clear();
				else return {Response::Type::BPRT_INVALID_ARGUMENT};
			} else if (command == "@reset") return {-1};
			else if (
				command == "@read"
			||	command == "@r"
			) {
				String const param = arg(str, 1);
				if (
					!param.length()
				||	param == "sensors"
				) return {6};
				else if (
					param == "rawldr"
				) return {7};
				else return {Response::Type::BPRT_INVALID_ARGUMENT};
			}
			else return {Response::Type::BPRT_INVALID_COMMAND};
			return {};
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
			Serial.println(max.temperature / 100.0);
			Serial.println("Humidity (%):");
			Serial.print("    MIN: ");
			Serial.println(min.humidity / 100.0);
			Serial.print("    MAX: ");
			Serial.println(max.humidity / 100.0);
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