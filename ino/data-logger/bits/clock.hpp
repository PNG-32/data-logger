#ifndef BITS_CLOCK_H
#define BITS_CLOCK_H

#include "types.hpp"
#include "record.hpp"

#include <RTClib.h>

/// @brief Helper classes & functions.
namespace Bits {
	/// @brief External clock.
	struct Clock {
		/// @brief Clock information, to be stored in the EEPROM.		
		struct PACKED Info {
			/// @brief Whether the clock's current time and time zone has been set.
			int8 adjusted: 1;
			/// @brief Time zone.
			int8 timeZone: 7;
		};

		/// @brief Constructs the clock.
		/// @param info EEPROM address of the clock's info.
		Clock(eeprom_address const info):
			info(info) {}

		/// @brief Initializes the clock.	
		void begin() {
			rtc.begin();
			info.begin();
		}

		/// @brief Returns the current time as a UNIX timestamp.
		/// @return Current time as UNIX timestamp.
		uint32 unixNow() const {
			return rtc.now().unixtime() + timeZoneOffset();
		}

		/// @brief Returns the current time as a `DateTime`.
		/// @return Current time as `DateTime`.
		DateTime now() const {
			return DateTime(unixNow());
		}

		/// @brief Returns the current time zone, in quarter-of-hours.
		/// @return Current time zone in quarter-of-hours.
		int8 getTimeZone() const {
			return info.get().timeZone;
		}
		
		/// @brief Sets the current time zone.
		/// @param timeZone Current time zone, in quarter-of-hours.
		void setTimeZone(int8 const timeZone) const {
			Info i = info;
			i.timeZone = timeZone;
			info = i;
		}

		/// @brief Adjusts the current date and time.
		/// @param dt Date and time to set.
		void adjust(DateTime const& dt) {
			rtc.adjust({dt.unixtime() - timeZoneOffset()});
			if (!info.get().adjusted) {
				Info i = info;
				i.adjusted = true;
				info = i;
			}
		}

		/// @brief Returns whether the clock's current time and time zone has ben set.
		bool adjusted() const {
			return info.get().adjusted;
		}

		/// @brief Returns the clock's address in the EEPROM.
		uint16 address() const	{return info.address();				}
		/// @brief Returns one-past-the-end of the clock's address in the EEPROM.
		uint16 end() const		{return address()	+ sizeof(Info);	}

	private:
		/// @brief Returns the time zone as an UNIX timestamp offset.
		int32 timeZoneOffset() const {
			return static_cast<int32>(getTimeZone()) * (3600 / 4);
		}

		/// @brief Pin the external clock is located in.
		uint8			pin;
		/// @brief Clock information.
		Record<Info>	info;
		/// @brief Underlying Real-Time Clock.
		RTC_DS1307		rtc;
	};
}

#endif
