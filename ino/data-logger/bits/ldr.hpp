#ifndef BITS_LDR_H
#define BITS_LDR_H

#include "types.hpp"
#include "record.hpp"

namespace Bits {
	struct LDR {
		struct PACKED Adjustment {
			uint16 min = 0, max = 1023
		};

		struct PACKED Info {
			Adjustment adjustment;
		};

		LDR(avr_pin const pin, eeprom_address const info):
			pin(pin), info(info) {
		}

		uint8 read() const {
			uint16 const v = readRaw();
			auto const i = info.get();
			return constrain(map(v, i.adjustment.min, i.adjustment.max, 0, 255), 0, 255);
		}

		uint16 readRaw() const {
			return analogRead(pin);
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
