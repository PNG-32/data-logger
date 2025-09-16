#ifndef BITS_DISPLAY_H
#define BITS_DISPLAY_H

#include "types.hpp"

#include <LiquidCrystal_I2C.h>

namespace Bits {
	struct Display {
		enum class Cursor: uint8 {
			BDC_OFF,
			BDC_ON,
			BDC_BLINKING
		};

		enum class State: bool {
			BDS_OFF,
			BDS_ON
		};

		enum class TextDirection: bool {
			BDTD_LEFT,
			BDTD_RIGHT
		};

		struct Character {
			byte		id: 3;
			As<byte[8]>	data;
		};

		Display(uint8 const cols = 16, uint8 const rows = 2): lcd(0x27, cols, rows) {}

		void begin() {
			lcd.init();
			setDisplay(State::BDS_OFF);
		}

		void setCursor(Cursor const cursor) {
			switch (cursor) {
				case (Cursor::BDC_OFF):			lcd.noCursor();					break;
				case (Cursor::BDC_ON):			lcd.cursor();	lcd.noBlink();	break;
				case (Cursor::BDC_BLINKING):	lcd.cursor();	lcd.blink();	break;
			}
		}

		void setCursorPosition(uint8 const x, uint8 const y) {
			lcd.setCursor(x, y);
		}

		void setDisplay(State const state) {
			switch (state) {
				case (State::BDS_OFF):	lcd.noDisplay(); lcd.noBacklight();	break;
				case (State::BDS_ON):	lcd.display(); lcd.backlight();		break;
			}
		}

		void setTextAlignment(TextDirection const direction) {
			switch (direction) {
				case (TextDirection::BDTD_LEFT):	lcd.leftToRight();	break;
				case (TextDirection::BDTD_RIGHT):	lcd.rightToLeft();	break;
			}
		}

		void scroll(TextDirection const direction) {
			switch (direction) {
				case (TextDirection::BDTD_LEFT):	lcd.scrollDisplayLeft();	break;
				case (TextDirection::BDTD_RIGHT):	lcd.scrollDisplayRight();	break;
			}
		}

		void setAutoscroll(State const state) {
			switch (state) {
				case (State::BDS_OFF):	lcd.autoscroll();	break;
				case (State::BDS_ON):	lcd.noAutoscroll();	break;
			}
		}

		void addCharacter(Character const character) {
			lcd.createChar(character.id, character.data);
		}

		void write(char const c) {
			lcd.write(c);
		}

		void write(String const str) {
			lcd.print(str);
		}

		template <class T>
		void write(T const v, usize const base) {
			lcd.print(v, base);
		}

		void clear() {lcd.clear();}

	private:
		LiquidCrystal_I2C lcd;
	};
}

#endif