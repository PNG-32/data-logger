#ifndef BITS_LDR_H
#define BITS_LDR_H

#include "types.hpp"
#include "record.hpp"

namespace Bits {
	struct LDR {
		struct PACKED Adjustment {
			uint16 min = 0, max = 1023;

			constexpr bool valid() const	{return (min < max);	}
			constexpr bool invalid() const	{return !valid();		}
		};

		struct PACKED Threshold {
			uint16 min = 0, max = 1023;
		};

		struct PACKED Info {
			Adjustment	adjustment;
			Threshold	threshold;
		};

		LDR(avr_pin const pin, eeprom_address const info):
			pin(pin), info(info) {
		}

		void begin() {pinMode(pin, INPUT);}

		int16 read() const {
			auto const v	= readRaw();
			auto const adj	= info.get().adjustment;
			return constrain(map(v, adj.min, adj.max, 0, 100), 0, 100);
		}

		int16 readRaw() const {
			return analogRead(pin);
		}

		Adjustment getAdjustment() const {
			return info.get().adjustment;
		}

		void setAdjustment(Adjustment const& adjustment) {
			Info i = info;
			i.adjustment = adjustment;
			info = i;
		}

		Threshold getThreshold() const {
			return info.get().threshold;
		}

		void setThreshold(Threshold const& threshold) {
			Info i = info;
			i.threshold = threshold;
			info = i;
		}

		bool inTheDangerZone() const {
			return !inTheSafeZone();
		}

		bool inTheSafeZone() const {
			auto const v			= read();
			auto const threshold	= info.get().threshold;
			return (
				threshold.min <= v
			&&	v <= threshold.max
			);
		}

		uint16 address() const	{return info.address();				}
		uint16 end() const		{return address()	+ sizeof(Info);	}

	private:
		uint8 const		pin;
		Record<Info>	info;
	};
}

#endif
