
#ifndef BITS_LOGO_H
#define BITS_LOGO_H

#include "display.hpp"
#include "core.hpp"

struct Logo {
	Display& display;
	constexpr static Display::Character image01 = {1, {B11111, B11111, B01110, B01110, B01101, B01101, B01100, B01100}};
	constexpr static Display::Character image02 = {2, {B11111, B11111, B00000, B11111, B00000, B11111, B10001, B01110}};
	constexpr static Display::Character image03 = {3, {B11111, B11111, B01110, B01110, B10110, B10110, B00110, B00110}};
	constexpr static Display::Character image17 = {4, {B01100, B01100, B01101, B01101, B01110, B01110, B11111, B11111}};
	constexpr static Display::Character image19 = {5, {B00110, B00110, B10110, B10110, B01110, B01110, B11111, B11111}};
	constexpr static Display::Character image18 = {6, {B01110, B10001, B11111, B00000, B11111, B00000, B11111, B11111}};

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
		display.write("\x01\x02\x03");
		display.write("CHRONO");
		display.setCursorPosition(0, 1);
		display.write("\x04\x06\x05");
		display.setCursorPosition(13, 0);
		display.write("\x01\x02\x03");
		display.setCursor(13, 1);
		display.write("\x04\x06\x05");
		for (usize i = 0; i < 16; ++i)
			display.scroll(Display::TextDirection::BDTD_LEFT);
		for (usize i = 0; i < 32; ++i) {
			display.scroll(Display::TextDirection::BDTD_RIGHT);
			Wait::seconds(1);
		}
	}
}

#endif