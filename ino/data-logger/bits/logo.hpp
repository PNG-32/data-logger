#ifndef BITS_LOGO_H
#define BITS_LOGO_H

#include "display.hpp"
#include "core.hpp"

namespace Bits {
	struct Logo {
		Display& display;

		Logo(Display& display): display(display) {}

		void begin() {
			display.addCharacter(image01);
			display.addCharacter(image02);
			display.addCharacter(image03);
			display.addCharacter(image17);
			display.addCharacter(image19);
			display.addCharacter(image18);
		}

		void animate() {
			display.clear();
			display.setCursorPosition(2, 0);
			display.write("\x01\x02\x03");
			display.write("CHRONO");
			display.setCursorPosition(2, 1);
			display.write("\x04\x06\x05");
			display.setCursorPosition(11, 0);
			display.write("\x01\x02\x03");
			display.setCursorPosition(11, 1);
			display.write("\x04\x06\x05");
			for (usize i = 0; i < 16; ++i)
				display.scroll(Display::TextDirection::BDTD_LEFT);
			for (usize i = 0; i < 16; ++i) {
				display.scroll(Display::TextDirection::BDTD_RIGHT);
				Wait::seconds(0.5);
			}
			Wait::seconds(0.5);
			display.clear();
		}

	private:
		Display::Character const image01 = {1, {B11111, B11111, B01110, B01110, B01101, B01101, B01100, B01100}};
		Display::Character const image02 = {2, {B11111, B11111, B00000, B11111, B00000, B11111, B10001, B01110}};
		Display::Character const image03 = {3, {B11111, B11111, B01110, B01110, B10110, B10110, B00110, B00110}};
		Display::Character const image17 = {4, {B01100, B01100, B01101, B01101, B01110, B01110, B11111, B11111}};
		Display::Character const image19 = {5, {B00110, B00110, B10110, B10110, B01110, B01110, B11111, B11111}};
		Display::Character const image18 = {6, {B01110, B10001, B11111, B00000, B11111, B00000, B11111, B11111}};
	};
}

#endif