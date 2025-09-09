#include "bits/bits.hpp"

#include <LiquidCrystal_I2C.h>

#define LCD_ADDRESS (0x27)

LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2);

void image1() {
  lcd.clear();

  byte image01[8] = {B11111, B11111, B01110, B01110, B01101, B01101, B01100, B01100};
  byte image02[8] = {B11111, B11111, B00000, B11111, B00000, B11111, B10001, B01110};
  byte image03[8] = {B11111, B11111, B01110, B01110, B10110, B10110, B00110, B00110};
  byte image17[8] = {B01100, B01100, B01101, B01101, B01110, B01110, B11111, B11111};
  byte image19[8] = {B00110, B00110, B10110, B10110, B01110, B01110, B11111, B11111};
  byte image18[8] = {B01110, B10001, B11111, B00000, B11111, B00000, B11111, B11111};


  lcd.createChar(0, image01);
  lcd.createChar(1, image02);
  lcd.createChar(2, image03);
  lcd.createChar(3, image17);
  lcd.createChar(4, image19);
  lcd.createChar(5, image18);


  lcd.setCursor(0, 0);
  lcd.write(byte(0));
  lcd.setCursor(1, 0);
  lcd.write(byte(1));
  lcd.setCursor(2, 0);
  lcd.write(byte(2));
  lcd.setCursor(0, 1);
  lcd.write(byte(3));
  lcd.setCursor(2, 1);
  lcd.write(byte(4));
  lcd.setCursor(1, 1);
  lcd.write(byte(5));
}


void image2() {
  lcd.clear();

  byte image31[8] = {B00000, B00100, B01010, B01010, B01010, B01010, B00100, B00000};
  byte image30[8] = {B00000, B01000, B01000, B01000, B01000, B01000, B01110, B00000};
  byte image32[8] = {B00000, B01110, B01010, B01110, B00010, B01010, B01110, B00000};
  byte image13[8] = {B01110, B11111, B11011, B11011, B11011, B11011, B11111, B01110};
  byte image11[8] = {B01110, B11111, B11011, B11011, B11011, B11011, B11111, B01110};
  byte image12[8] = {B11001, B11001, B11001, B10101, B10101, B10011, B10011, B10011};
  byte image09[8] = {B01111, B11111, B11100, B11000, B11000, B11100, B11111, B01111};
  byte image10[8] = {B11110, B11011, B10001, B11011, B11110, B11110, B11011, B11011};


  lcd.createChar(0, image31);
  lcd.createChar(1, image30);
  lcd.createChar(2, image32);
  lcd.createChar(3, image13);
  lcd.createChar(4, image11);
  lcd.createChar(5, image12);
  lcd.createChar(6, image09);
  lcd.createChar(7, image10);


  lcd.setCursor(14, 1);
  lcd.write(byte(0));
  lcd.setCursor(13, 1);
  lcd.write(byte(1));
  lcd.setCursor(15, 1);
  lcd.write(byte(2));
  lcd.setCursor(12, 0);
  lcd.write(byte(3));
  lcd.setCursor(10, 0);
  lcd.write(byte(4));
  lcd.setCursor(11, 0);
  lcd.write(byte(5));
  lcd.setCursor(8, 0);
  lcd.write(byte(6));
  lcd.setCursor(9, 0);
  lcd.write(byte(7));
}


void setup() {
  lcd.begin(16, 2);
}

void loop() {
  image1();
  delay(2000);
}