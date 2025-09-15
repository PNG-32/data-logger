#ifndef BITS_DATALOGGER_H
#define BITS_DATALOGGER_H

#include "types.hpp"
#include "sensor.hpp"
#include "clock.hpp"
#include "ldr.hpp"
#include "databank.hpp"
#include "display.hpp"
#include "parser.hpp"
#include "core.hpp"
#include "pitch.hpp"

/// @brief Helper classes & functions.
namespace Bits {
	/// @brief Data logger implementation.	
	struct DataLogger {
		/// @brief Light display status information.
		enum class LightDisplay {
			BDLLD_CONFIG_ERROR,
			BDLLD_OK,
			BDLLD_EMERGENCY,
		};

		/// @brief Data logger information.
		struct PACKED Info {
			/// @brief Whether the clock was configured correctly.
			byte clockOK:		1;
			/// @brief Whether the sensor's temperature thresholds was configured correctly.
			byte temperatureOK:	1;
			/// @brief Whether the sensor's humidity thresholds was configured correctly.
			byte humidityOK:	1;
			/// @brief Whether the LDR's luminosity thresholds was configured correctly.
			byte luminosityOK:	1;
			/// @brief Whether the LDR was calibrated correctly.
			byte calibrationOK:	1;

			/// @brief Returns whether everything was configured correctly.
			constexpr bool ok() {
				return (
					clockOK
				&&	temperatureOK
				&&	humidityOK
				&&	luminosityOK
				&&	calibrationOK
				);
			}
		};

		/// @brief Data logger log.
		struct PACKED Log {
			/// @brief UNIX timestamp of the log's date.
			uint32			timestamp;
			/// @brief Temperature & humidity values.
			Sensor::Value	value;
			/// @brief Luminosity.
			int16			luminosity;

			/// @brief Returns the log as a string.
			/// @param sensor Sensor to get current display unit from.
			/// @return Log as string
			String toString(Sensor& sensor) const {
				String log = "[";
				log += DateTime(timestamp).timestamp() + "]: { ";
				auto const v = sensor.toCurrentUnit(value);
				log += "Temperature: [" + String(v.temperature) + static_cast<char>(sensor.getUnit()) + "], ";
				log += "Humidity: [" + String(v.humidity) + "%], ";
				log += "Luminosity: [" + String(luminosity) + "%] };";
				return log;
			}
		};

		/// @brief Serial interface parser type.
		using ParserType = Parser<Log>;

		/// @brief LED indicator pins.
		struct LEDPins {
			avr_pin red, yellow, green;
		};

		/// @brief Constructs the data logger.
		/// @param sensorPin Temperature & humidity sensor pin.
		/// @param ldrPin LDR pin.
		/// @param ledPins LED pins.
		/// @param alarmPin Alarm pin.	
		DataLogger(
			avr_pin const	sensorPin	= 2,
			avr_pin const	ldrPin		= A0,
			LEDPins const&	ledPins		= {3, 4, 5},
			avr_pin const	alarmPin	= 6
		):
			info(0),
			clock(info.address() + sizeof(Info)),
			sensor(sensorPin, clock.end()),
			ldr(ldrPin, sensor.end()),
			db(ldr.end()),
			display(),
			led(ledPins),
			alarm(alarmPin),
			parser{db, clock, sensor, ldr} {}

		/// @brief Initializes the data logger.
		void begin() {
			Serial.begin(9600);
			EEPROM.begin();
			sensor.begin();
			clock.begin();
			ldr.begin();
			display.begin();
			db.begin();
			info.begin();
			if (!clock.adjusted())
				clock.adjust({F(__DATE__), F(__TIME__)});
			pinMode(led.red,	OUTPUT);
			pinMode(led.yellow,	OUTPUT);
			pinMode(led.green,	OUTPUT);
			pinMode(led.green,	OUTPUT);
			pinMode(alarm,		OUTPUT);
			display.setDisplay(Display::State::BDS_OFF);
			Wait::seconds(1);
			display.setDisplay(Display::State::BDS_ON);
			Serial.println("Command-line ready.");
		}

		/// @brief Updates the data logger.
		void update() {
			if ((Serial.peek() != '\n' && Serial.peek() != '\0')) {
				auto str = Serial.readStringUntil('\n');
				str.toLowerCase();
				auto const result = parser.evaluate(str);
				switch (result.type) {
					case ParserType::Response::Type::BPRT_OK: {
						Serial.println("Operation completed successfully.");
						auto i = info.get();
						switch (result.id) {
							case static_cast<decltype(result.id)>(-1): reset();
							case 1: i.clockOK = true;		break;
							case 2: i.temperatureOK = true; break;
							case 3: i.humidityOK = true;	break;
							case 4: i.luminosityOK = true;	break;
							case 5: i.calibrationOK = true;	break;
							default: break;
						}
						info = i;
					} break;
					case ParserType::Response::Type::BPRT_NOT_A_COMMAND:	{Serial.println("Not a command string. Commands start with '@'.");									} break;
					case ParserType::Response::Type::BPRT_MISSING_COMMAND: 	{																									} break;
					case ParserType::Response::Type::BPRT_INVALID_COMMAND:	{Serial.println("Unknown/Invalid/Unsupported command. Did you perhaps misspell it?");				} break;
					case ParserType::Response::Type::BPRT_MISSING_ARGUMENT:	{Serial.println("Missing necessary argument(s) for command.");										} break;
					case ParserType::Response::Type::BPRT_INVALID_ARGUMENT:	{Serial.println("Unknown/Invalid/Unsupported argument(s). Did you perhaps misspell one or more?");	} break;
					case ParserType::Response::Type::BPRT_MISSING_VALUE:	{Serial.println("Missing necessary value(s) for command.");											} break;
					case ParserType::Response::Type::BPRT_INVALID_VALUE:	{Serial.println("Invalid value(s) for argument. Did you perhaps mistype/misarrange them?");			} break;
					default: Serial.println("Unknown error. Sorry :/"); break;
				}
				Serial.println("Command-line ready.");
				// TODO: the rest of the parser stuff
			}
			if (!info.get().ok()) {
				setLights(LightDisplay::BDLLD_CONFIG_ERROR);
				return;
			}
			bool const inTheDangerZone = sensor.inTheDangerZone()/* || ldr.inTheDangerZone()*/;
			if (inTheDangerZone) {
				if (!cooldown) {
					db.record({
						clock.unixNow(),
						sensor.readRaw(),
						ldr.read()
					});
					cooldown = 15000;
				}
				setLights(LightDisplay::BDLLD_EMERGENCY);
			} else {
				cooldown = 100;
				setLights(LightDisplay::BDLLD_OK);
			}
			if (cooldown) --cooldown;
			updateScreen();
			Wait::millis(1);
		}

	private:
		void updateScreen() {
			if (screenCooldown) {
				--screenCooldown;
				return;
			}
			screenCooldown = 1000;
			auto const now = clock.now();
			String const ts = 
				now.timestamp(DateTime::TIMESTAMP_DATE)
			+	" "
			+	now.timestamp(DateTime::TIMESTAMP_TIME)
			;
			display.setCursorPosition(0, 0);
			display.write(ts);
			auto const v = sensor.read();
			writeDecimal(v.temperature, static_cast<char>(sensor.getUnit()), 0, 1);
			writeDecimal(v.humidity, '%', 6, 1);
			writeInt(ldr.read(), '%', 12, 1);
		}

		void writeInt(int32 const val, char const append, uint8 const x, uint8 const y, uint8 const maxDigits = 2) {
			if (val < 0) {
				display.write('-');
				return writeInt(-val, append, x+1, y);
			} else {
				auto v = val;
				uint8 digits = 0;
				uint32 max = 1;
				while (v > 9 && digits < maxDigits) {
					v /= 10;
					max *= 10;
					++digits;
				}
				display.setCursorPosition(x + (maxDigits - digits), y);
				display.write(static_cast<uint32>(val) % max, 10);
				display.setCursorPosition(x + maxDigits, y);
				display.write(append);
			}
		}
		
		void writeDecimal(int32 const cents, char const append, uint8 const x, uint8 const y) {
			if (cents < 0) {
				display.write('-');
				return writeDecimal(-cents, append, x+1, y);
			} else {
				display.setCursorPosition(x, y);
				auto const
					num = static_cast<uint32>(cents) / 100,
					frac = static_cast<uint32>(cents) % 100
				;
				if (num < 10)
					display.write('0');
				display.write(num, 10);
				display.write('.');
				display.write(frac / 10, 10);
				display.setCursorPosition(x + 4, y);
				display.write(append);
			}
		}

		void setLights(LightDisplay const lights) {
			digitalWrite(led.red,		LOW);
			digitalWrite(led.yellow,	LOW);
			digitalWrite(led.green,		LOW);
			tone(alarm, );
			switch (lights) {
				case (LightDisplay::BDLLD_OK):				digitalWrite(led.green,		HIGH);	break;
				case (LightDisplay::BDLLD_CONFIG_ERROR):	digitalWrite(led.yellow,	HIGH);	break;
				case (LightDisplay::BDLLD_EMERGENCY):		digitalWrite(led.red,		HIGH);	break;
			}
		}

		Record<Info>	info;
		Clock			clock;
		Sensor			sensor;
		LDR				ldr;
		DataBank<Log>	db;
		Display			display;
		uint16			cooldown		= 0;
		uint16			screenCooldown	= 0;
		LEDPins			led;
		ParserType		parser;
		avr_pin			alarm;
	};
}

#endif
