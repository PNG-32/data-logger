#ifndef BITS_DATALOGGER_H
#define BITS_DATALOGGER_H

#include "types.hpp"
#include "sensor.hpp"
#include "clock.hpp"
#include "ldr.hpp"
#include "databank.hpp"
#include "display.hpp"

namespace Bits {
	struct DataLogger {
		enum class LightDisplay {
			BDLLD_CONFIG_ERROR,
			BDLLD_OK,
			BDLLD_EMERGENCY,
		};

		struct PACKED Info {
			uint8 clockOK:			1;
			uint8 temperatureOK:	1;
			uint8 humidityOK:		1;
			uint8 luminosityOK:		1;
			constexpr bool ok() {
				return (
					clockOK
				&&	temperatureOK
				&&	humidityOK
				&&	luminosityOK
				);
			}
		};

		struct PACKED Log {
			uint32			timestamp;
			Sensor::Value	value;
			uint8_t			luminosity;
		};

		struct LEDPins {
			avr_pin
				red,
				yellow,
				green
			;
		};

		DataLogger(
			avr_pin const	sensorPin	= 2,
			avr_pin const	ldrPin		= A0,
			LEDPins const&	ledPins		= {3, 4, 5}
		):
			info(0),
			clock(info.address() + sizeof(Info)),
			sensor(sensorPin, clock.end()),
			ldr(ldrPin, sensor.end()),
			db(ldr.end()),
			display(),
			led(ledPins) {}

		void begin() {
			Serial.begin(9600);
			clock.begin();
			ldr.begin();
			display.begin();
			if (!clock.adjusted())
				clock.adjust({F(__DATE__), F(__TIME__)});
			pinMode(led.red,	OUTPUT);
			pinMode(led.yellow,	OUTPUT);
			pinMode(led.green,	OUTPUT);
		}

		void update() {
			if (!info.get().ok()) {
				setLights(LightDisplay::BDLLD_CONFIG_ERROR);
				return;
			}
			bool const inTheDangerZone = sensor.inTheDangerZone() || ldr.inTheDangerZone();
			if (inTheDangerZone) {
				if (!cooldown) {
					db.record({
						clock.unixNow(),
						sensor.readRaw(),
						ldr.read()
					});
					cooldown = 1000;
				}
				setLights(LightDisplay::BDLLD_EMERGENCY);
			} else setLights(LightDisplay::BDLLD_OK);
			if (cooldown) --cooldown;
			updateScreen();
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
			writeDecimal(v.temperature, sensor.getUnit(), 0, 1);
			writeDecimal(v.humidity, '%', 6, 1);
			writeInt(ldr.read(), '%', 12, 1);
		}

		void writeInt(int32 const val, char const append, uint8 const x, uint8 const y, uint8 const maxDigits = 2) {
			if (cents < 0) {
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
				display.writeNumber(static_cast<uint32>(val) % max, 10);
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
				display.writeNumber(num, 10);
				display.write('.');
				if (frac < 10)
					display.write('0');
				display.writeNumber(frac, 10);
				if (frac % 10 && frac > 0)
					display.write('0');
				display.setCursorPosition(x + 5, y);
				display.write(append);
			}
		}

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

		Record<Info>	info;
		Clock			clock;
		Sensor			sensor;
		LDR				ldr;
		DataBank<Log>	db;
		Display			display;
		uint16			cooldown		= 0;
		uint16			screenCooldown	= 0;
		LEDPins			led;

	};
}

#endif
