#include "bits/bits.hpp"

Bits::Display display;

void setup() {
	display.begin();
	display.setDisplay(Bits::Display::State::BDS_ON);
	//display.setCursor(Bits::Display::Cursor::BDC_BLINKING);
	display.setCursorPosition(0, 0);
	display.write("[=++==+==+==++=]");
	display.setCursorPosition(0, 1);
	display.write("[=++==+==+==++=]");
	for (usize i = 0; i < 16; ++i)
		display.scroll(Bits::Display::TextDirection::BDTD_RIGHT);
}
void loop() {
	for (usize i = 0; i < 32; ++i) {
		Bits::Wait::seconds(0.5);
		display.scroll(Bits::Display::TextDirection::BDTD_LEFT);
	}
  Bits::Wait::seconds(0.5);
	for (usize i = 0; i < 32; ++i)
		display.scroll(Bits::Display::TextDirection::BDTD_RIGHT);
}