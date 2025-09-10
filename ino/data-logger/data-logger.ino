#include "bits/bits.hpp"

Bits::Display display;

void setup() {
  display.begin();
	display.setDisplay(Bits::Display::State::BDS_ON);
	display.setCursor(Bits::Display::Cursor::BDC_BLINKING);
}
void loop() {
	display.setCursorPosition(0, 0);
	display.write("What are you looking at?");
	Bits::Wait::seconds(1);
}