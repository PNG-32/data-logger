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
				log += "Temperature: [" + String(v.temperature / 100.0) + static_cast<char>(sensor.getUnit()) + "], ";
				log += "Humidity: [" + String(v.humidity / 100.0) + "%], ";
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
			if (Serial.available()) {
				auto str = Serial.readStringUntil('\n');
				str.toLowerCase();
				auto const result = parser.evaluate(str);
				switch (result.type) {
					case ParserType::Response::Type::BPRT_OK: {
						Serial.println("Operation completed successfully.");
						auto i = info.get();
						switch (result.id & 0xF) {
							case static_cast<decltype(result.id)>(-1): Serial.println("Resetting..."); reset();
							case 1: i.clockOK = true;		break;
							case 2: i.temperatureOK = true; break;
							case 3: i.humidityOK = true;	break;
							case 4: i.luminosityOK = true;	break;
							case 5: i.calibrationOK = true;	break;
							case 6: {
								Serial.println(
									Log{
										clock.unixNow(),
										sensor.readRaw(),
										ldr.read()
									}.toString(sensor)
								);
							} break;
							case 7: {
								Serial.print("LDR: ");
								Serial.println(ldr.readRaw());
							}
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
			}
			if (!info.get().ok()) {
				setLights(LightDisplay::BDLLD_CONFIG_ERROR);
				return;
			}
			if (!screenCooldown) {
				bool const inTheDangerZone = sensor.inTheDangerZone() || ldr.inTheDangerZone();
				if (inTheDangerZone) {
					if (!cooldown) {
						db.record({
							clock.unixNow(),
							sensor.readRaw(),
							ldr.read()
						});
						if (db.empty()) Serial.println(":(");
						cooldown = 15;
					}
					setLights(LightDisplay::BDLLD_EMERGENCY);
				} else {
					cooldown = 1;
					setLights(LightDisplay::BDLLD_OK);
				}
				if (cooldown) --cooldown;
			}
			updateScreen();
			Wait::millis(1);
		}

	private:
		/// @brief Updates the data logger's display.
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
			display.clear();
			display.write(ts);
			auto const v = sensor.read();
			if (switchTimer) --switchTimer;
			else {
				switchTimer = 5;
				showHumidity = !showHumidity;
			}
			if (showHumidity) {
				display.setCursorPosition(0, 1);
				display.write("Luminosity: ");
				writeInt(ldr.read(), '%', 11, 1);
			} else {
				writeDecimal(v.temperature, static_cast<char>(sensor.getUnit()), 1, 1);
				writeDecimal(v.humidity, '%', 8, 1);
			}
			tone(alarm, NOTE_C5, 100);
		}

		/// @brief Writes an integer to the display.
		/// @param val Value to write.
		/// @param append Character to append.
		/// @param x X position to write in.
		/// @param y Y position to write in.
		/// @param maxDigits Maximum amount of digits to write.
		void writeInt(int32 const val, char const append, uint8 const x, uint8 const y) {
			display.setCursorPosition(x, y);
			if (val < 0) {
				display.write('-');
				return writeInt(-val, append, x+1, y);
			} else {
				display.write(' ');
				if (val < 100)
					display.write('0');
				if (val < 10)
					display.write('0');
				display.write(val, 10);
				display.write(append);
			}
		}
		
		/// @brief Writes a decimal value to the display.
		/// @param cents Value (in cents) to write.
		/// @param append Character to append.
		/// @param x X position to write in.
		/// @param y Y position to write in.
		void writeDecimal(int32 const cents, char const append, uint8 const x, uint8 const y) {
			display.setCursorPosition(x, y);
			if (cents < 0) {
				display.write('-');
				return writeDecimal(-cents, append, x+1, y);
			} else {
				display.write(' ');
				auto const
					num = static_cast<uint32>(cents) / 100,
					frac = static_cast<uint32>(cents) % 100
				;
				if (num < 10)
					display.write('0');
				display.write(num, 10);
				display.write('.');
				display.write(frac / 10, 10);
				display.setCursorPosition(x + 5, y);
				display.write(append);
			}
		}

		/// @brief Sets the light indicator state.
		/// @param lights State to set to.
		void setLights(LightDisplay const lights) {
			digitalWrite(led.red,		LOW);
			digitalWrite(led.yellow,	LOW);
			digitalWrite(led.green,		LOW);
			switch (lights) {
				case (LightDisplay::BDLLD_OK):				digitalWrite(led.green,		HIGH);	break;
				case (LightDisplay::BDLLD_CONFIG_ERROR):	digitalWrite(led.yellow,	HIGH);	break;
				case (LightDisplay::BDLLD_EMERGENCY):		digitalWrite(led.red,		HIGH);	break;
			}
		}

		bool showHumidity = false;
		uint32 switchTimer = 5;
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
