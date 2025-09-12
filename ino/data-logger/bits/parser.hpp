#ifndef BITS_PARSER_H
#define BITS_PARSER_H

namespace Bits {
	struct Parser {
		Clock&	clock;
		Sensor&	sensor;
		LDR&	ldr;

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

		bool evaluate(String const& str) {
			if (str.empty()) return Error::BPE_MISSING_COMMAND;
			String const command = arg(data, 0);
			if (command[0] != '@') return Error::BPE_NOT_A_COMMAND;
			if (
				command == "@set"
			||	command == "@s"
			) {
				String const param = arg(data, 1);
				if (param.empty()) return Error::BPE_MISSING_ARGUMENT;
				if (
					param == "temperature"
				||	param == "temp"
				||	param == "t"
				) {
					String const min = arg(data, 2);
					String const max = arg(data, 3);
					if (
						min.empty()
					||	max.empty()
					) return Error::BPE_MISSING_VALUE;
					auto v = sensor.getThreshold();
					v.min.temperature = min.toFloat() * 100;
					v.max.temperature = max.toFloat() * 100;
					sensor.setThreshold(v);
				} else if (
					param == "humidity"
				||	param == "h"
				) {
					String const min = arg(data, 2);
					String const max = arg(data, 3);
					if (
						min.empty()
					||	max.empty()
					) return Error::BPE_MISSING_VALUE;
					auto v = sensor.getThreshold();
					v.min.humidity = min.toFloat() * 100;
					v.max.humidity = max.toFloat() * 100;
					sensor.setThreshold(v);
				} else if (
					param == "datetime"
				||	param == "dt"
				||	param == "d"
				) {
					String const dt = arg(data, 2);
					if (dt.empty())
						return Error::BPE_MISSING_VALUE;
					auto const date = DateTime(dt.data());
					if (!date.isValid()) reutn Error::BPE_INVALID_VALUE;
					clock.adjust(date);
				} else if (
					param == "timezone"
				||	param == "zone"
				||	param == "z"
				) {
					String const zone = arg(data, 2);
					if (zone.empty())
						return Error::BPE_MISSING_VALUE;
					String const hour = arg(zone, 0, ':'), minute = arg(zone, 1, ':');
					if (
						hour.empty()
					||	minute.empty()
					) return Error::BPE_MISSING_VALUE;
					int8 const z = (hour.toInt() / 15) + minute.toInt();
					clock.setTimeZone(z);
				} else if (
					param == "luminosity"
				||	param == "ldr"
				||	param == "l"
				) {
					String const min = arg(data, 2);
					String const max = arg(data, 3);
					if (
						min.empty()
					||	max.empty()
					) return Error::BPE_MISSING_VALUE;
					auto v = ldr.getThreshold();
					v.min = min.toInt();
					v.max = max.toInt();
					sensor.setThreshold(v);
				} else return Error::BPE_INVALID_ARGUMENT;
			} else if (
				command == "@calibrate"
			||	command == "@c"
			) {

			} else if (
				command == "@view"
			||	command == "@v"
			) {
				String const param = arg(data, 1);
				if (param.empty()) return Error::BPE_MISSING_ARGUMENT;
				if (
					param == "log"
				||	param == "l"
				) {

				} else if (
					param == "thresholds"
				||	param == "t"
				) {

				} else if (
					param == "timezone"
				||	param == "z"
				) {

				} else return Error::BPE_INVALID_ARGUMENT;
			} else return return Error::BPE_INVALID_COMMAND;
			return Error::BPE_OK;
		}

	private:
		// https://stackoverflow.com/questions/9072320/split-string-into-string-array
		static String arg(String const& data, usize const index, char const sep = ' ') {
			ssize found = 0;
			ssize strIndex[] = {0, -1};
			ssize maxIndex = data.length()-1;
			for(int i=0; i<=maxIndex && found<=index; i++){
				if(data.charAt(i)==seep || i==maxIndex){
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