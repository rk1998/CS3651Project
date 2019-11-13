#include <Wire.h>
#include <SparkFunDS1307RTC.h>

#include <HX711.h>
#include <Stepper.h>
#include <LiquidCrystal.h>

const int rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8;
const int scale_dout = 3;
const int scale_clk = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int motor1 = 7;
const int motor2 = 6;
const int motor3 = 5;
const int motor4 = 4;


const int setWeightButton = 15;
const int setTimeButton = 16;
const int rtc_sqw = 14;


Stepper motor = Stepper(4, motor1, motor2, motor3, motor4);

int weightVal = 0;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("Current Time: ");
  //pinMode(weightButton, INPUT_PULLUP);
  int hour = 0;
  int minute = 5;
  int second = 10;
  int day = 4;
  int date = 13;
  int month = 11;
  int year = 19;
  rtc.setTime(second, minute, hour, day, date, month, year);
  //rtc.autoTime();
  //motor.setSpeed(5000);
}

void loop() {
  rtc.update();
  int sec = rtc.second();
  int m = rtc.minute();
  int h = rtc.hour();
  lcd.setCursor(1, 1);
  lcd.print(String(h) + ":" + String(m) + ":" + String(sec));
//  int buttonVal = digitalRead(weightButton);
//  delay(1000);
//  if(buttonVal == LOW) {
//    weightVal += 1;
//  }
//  lcd.print(weightVal);
  //motor.step(1);
  
}
