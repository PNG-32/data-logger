#ifndef BITS_CLOCK_H
#define BITS_CLOCK_H

#include "types.hpp"
#include "record.hpp"

#include <RTClib.h>

namespace Bits {
	struct Clock {
		struct Info {
			int8 timeZone = 0;
		};

		Clock(uint8 const pin, uint16 const info):
			pin(pin), info(info) {
			rtc.begin();
		}

		uint32 unixNow() const {
			return rtc.now().unixtime() + timeZoneOffset();
		}

		DateTime now() const {
			return DateTime(unixNow());
		}

		int8 getTimeZone() const {
			return info.get().timeZone;
		}
		
		void setTimeZone(int8 const timeZone) const {
			Info i = info;
			i.timeZone = timeZone;
			info = i;
		}

		void adjust(DateTime const& dt) {
			rtc.adjust(dt);
		}

	private:
		int32 timeZoneOffset() const {
			return static_cast<int32>(getTimeZone()) * (3600 / 4);
		}

		uint8			pin;
		Record<Info>	info;
		RTC_DS1307		rtc;
	};
}

#endif
